
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
   * Request a new glyph from the font. If it has not been created yet, it will
   * be. 
   * @param charcode The code of the character to load.
   * @return A pointer on the new glyph or NULL if the texture atlas isnt big
   * enough.
   */
  ft::Glyph *getGlyph(wchar_t charcode);

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
  wchar_t *_cache;

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

/**
 * Request the loading of several glyphs at once.
 *
 * @param self      a valid texture font
 * @param charcodes character codepoints to be loaded.
 * @param padding   the padding between each letters
 *
 * @return Number of missed glyph if the texture is not big enough to hold
 *         every glyphs.
 */
  size_t
  texture_font_load_glyphs( TextureFont * self,
                            const wchar_t * charcodes );

/**
 * Request the loading of several glyphs at once.
 *
 * @param self      a valid texture font
 * @param charcodes character codepoints to be loaded.
 * @param padding   the padding between each letters
 *
 * @return Number of missed glyph if the texture is not big enough to hold
 *         every glyphs.
 */
  size_t
  texture_font_load_glyphs_with_padding( TextureFont * self,
                            const wchar_t * charcodes,
                            size_t padding );

/**
 * Load all unicodes characters for this font (0 to 65536).
 *
 * @param self      a valid texture font
 * @param padding   the paddding between each letters
 */
size_t
texture_font_load_with_padding( TextureFont * self,
                                size_t padding );

