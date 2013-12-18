
#include "ft/Glyph.hpp"

namespace ft {

Glyph::Glyph() :
  width(0), height(0),
  offset_x(0), offset_y(0),
  advance_x(0), advance_y(0),
  s0(0), t0(0), s1(0), t1(0) {
}

Glyph::~Glyph() {}

float Glyph::getKerning(const wchar_t charcode) const {
  for (auto &k : kerning) {
    if (k.charcode == charcode)
      return k.kerning;
  }
  return 0;
}

}  // namespace ft
