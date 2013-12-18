
#pragma once

#include <vector>
#include "ft/ft.hpp"
#include FT_GLYPH_H
#include "ft/Glyph.hpp"
#include "core/TextureAtlas.hpp"

/**
 * This class can generate a texture altas from a list of
 * characters.
 */
class TextureFont {
 public:

  /**
   * Create a font from a filename and the font size.
   */
  explicit TextureFont(core::TextureAtlas *atlas,
                       std::string const &path,
                       float size);

  /**
   * Free all memory allocated by the font. However atlas isnt
   * deleted.
   */
  ~TextureFont();

  /**
   * Set the padding value.
   */
  void setPadding(size_t padding);

  /**
   * Generate the texture from the chars into the texture fonts
   * and others settings (like padding).
   */
  void generate();

  /**
   * Load the font and store data into the given `FT_Face`.
   */
  void _loadFace(FT_Face *face, float k = 1);

  /**
   * Compute the kerning of all characters of the font.
   */
  void _computeKerning();

  /** Generated glyphs from this font. */
  std::vector<ft::Glyph *> _glyphs;

  /** Generated atlas from this font */
  core::TextureAtlas *_atlas;

  /** Path to the font file */
  std::string _path;
  
  /** Path to the font file */
  char *filename;

  /** Cache which contains all chars stored into this texture font. */
  std::wstring _chars;

  size_t _padding;

  /** Font size used. */
  float _size;
  
  /** Whether to use kerning if available. */
  int _kerning;

  /** Default line height used. */
  float _height;

  /**
   * Representation of the space between the top of one letter and
   * the bottom of the other.
   */
  float _linegap;

  /** Distance between the baseline and the top of the highest char. */
  float _ascender;

  /** Distance between the baseline and the bottom of the lowest char. */
  float _descender;
};

