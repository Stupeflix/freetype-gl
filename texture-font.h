/* ============================================================================
 * Freetype GL - A C OpenGL Freetype engine
 * Platform:    Any
 * WWW:         http://code.google.com/p/freetype-gl/
 * ----------------------------------------------------------------------------
 * Copyright 2011,2012 Nicolas P. Rougier. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice,
 *     this list of conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY NICOLAS P. ROUGIER ''AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO
 * EVENT SHALL NICOLAS P. ROUGIER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are
 * those of the authors and should not be interpreted as representing official
 * policies, either expressed or implied, of Nicolas P. Rougier.
 * ============================================================================
 */
#ifndef __TEXTURE_FONT_H__
#define __TEXTURE_FONT_H__

#include <stdlib.h>
#include <vector>

#include "vector.h"
#include "ft/Glyph.hpp"
#include "texture-atlas.h"

/**
 * This class can generate a texture altas from a list of
 * characters.
 */
class TextureFont {
 public:

    /**
     * Create a font from a filename and the font size.
     */
    explicit TextureFont(texture_atlas_t *atlas,
                         std::string const &path,
                         float size);

    /**
     * Free all memory allocated by the font.
     */
    ~TextureFont();

    /** Generated glyphs from this font. */
    vector_t *_glyphs;

    /** Generated atlas from this font */
    texture_atlas_t *_atlas;

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
 * This function creates a new texture font from given filename and size.  The
 * texture atlas is used to store glyph on demand. Note the depth of the atlas
 * will determine if the font is rendered as alpha channel only (depth = 1) or
 * RGB (depth = 3) that correspond to subpixel rendering (if available on your
 * freetype implementation).
 *
 * @param atlas     A texture atlas
 * @param filename  A font filename
 * @param size      Size of font to be created (in points)
 *
 * @return A new empty font (no glyph inside yet)
 *
 */
  TextureFont *
  texture_font_new( texture_atlas_t * atlas,
                    const char * filename,
                    const float size );


/**
 * Delete a texture font. Note that this does not delete the glyph from the
 * texture atlas.
 *
 * @param self a valid texture font
 */
  void
  texture_font_delete( TextureFont * self );


/**
 * Request a new glyph from the font. If it has not been created yet, it will
 * be. 
 *
 * @param self     A valid texture font
 * @param charcode Character codepoint to be loaded.
 *
 * @return A pointer on the new glyph or 0 if the texture atlas is not big
 *         enough
 *
 */
  ft::Glyph *
  texture_font_get_glyph( TextureFont * self,
                          wchar_t charcode );


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


#endif /* __TEXTURE_FONT_H__ */

