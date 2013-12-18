
#include <iostream>
#include <limits>
#include "ft/FontFace.hpp"

namespace ft {

FontFace::FontFace(std::string const &path, std::size_t size, std::size_t hres) :
  _size(size),
  _hres(hres) {
  FT_Error error;
  FT_Matrix matrix = {(int)((1.0/hres) * 0x10000L),
                      (int)((0.0)       * 0x10000L),
                      (int)((0.0)       * 0x10000L),
                      (int)((1.0)       * 0x10000L)};

  /* Load face */
  error = FT_New_Face(ft::Library::getHandle(),
                      path.c_str(), 0, &_face);
  if (error)
    throw ft::Error(error, __FILE__, __LINE__);

  /* Select charmap */
  error = FT_Select_Charmap(_face, FT_ENCODING_UNICODE);
  if (error)
    throw ft::Error(error, __FILE__, __LINE__);

  /* Set char size */
  error = FT_Set_Char_Size(_face, (int)(size * 64),
                           0, 72 * hres, 72);
  if (error)
    throw ft::Error(error, __FILE__, __LINE__);

  /* Set transform matrix */
  FT_Set_Transform(_face, &matrix, NULL);
}

FontFace::~FontFace() {
  FT_Done_Face(_face);
}

std::size_t FontFace::getCharacterSize() const {
  return _size;
}

std::size_t FontFace::getHorizontalResolution() const {
  return _hres;
}

float FontFace::getAscender() const {
  return _face->size->metrics.ascender >> 6;
}

float FontFace::getDescender() const {
  return _face->size->metrics.descender >> 6;
}

float FontFace::getHeight() const {
  return _face->size->metrics.height >> 6;
}

float FontFace::getLinegap() const {
  return getHeight() - getAscender() + getDescender();
}

unsigned int FontFace::getCharIndex(wchar_t charcode) const {
  return FT_Get_Char_Index(_face, charcode);
}

FT_GlyphSlot FontFace::loadGlyph(unsigned int index, int flags) const {
  FT_Error error = FT_Load_Glyph(_face, index, flags);
  if (error)
    throw ft::Error(error);

  ft::Glyph glyph;


  return _face->glyph;
}

Vector2i FontFace::getKerning(unsigned int prev,
                              unsigned int index,
                              unsigned int mode) const {
  Vector2i v;
  FT_Vector tmp;
  FT_Get_Kerning(_face, prev, index, mode, &tmp);
  v.x = tmp.x;
  v.y = tmp.y;
  return v;
}

std::wstring FontFace::getCharacters() const {
  std::wstring chars;
  wchar_t i = 0;
  wchar_t end = 65536;
  // wchar_t end = std::numeric_limits<wchar_t>::max();
  while (i < end) {
    if (getCharIndex(i) != 0)
      chars += i;
    ++i;
  }
  return chars;
}

}
