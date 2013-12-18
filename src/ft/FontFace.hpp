
#pragma once

#include "ft/Library.hpp"

namespace ft {

/**
 * FreeType font face wrapper. 
 */
class FontFace {
 public:

  /**
   * Create a FontFace.
   */
  explicit FontFace(std::string const &path,
                    std::size_t size = 30,
                    std::size_t hres = 64);

  /**
   * Free the memory allocated by the font FontFace.
   */
  ~FontFace();

  /**
   * Return the size of the characters.
   */
  std::size_t getCharacterSize() const;

  /**
   * Return the horizontal resolution.
   */
  std::size_t getHorizontalResolution() const;

  /**
   * Return the freetype face pointer. 
   */
  FT_Face get() const;

  /**
   * Return ascender metric.
   */
  float getAscender() const;

  /**
   * Return descender metric.
   */
  float getDescender() const;

  /**
   * Return height metric.
   */
  float getHeight() const;

  /**
   * Return the index for the given charcode.
   */
  unsigned int getCharIndex(wchar_t charcode) const;

  /**
   * Load a glyph from the given index.
   */
  FT_GlyphSlot loadGlyph(unsigned int index,
                         int flags = FT_LOAD_RENDER) const;

 private:
  FT_Face _face;
  std::size_t _size;
  std::size_t _hres;
};

}  // namespace ft
