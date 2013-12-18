
#include "ft/Error.hpp"
#include "ft/Library.hpp"
#include "ft/FontFace.hpp"

#include <iostream>
#include <stdexcept>
#include <math.h>
#include <wchar.h>
#include "texture-font.h"

static unsigned int
_get_next_power_of_two(unsigned int v)
{
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    return v + 1;
}

static wchar_t *
_get_all_charcodes()
{
    wchar_t *cache = (wchar_t *) malloc(65536 * sizeof(wchar_t));
    wchar_t i;
    if (cache == NULL)
    {
        fprintf(stderr, "Error: Cannot allocate unicode cache.\n");
        return 0;
    }
    for (i = 32; i < 65535; ++i) {
        cache[i - 32] = i;
    }
    cache[i] = 0;
    return cache;
}

static int
_compute_atlas(TextureFont *self, ft::FontFace const &face)
{
    // size_t size = 0;
    // size_t i;

    // for( i=0; i<wcslen(self->_cache); ++i )
    // {
    //     if (self->_cache[i] == '\n')
    //         continue;
    //     if (face.getCharIndex(self->_cache[i]))
    //         size++;
    // }
    size_t size = self->_chars.size();
    size = _get_next_power_of_two((size * self->_size) / 20);
    self->_atlas = new core::TextureAtlas(size, size);
    return 0;
}

TextureFont::TextureFont(core::TextureAtlas *atlas,
                         std::string const &path,
                         float size) :
  _path(path),
  _size(size) {
  if (size <= 0)
    throw std::runtime_error("Font size should be positive.");

  /** Load high resolution font face. */
  ft::FontFace face(path, size * 100);

  _chars = face.getCharacters();
  _cache = _get_all_charcodes();
  // _glyphs = vector_new( sizeof(ft::Glyph *) );
  _atlas = atlas;
  _height = 0;
  _ascender = 0;
  _descender = 0;
  _size = size;
  _kerning = 1;

  if (_atlas == NULL)
    _compute_atlas(this, face);

  _ascender = face.getAscender() / 100.0;
  _descender = face.getDescender() / 100.0;
  _height = face.getHeight() / 100.0;
  _linegap = face.getLinegap() / 100.0;
}

TextureFont::~TextureFont() {
  for (auto glyph : _glyphs)
    delete glyph;
}

ft::Glyph *TextureFont::getGlyph(wchar_t charcode) {
  wchar_t buffer[2] = {0,0};

  /* Check if charcode has been already loaded */
  for(auto glyph : _glyphs) {
    if (glyph->charcode == charcode)
      return glyph;
  }

  /* Glyph has not been already loaded */
  buffer[0] = charcode;
  if (texture_font_load_glyphs(this, buffer) == 0)
    return _glyphs.back();
  return NULL;
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

// ----------------------------------------------- texture_font_load_glyphs ---
size_t
texture_font_load_glyphs( TextureFont * self,
                          const wchar_t * charcodes )
{
    return texture_font_load_glyphs_with_padding(self, charcodes, 1);
}

// ----------------------------------------- texture_font_load_with_padding ---
size_t
texture_font_load_with_padding( TextureFont * self,
                                size_t padding )
{
    return texture_font_load_glyphs_with_padding(self, self->_cache, padding);
}

// ---------------------------------- texture_font_load_glyphs_with_padding ---
size_t
texture_font_load_glyphs_with_padding( TextureFont * self,
                          const wchar_t * charcodes,
                          size_t padding )
{
    size_t x, y, w, h;
    FT_Error error;
    FT_Glyph ft_glyph;
    FT_GlyphSlot slot;
    FT_Bitmap ft_bitmap;

    unsigned int glyph_index;
    ft::Glyph *glyph;
    Vector4i region;
    size_t missed = 0;

    ft::FontFace face(self->_path, self->_size);

    size_t width  = self->_atlas->getWidth();
    size_t height = self->_atlas->getHeight();

    /* Load each glyph */
    for (std::size_t i = 0; i < self->_chars.size(); ++i) {
        glyph_index = face.getCharIndex(self->_chars[i]);
        if (glyph_index == 0) // skip
            continue;

        slot            = face.loadGlyph(glyph_index);
        ft_bitmap       = slot->bitmap;

        w = slot->bitmap.width;
        h = slot->bitmap.rows;
        region = self->_atlas->getRegion(w + padding, h + padding);
        if (region.x < 0) {
          missed++;
          fprintf( stderr, "Texture atlas is full (line %d) (size %lu)\n", __LINE__, i );
          break;
        }
        x = region.x + (padding >> 1);
        y = region.y + (padding >> 1);
        self->_atlas->setRegion(x, y, w, h, ft_bitmap.buffer, ft_bitmap.pitch);

        glyph = new ft::Glyph;
        glyph->charcode = self->_chars[i];
        glyph->width    = w;
        glyph->height   = h;
        glyph->offset_x = slot->bitmap_left;
        glyph->offset_y = slot->bitmap_top;
        glyph->s0       = x/(float)width;
        glyph->t0       = y/(float)height;
        glyph->s1       = (x + glyph->width)/(float)width;
        glyph->t1       = (y + glyph->height)/(float)height;
        glyph->advance_x = slot->advance.x / face.getHorizontalResolution();
        glyph->advance_y = slot->advance.y / face.getHorizontalResolution();
        self->_glyphs.push_back(glyph);
    }
    self->_computeKerning();
    return missed;
}


