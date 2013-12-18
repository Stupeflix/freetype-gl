
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <wchar.h>
#include "ft/Error.hpp"
#include "ft/Library.hpp"
#include "ft/FontFace.hpp"
#include "core/TextureFont.hpp"

TextureFont::TextureFont(core::TextureAtlas &atlas,
                         std::string const &path,
                         float size) :
  _atlas(atlas),
  _path(path),
  _size(size) {
  if (size <= 0)
    throw std::runtime_error("Font size should be positive.");

  /** Load high resolution font face. */
  ft::FontFace face(path, size * 100);
  _cache = face.getCharacters();
  _height = 0;
  _padding = 0;
  _ascender = 0;
  _descender = 0;
  _size = size;
  _ascender = face.getAscender() / 100.0;
  _descender = face.getDescender() / 100.0;
  _height = face.getHeight() / 100.0;
  _linegap = face.getLinegap() / 100.0;
}

TextureFont::~TextureFont() {
  for (auto glyph : _glyphs)
    delete glyph;
}

void TextureFont::setPadding(size_t padding) {
  _padding = padding;
}

void TextureFont::_computeKerning() {
  FT_UInt glyph_index, prev_index;
  ft::Glyph *glyph, *prev_glyph;
  FT_Vector kerning;
  
  /* Load font */
  ft::FontFace face(_path, _size);

  /* Compute kernings. */
  for (size_t i = 1; i < _glyphs.size(); ++i) {
    glyph = _glyphs[i];
    glyph_index = face.getCharIndex(glyph->charcode);
    glyph->kerning.clear();
    for (size_t j = 1; j < _glyphs.size(); ++j) {
      prev_glyph = _glyphs[j];
      prev_index = face.getCharIndex(prev_glyph->charcode);
      Vector2i const &kerning = face.getKerning(prev_index, glyph_index);
      if( kerning.x ) {
        // hres = 64
        ft::Kerning k = {prev_glyph->charcode, kerning.x / (float)(64.0f * 64.0f)};
        glyph->kerning.push_back(k);
      }
    }
  }
}

void TextureFont::generate() {
    ft::FontFace face(_path, _size);
    size_t x = 0;
    size_t y = 0;

    /* Load each glyph */
    for (std::size_t i = 0; i < _cache.size(); ++i) {
        std::size_t glyph_index = face.getCharIndex(_cache[i]);
        if (glyph_index == 0) // skip
            continue;

        FT_GlyphSlot slot = face.loadGlyph(glyph_index);
        std::size_t w = slot->bitmap.width;
        std::size_t h = slot->bitmap.rows;

        ft::Glyph *glyph = new ft::Glyph;
        glyph->charcode = _cache[i];
        glyph->width    = w;
        glyph->height   = h;
        glyph->offset_x = slot->bitmap_left;
        glyph->offset_y = slot->bitmap_top;
        glyph->s0       = x/(float)_atlas.getWidth();
        glyph->t0       = y/(float)_atlas.getHeight();
        glyph->s1       = (x + glyph->width)/(float)_atlas.getWidth();
        glyph->t1       = (y + glyph->height)/(float)_atlas.getHeight();
        glyph->advance_x = slot->advance.x / face.getHorizontalResolution();
        glyph->advance_y = slot->advance.y / face.getHorizontalResolution();
        _glyphs.push_back(glyph);

        Rect const &region = _atlas.getRegion(w + _padding, h + _padding);
        if (region.x < 0)
          throw std::out_of_range("Atlas is full.");
        x = region.x + (_padding >> 1);
        y = region.y + (_padding >> 1);
        _atlas.setRegion(x, y, w, h, slot->bitmap.buffer, slot->bitmap.pitch);
    }
    _computeKerning();
}


