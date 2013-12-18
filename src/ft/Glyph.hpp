
#pragma once

#include "vector.h"
#include "ft/Kerning.hpp"

namespace ft {

/**
 * @struct Glyph
 * A structure that describe glyph data.
 */
struct Glyph {

  /**
   * Create a glyph.
   */
  Glyph();

  /**
   * Clear all memory allocated by the glyph.
   */
  ~Glyph();

  /**
   * Get the kerning between two horizontal glyphs.
   * @param charcode  codepoint of the preceding glyph
   * @return x kerning value
   */
  float getKerning(const wchar_t charcode) const;

  /** Glyph's charcode. */
  wchar_t charcode;

  /** Glyph's id (for OpenGL). */
  unsigned int id;

  /** Glyph's width in pixels. */
  size_t width;

  /** Glyph's height in pixels. */
  size_t height;

  /** Glyph's left offset in pixels. */
  int offset_x;

  /** Glyph's top offset in pixels. */
  int offset_y;

  /** Glyph's advancement relative to the previous char. */
  float advance_x;

  /** Glyph's advancement relative to the previous line. */
  float advance_y;

  /** Normalized texture coordinates. */
  float s0, t0, s1, t1;

  /**
   * A vector of kerning pairs relative to this glyph.
   */
  vector_t *kerning;
};

}  // namespace ft