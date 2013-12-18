
#include "core/ft.hpp"
#include "core/Error.hpp"
#include "core/Library.hpp"

#include <iostream>
#include <stdexcept>
#include <assert.h>
#include <math.h>
#include <wchar.h>
#include "texture-font.h"

static size_t
_get_next_power_of_two(size_t v)
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
_compute_atlas( TextureFont * self,
                           FT_Face face )
{
    size_t size = 0;
    size_t i;

    for( i=0; i<wcslen(self->_cache); ++i )
    {
        if (self->_cache[i] == '\n')
            continue;
        if(FT_Get_Char_Index( face, self->_cache[i] ))
            size++;
    }
    size = _get_next_power_of_two((size * self->_size) / 20);
    self->_atlas = texture_atlas_new( size, size, 1 );
    return 0;
}

int
texture_font_load_face( const char * filename,
                        const float size,
                        FT_Face * face );

TextureFont::TextureFont(texture_atlas_t *atlas,
                         std::string const &path,
                         float size) :
  _path(path),
  _size(size) {
  FT_Face face;
  FT_Size_Metrics metrics;

  if (size <= 0)
    throw std::runtime_error("Font size should be positive.");
  _cache = _get_all_charcodes();
  _glyphs = vector_new( sizeof(Glyph *) );
  _atlas = atlas;
  _height = 0;
  _ascender = 0;
  _descender = 0;
  _size = size;
  _kerning = 1;

  /* Get font metrics at high resolution */

  if(!texture_font_load_face(_path.c_str(), _size * 100, &face))
    throw std::runtime_error("Cannot load font face.");

  if (_atlas == NULL)
      _compute_atlas( this, face );

  metrics = face->size->metrics; 
  _ascender = (metrics.ascender >> 6) / 100.0;
  _descender = (metrics.descender >> 6) / 100.0;
  _height = (metrics.height >> 6) / 100.0;
  _linegap = _height - _ascender + _descender;
  FT_Done_Face( face );
}

TextureFont::~TextureFont() {}

// ------------------------------------------------------- texture_font_new ---
TextureFont *
texture_font_new( texture_atlas_t * atlas,
                  const char * filename,
                  const float size)
{
    TextureFont *self = (TextureFont *) malloc( sizeof(TextureFont) );
    FT_Face face;
    FT_Size_Metrics metrics;
    
    assert( filename );
    assert( size );

    self->_cache = _get_all_charcodes();

    if( self == NULL)
    {
        fprintf( stderr,
                 "line %d: No more memory for allocating data\n", __LINE__ );
        exit( EXIT_FAILURE );
    }
    self->_glyphs = vector_new( sizeof(Glyph *) );
    self->_atlas = atlas;
    self->_height = 0;
    self->_ascender = 0;
    self->_descender = 0;
    self->filename = strdup( filename );
    self->_size = size;
    self->_kerning = 1;

    /* Get font metrics at high resolution */

    if( !texture_font_load_face(self->filename, self->_size*100, &face ) )
    {
        return self;
    }

    if (self->_atlas == NULL)
        _compute_atlas( self, face );

    metrics = face->size->metrics; 
    self->_ascender = (metrics.ascender >> 6) / 100.0;
    self->_descender = (metrics.descender >> 6) / 100.0;
    self->_height = (metrics.height >> 6) / 100.0;
    self->_linegap = self->_height - self->_ascender + self->_descender;
    FT_Done_Face( face );

    return self;
}

// ------------------------------------------------- texture_font_load_face ---
int
texture_font_load_face( const char * filename,
                        const float size,
                        FT_Face * face )
{
    size_t hres = 64;
    FT_Error error;
    FT_Matrix matrix = { (int)((1.0/hres) * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((0.0)      * 0x10000L),
                         (int)((1.0)      * 0x10000L) };

    assert( filename );
    assert( size );

    /* Load face */
    error = FT_New_Face(ft::Library::getHandle(), filename, 0, face );
    if( error )
      throw ft::Error(error);

    /* Select charmap */
    error = FT_Select_Charmap( *face, FT_ENCODING_UNICODE );
    if( error )
      throw ft::Error(error);

    /* Set char size */
    error = FT_Set_Char_Size( *face, (int)(size*64), 0, 72*hres, 72 );
    if( error )
      throw ft::Error(error);

    /* Set transform matrix */
    FT_Set_Transform( *face, &matrix, NULL );

    return 1;
}


// ------------------------------------------------------ texture_glyph_new ---
Glyph *
texture_glyph_new( void )
{
    Glyph *self = (Glyph *) malloc( sizeof(Glyph) );
    if( self == NULL)
    {
        fprintf( stderr,
                 "line %d: No more memory for allocating data\n", __LINE__ );
        exit( EXIT_FAILURE );
    }
    self->id        = 0;
    self->width     = 0;
    self->height    = 0;
    self->offset_x  = 0;
    self->offset_y  = 0;
    self->advance_x = 0.0;
    self->advance_y = 0.0;
    self->s0        = 0.0;
    self->t0        = 0.0;
    self->s1        = 0.0;
    self->t1        = 0.0;
    self->kerning   = vector_new( sizeof(Kerning) );
    return self;
}


// --------------------------------------------------- texture_glyph_delete ---
void
texture_glyph_delete( Glyph *self )
{
    assert( self );
    vector_delete( self->kerning );
    free( self );
}

// ---------------------------------------------- texture_glyph_get_kerning ---
float 
texture_glyph_get_kerning( const Glyph * self,
                           const wchar_t charcode )
{
    size_t i;

    assert( self );
    for( i=0; i<vector_size(self->kerning); ++i )
    {
        Kerning * kerning = (Kerning *) vector_get( self->kerning, i );
        if( kerning->charcode == charcode )
        {
            return kerning->kerning;
        }
    }
    return 0;
}


// ------------------------------------------ texture_font_generate_kerning ---
void
texture_font_generate_kerning( TextureFont *self )
{
    size_t i, j;
    FT_Face face;
    FT_UInt glyph_index, prev_index;
    Glyph *glyph, *prev_glyph;
    FT_Vector kerning;
    
    assert( self );

    /* Load font */
    if( !texture_font_load_face(self->filename, self->_size, &face ) )
    {
        return;
    }

    /* For each glyph couple combination, check if kerning is necessary */
    /* Starts at index 1 since 0 is for the special backgroudn glyph */
    for( i=1; i<self->_glyphs->size; ++i )
    {
        glyph = *(Glyph **) vector_get( self->_glyphs, i );
        glyph_index = FT_Get_Char_Index( face, glyph->charcode );
        vector_clear( glyph->kerning );

        for( j=1; j<self->_glyphs->size; ++j )
        {
            prev_glyph = *(Glyph **) vector_get( self->_glyphs, j );
            prev_index = FT_Get_Char_Index( face, prev_glyph->charcode );
            FT_Get_Kerning( face, prev_index, glyph_index, FT_KERNING_UNFITTED, &kerning );
            // printf("%c(%d)-%c(%d): %ld\n",
            //       prev_glyph->charcode, prev_glyph->charcode,
            //       glyph_index, glyph_index, kerning.x);
            if( kerning.x )
            {
                // 64 * 64 because of 26.6 encoding AND the transform matrix used
                // in texture_font_load_face (hres = 64)
                Kerning k = {prev_glyph->charcode, kerning.x / (float)(64.0f*64.0f)};
                vector_push_back( glyph->kerning, &k );
            }
        }
    }
    FT_Done_Face( face );
}

// ---------------------------------------------------- texture_font_delete ---
void
texture_font_delete( TextureFont *self )
{
    size_t i;
    Glyph *glyph;

    assert( self );

    for( i=0; i<vector_size( self->_glyphs ); ++i)
    {
        glyph = *(Glyph **) vector_get( self->_glyphs, i );
        texture_glyph_delete( glyph);
    }

    vector_delete( self->_glyphs );
    free( self );
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
    size_t i, x, y, width, height, depth, w, h;
    FT_Error error;
    FT_Face face;
    FT_Glyph ft_glyph;
    FT_GlyphSlot slot;
    FT_Bitmap ft_bitmap;

    FT_UInt glyph_index;
    Glyph *glyph;
    ivec4 region;
    size_t missed = 0;

    assert( self );
    assert( charcodes );

    if( !texture_font_load_face(self->filename, self->_size, &face ) )
    {
        return wcslen(charcodes);
    }

    width  = self->_atlas->width;
    height = self->_atlas->height;
    depth  = self->_atlas->depth;

    /* Load each glyph */
    for( i=0; i<wcslen(charcodes); ++i )
    {
        FT_Int32 flags = 0;
        int ft_bitmap_width = 0;
        int ft_bitmap_rows = 0;
        int ft_bitmap_pitch = 0;
        int ft_glyph_top = 0;
        int ft_glyph_left = 0;
        glyph_index = FT_Get_Char_Index( face, charcodes[i] );
        // WARNING: We use texture-atlas depth to guess if user wants
        //          LCD subpixel rendering
        if (glyph_index == 0) // skip
            continue;
        flags |= FT_LOAD_RENDER;
        flags |= FT_LOAD_NO_HINTING | FT_LOAD_NO_AUTOHINT;
        error = FT_Load_Glyph( face, glyph_index, flags );
        if( error )
          throw ft::Error(error);


        slot            = face->glyph;
        ft_bitmap       = slot->bitmap;
        ft_bitmap_width = slot->bitmap.width;
        ft_bitmap_rows  = slot->bitmap.rows;
        ft_bitmap_pitch = slot->bitmap.pitch;
        ft_glyph_top    = slot->bitmap_top;
        ft_glyph_left   = slot->bitmap_left;

        // We want each glyph to be separated by at least one black pixel
        // (for example for shader used in demo-subpixel.c)
        w = ft_bitmap_width/depth + padding;
        h = ft_bitmap_rows + padding;
        region = texture_atlas_get_region( self->_atlas, w, h );
        if ( region.x < 0 )
        {
            missed++;
            fprintf( stderr, "Texture atlas is full (line %d) (size %lu)\n",  __LINE__, i );
            break;
        }
        w = w - padding;
        h = h - padding;
        x = region.x + (padding >> 1);
        y = region.y + (padding >> 1);
        texture_atlas_set_region( self->_atlas, x, y, w, h,
                                  ft_bitmap.buffer, ft_bitmap.pitch );

        glyph = texture_glyph_new( );
        glyph->charcode = charcodes[i];
        glyph->width    = w;
        glyph->height   = h;
        glyph->offset_x = ft_glyph_left;
        glyph->offset_y = ft_glyph_top;
        glyph->s0       = x/(float)width;
        glyph->t0       = y/(float)height;
        glyph->s1       = (x + glyph->width)/(float)width;
        glyph->t1       = (y + glyph->height)/(float)height;

        // Discard hinting to get advance
        FT_Load_Glyph( face, glyph_index, FT_LOAD_RENDER | FT_LOAD_NO_HINTING);
        slot = face->glyph;
        glyph->advance_x = slot->advance.x/64.0;
        glyph->advance_y = slot->advance.y/64.0;

        vector_push_back( self->_glyphs, &glyph );

    }
    FT_Done_Face( face );
    texture_font_generate_kerning( self );
    return missed;
}


// ------------------------------------------------- texture_font_get_glyph ---
Glyph *
texture_font_get_glyph( TextureFont * self,
                        wchar_t charcode )
{
    size_t i;
    wchar_t buffer[2] = {0,0};
    Glyph *glyph;

    assert( self );

    assert( self );
    assert( self->filename );
    assert( self->_atlas );

    /* Check if charcode has been already loaded */
    for( i=0; i<self->_glyphs->size; ++i )
    {
        glyph = *(Glyph **) vector_get( self->_glyphs, i );
        // If charcode is -1, we don't care about outline type or thickness
        if( (glyph->charcode == charcode) &&
            ((charcode == (wchar_t)(-1) )))
        {
            return glyph;
        }
    }

    /* Glyph has not been already loaded */
    buffer[0] = charcode;
    if( texture_font_load_glyphs( self, buffer ) == 0 )
    {
        return *(Glyph **) vector_back( self->_glyphs );
    }
    return NULL;
}


