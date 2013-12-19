
#include <iostream>
#include <stdexcept>
#include <cmath>
#include <wchar.h>
#include "ft/Error.hpp"
#include "ft/Library.hpp"
#include "core/FontTexture.hpp"

namespace core {

FontTexture::FontTexture(core::Atlas &atlas,
                         std::string const &path,
                         float size) :
  _atlas(atlas),
  _face(path, size),
  _path(path),
  _size(size) {
  if (size <= 0)
    throw std::runtime_error("Font size should be positive.");

  _cache = _face.getCharacters();
  _height = 0;
  _padding = 0;
  _ascender = 0;
  _descender = 0;
  _size = size;
  _ascender = _face.getAscender();
  _descender = _face.getDescender();
  _height = _face.getHeight();
  _linegap = _face.getLinegap();
}

FontTexture::~FontTexture() {
  for (auto glyph : _glyphs)
    delete glyph;
}

void FontTexture::setPadding(size_t padding) {
  _padding = padding;
}

void FontTexture::_computeKerning() {
  ft::Glyph *glyph, *prev;
  
  /* Compute kernings. */
  for (size_t i = 1; i < _glyphs.size(); ++i) {
    glyph = _glyphs[i];
    for (size_t j = 1; j < _glyphs.size(); ++j) {
      prev = _glyphs[j];
      Vector2i const &kerning = _face.getKerning(prev->charcode, glyph->charcode);
      if( kerning.x ) {
        // hres = 64
        ft::Kerning k = {prev->charcode, kerning.x / (float)(64.0f * 64.0f)};
        glyph->kerning.push_back(k);
      }
    }
  }
}

void FontTexture::generate() {
  std::size_t s = _cache.size();
  for (std::size_t i = 0; i < s; ++i) {

    /* Load glyphs data from face */
    std::size_t x = 0;
    std::size_t y = 0;
    ft::Glyph *glyph = _face.loadGlyph(_cache[i]);

    /* Place the texture into the altas */
    Rect const &region = _atlas.getRegion(glyph->width + _padding,
                                          glyph->height + _padding);
    if (region.x < 0)
      throw std::out_of_range("Atlas is full.");
    x = region.x + (_padding >> 1);
    y = region.y + (_padding >> 1);
    _atlas.setRegion(x, y, glyph->width, glyph->height,
                     glyph->buffer, glyph->pitch);

    /* Deduce texture coordinates from atlas region */
    glyph->s0 = x / (float)_atlas.getWidth();
    glyph->t0 = y / (float)_atlas.getHeight();
    glyph->s1 = (x + glyph->width) / (float)_atlas.getWidth();
    glyph->t1 = (y + glyph->height) / (float)_atlas.getHeight();
    _glyphs.push_back(glyph);

  }
  _computeKerning();
}

}
