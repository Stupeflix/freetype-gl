
#include "ft/Glyph.hpp"

namespace ft {

Glyph::Glyph() :
  id(0),
  width(0), height(0),
  offset_x(0), offset_y(0),
  advance_x(0), advance_y(0),
  s0(0), t0(0), s1(0), t1(0),
  kerning(vector_new(sizeof(Kerning))) {
}

Glyph::~Glyph() {
  vector_delete(kerning);
}

float Glyph::getKerning(const wchar_t charcode) const {
  for (size_t i = 0; i < vector_size(kerning); ++i) {
    Kerning *k = (Kerning *)vector_get(kerning, i);
    if (k->charcode == charcode)
      return k->kerning;
  }
  return 0;
}

}  // namespace ft
