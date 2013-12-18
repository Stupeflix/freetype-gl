
#pragma once

#include <vector>
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
  explicit TextureFont(core::TextureAtlas &atlas,
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

  std::vector<ft::Glyph *> _glyphs;
  core::TextureAtlas &_atlas;
  std::string _path;
  std::wstring _cache;
  size_t _padding;
  float _size;
  float _height;
  float _linegap;
  float _ascender;
  float _descender;
};

