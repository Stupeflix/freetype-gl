
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <wchar.h>
#include "ft/Error.hpp"
#include "ft/Library.hpp"
#include "ft/FontFace.hpp"
#include "core/TextureFont.hpp"

namespace core {

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
  ft::Glyph *glyph, *prev;
  
  /* Load font */
  ft::FontFace face(_path, _size);

  /* Compute kernings. */
  for (size_t i = 1; i < _glyphs.size(); ++i) {
    glyph = _glyphs[i];
    for (size_t j = 1; j < _glyphs.size(); ++j) {
      prev = _glyphs[j];
      Vector2i const &kerning = face.getKerning(prev->charcode, glyph->charcode);
      if( kerning.x ) {
        // hres = 64
        ft::Kerning k = {prev->charcode, kerning.x / (float)(64.0f * 64.0f)};
        glyph->kerning.push_back(k);
      }
    }
  }
}

void TextureFont::generate() {
    ft::FontFace face(_path, _size);

    for (std::size_t i = 0; i < _cache.size(); ++i) {

      /* Load glyphs data from face */
      std::size_t x = 0;
      std::size_t y = 0;
      ft::Glyph *glyph = face.loadGlyph(_cache[i]);

      /* Place the texture into the altas */
      Rect const &region = _atlas.getRegion(glyph->width + _padding, glyph->height + _padding);
      if (region.x < 0)
        throw std::out_of_range("Atlas is full.");
      x = region.x + (_padding >> 1);
      y = region.y + (_padding >> 1);
      _atlas.setRegion(x, y, glyph->width, glyph->height, glyph->buffer, glyph->pitch);

      /* Deduce texture coordinates from atlas region */
      glyph->s0 = x/(float)_atlas.getWidth();
      glyph->t0 = y/(float)_atlas.getHeight();
      glyph->s1 = (x + glyph->width)/(float)_atlas.getWidth();
      glyph->t1 = (y + glyph->height)/(float)_atlas.getHeight();
      _glyphs.push_back(glyph);

    }
    _computeKerning();
}

}
