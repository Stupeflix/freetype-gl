
#include <stdio.h>
#include <string.h>
#include "freetype-gl.h"

// ------------------------------------------------------- global variables ---
texture_font_t  * font  = 0;

// ------------------------------------------------------ make_distance_map ---
unsigned char *
make_distance_map( unsigned char *img,
                   unsigned int width, unsigned int height )
{
    short * xdist = (short *)  malloc( width * height * sizeof(short) );
    short * ydist = (short *)  malloc( width * height * sizeof(short) );
    double * gx   = (double *) calloc( width * height, sizeof(double) );
    double * gy      = (double *) calloc( width * height, sizeof(double) );
    double * data    = (double *) calloc( width * height, sizeof(double) );
    double * outside = (double *) calloc( width * height, sizeof(double) );
    double * inside  = (double *) calloc( width * height, sizeof(double) );
    int i;

    // Convert img into double (data)
    double img_min = 255, img_max = -255;
    for( i=0; i<width*height; ++i)
    {
        double v = img[i];
        data[i] = v;
        if (v > img_max) img_max = v;
        if (v < img_min) img_min = v;
    }
    // Rescale image levels between 0 and 1
    for( i=0; i<width*height; ++i)
    {
        data[i] = (img[i]-img_min)/img_max;
    }

    // Compute outside = edtaa3(bitmap); % Transform background (0's)
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, height, width, xdist, ydist, outside);
    for( i=0; i<width*height; ++i)
        if( outside[i] < 0 )
            outside[i] = 0.0;

    // Compute inside = edtaa3(1-bitmap); % Transform foreground (1's)
    memset(gx, 0, sizeof(double)*width*height );
    memset(gy, 0, sizeof(double)*width*height );
    for( i=0; i<width*height; ++i)
        data[i] = 1 - data[i];
    computegradient( data, width, height, gx, gy);
    edtaa3(data, gx, gy, height, width, xdist, ydist, inside);
    for( i=0; i<width*height; ++i)
        if( inside[i] < 0 )
            inside[i] = 0.0;

    // distmap = outside - inside; % Bipolar distance field
    unsigned char *out = (unsigned char *) malloc( width * height * sizeof(unsigned char) );
    for( i=0; i<width*height; ++i)
    {
        outside[i] -= inside[i];
        outside[i] = 128+outside[i]*16;
        if( outside[i] < 0 ) outside[i] = 0;
        if( outside[i] > 255 ) outside[i] = 255;
        out[i] = 255 - (unsigned char) outside[i];
        //out[i] = (unsigned char) outside[i];
    }

    free( xdist );
    free( ydist );
    free( gx );
    free( gy );
    free( data );
    free( outside );
    free( inside );
    return out;
}

// ------------------------------------------------------ create_atlas_file ---
int
create_atlas_file(char const *output_name)
{
    char output_file[512];
    strcpy(output_file, output_name);
    strcat(output_file, ".atlas");

    fprintf(stdout, "    Create \"%s\"...", output_file);
    fflush( stdout );

    FILE *file_stream = fopen(output_file, "w");
    if( file_stream == NULL )
    {
        fprintf( stderr, "Error: Cannot open file \"%s\".\n", output_file );
        return 1;
    }
    fwrite( font->atlas->data, font->atlas->width * font->atlas->height, sizeof(unsigned char), file_stream );
    if( file_stream != NULL )
        fclose(file_stream);
    fprintf(stdout, "OK\n");
    return 0;
}

// ----------------------------------------------------- create_python_file ---
int
create_python_file(const char *output_name)
{
    char output_file[512];
    strcpy(output_file, output_name);
    strcat(output_file, ".py");

    fprintf( stdout, "    Create \"%s\"...", output_file);
    fflush( stdout );

    FILE *file_stream = fopen(output_file, "w");
    if( file_stream == NULL )
    {
        fprintf( stderr, "Error: Cannot open file \"%s\".\n", output_file );
        return 1;
    }
    fprintf(file_stream, "#!/usr/bin/env python\n", output_name);
    fprintf(file_stream, "# -*- coding: utf-8 -*-\n");
    fprintf(file_stream, "# Contains all %s's meta data to use distmap atlas from python.\n\n", output_name);
    fprintf(file_stream, "data = {\n");
    fprintf(file_stream, "  'atlas_width': %lu,\n", font->atlas->width);
    fprintf(file_stream, "  'atlas_height': %lu,\n", font->atlas->height);
    fprintf(file_stream, "  'glyphs_number': %lu,\n", font->glyphs->size);
    fprintf(file_stream, "  'glyphs': {\n");
    size_t i;
    for( i = 1; i < font->glyphs->size; ++i )
    {
        texture_glyph_t const * glyph =
            *(texture_glyph_t **)
            vector_get(font->glyphs, i);
        if (glyph->charcode == '\'')
        {
            fprintf(file_stream,
                "    '\\'':{'s0':%f,'t0':%f,'s1':%f,'t1':%f},\n",
                glyph->s0, glyph->t0, glyph->s1, glyph->t1
            );
        }
        else if (glyph->charcode == '\\')
        {
            fprintf(file_stream,
                "    '\\\\':{'s0':%f,'t0':%f,'s1':%f,'t1':%f},\n",
                glyph->s0, glyph->t0, glyph->s1, glyph->t1
            );
        }
        else if (i != 194)
        {
            fprintf(file_stream,
                "    '%c':{'s0':%f,'t0':%f,'s1':%f,'t1':%f},\n",
                glyph->charcode,
                glyph->s0, glyph->t0, glyph->s1, glyph->t1
            );
        }
    }
    fprintf(file_stream, "  }\n");
    fprintf(file_stream, "}\n");
    if (file_stream != NULL)
        fclose(file_stream);
    fprintf(stdout, "OK\n");
    return 0;
}

// ------------------------------------------------------------------- main ---
int
main( int argc, char **argv )
{

    if (argc < 3)
    {
        printf("Usage: %s INPUT_FONT OUTPUT_NAME [RESOLUTION=40]\n", argv[0]);
        return 1;
    }

    unsigned char *map;
    const char *input_file = argv[1];
    const char *output_name = argv[2];
    const size_t RESOLUTION = argc > 3 ? atoi(argv[3]) : 40;
    const wchar_t *cache = L"!\"#$%&'()*+,-./0123456789:;<=>?"
                           L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
                           L"`abcdefghijklmnopqrstuvwxyz{|}~";

    printf("\n  Generate \"%s\" from \"%s\" (resolution %lu)...\n\n", output_name, input_file, RESOLUTION);

    fprintf( stdout, "    Generate font texture and atlas..." );
    fflush( stdout );
    font = texture_font_new( NULL, input_file, RESOLUTION );
    // texture_font_load_glyphs_with_padding( font, cache, 25 );
    texture_font_load_with_padding( font, 25 );
    fprintf( stdout, "OK\n");

    fprintf( stdout, "    Generate distance map..." );
    fflush( stdout );
    map = make_distance_map( font->atlas->data, font->atlas->width, font->atlas->height );
    memcpy( font->atlas->data, map, font->atlas->width * font->atlas->height * sizeof(unsigned char) );
    free( map );
    fprintf( stdout, "OK\n");

    if ( create_atlas_file(output_name) != 0 )
        return 1;
    if ( create_python_file(output_name) != 0)
        return 1;

    printf("\n");

    texture_atlas_delete( font->atlas );
    texture_font_delete( font );

    return 0;
}
