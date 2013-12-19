 
// #include <stdexcept>
// #include <stdlib.h>
// #include <stdio.h>
// #include <png.h>
 
// int width, height;
 
// unsigned char *read_png_file(std::string const &path) {
//   FILE *fp = fopen(path.c_str(), "rb");
//   if (!fp)
//     throw std::runtime_error("Cannot open png \"" + path + "\".");
//   png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//   if (!png)
//     throw std::runtime_error("Cannot create png.");
//   png_infop info = png_create_info_struct(png);
//   if (!info)
//     throw std::runtime_error("Cannot load png info.");
//   if (setjmp(png_jmpbuf(png)))
//     throw std::runtime_error("Cannot init png io.");
//   png_init_io(png, fp);
//   png_read_info(png, info);
//   width = png_get_image_width(png, info);
//   height = png_get_image_height(png, info);
//   png_byte color_type = png_get_color_type(png, info);
//   png_byte bit_depth = png_get_bit_depth(png, info);
//   if (bit_depth == 16)
//     png_set_strip_16(png);
//   if (color_type == PNG_COLOR_TYPE_PALETTE)
//     png_set_palette_to_rgb(png);
//   if (color_type == PNG_COLOR_TYPE_GRAY && bit_depth < 8)
//     png_set_expand_gray_1_2_4_to_8(png);
//   if (png_get_valid(png, info, PNG_INFO_tRNS))
//     png_set_tRNS_to_alpha(png);
//   if (color_type == PNG_COLOR_TYPE_RGB ||
//      color_type == PNG_COLOR_TYPE_GRAY ||
//      color_type == PNG_COLOR_TYPE_PALETTE)
//     png_set_filler(png, 0xFF, PNG_FILLER_AFTER);
//   if (color_type == PNG_COLOR_TYPE_GRAY ||
//      color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
//     png_set_gray_to_rgb(png);
//   png_read_update_info(png, info);
//   png_bytep *row_pointers;

//   /* Read data from file */
//   row_pointers = new png_bytep[height];
//   for (int y = 0; y < height; y++)
//     row_pointers[y] = new png_byte[png_get_rowbytes(png,info)];
//   png_read_image(png, row_pointers);
//   fclose(fp);

//   /* Copy data. */
//   unsigned char *data = new unsigned char[width * height * 4];
//   for (int y = 0; y < height; y++) {
//     png_bytep row = row_pointers[y];
//     for (int x = 0; x < width; x++) {
//       png_bytep px = &(row[x * 4]);
//       int i = (y * width + x) * 4;
//       data[i] = px[0];
//       data[i + 1] = px[1];
//       data[i + 2] = px[2];
//       data[i + 3] = px[3];
//     }
//   }

//   /* Free */
//   png_destroy_read_struct(&png, &info, NULL);
//   for (int y = 0; y < height; y++)
//     delete[] row_pointers[y];
//   delete[] row_pointers;
//   return data;
// }
 
// void write_png_file(std::string const &path, unsigned char *data) {
//   int y;
//   FILE *fp = fopen(path.c_str(), "wb");
//   if (!fp)
//     throw std::runtime_error("Cannot open png \"" + path + "\".");
//   png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
//   if (!png) abort();
//   png_infop info = png_create_info_struct(png);
//   if (!info) abort();
//   if (setjmp(png_jmpbuf(png))) abort();
//   png_init_io(png, fp);
//   png_set_IHDR(
//     png,
//     info,
//     width, height,
//     8,
//     PNG_COLOR_TYPE_RGBA,
//     PNG_INTERLACE_NONE,
//     PNG_COMPRESSION_TYPE_DEFAULT,
//     PNG_FILTER_TYPE_DEFAULT
//   );
//   png_write_info(png, info);

//   /* Copy data */
 
//   png_bytep *row_pointers;
//   row_pointers = new png_bytep[height];
//   for (int y = 0; y < height; y++) {
//     row_pointers[y] = new png_byte[png_get_rowbytes(png,info)];
//     for (int x = 0; x < width; ++x) {
//       png_bytep px = &(row_pointers[y][x * 4]);
//       int i = (y * width + x) * 4;
//       px[0] = data[i];
//       px[1] = data[i + 1];
//       px[2] = data[i + 2];
//       px[3] = data[i + 3];
//     }
//   }

//   /* Write to file. */

//   png_write_image(png, row_pointers);
//   png_write_end(png, NULL);

//   /* Free allocated memory. */
//   png_destroy_write_struct(&png, &info);
//   for (int y = 0; y < height; y++)
//     delete[] row_pointers[y];
//   delete[] row_pointers;
//   fclose(fp);
// }
 
// int main(int argc, char *argv[]) {
//   if(argc != 3) abort();
 
//   // process_png_file();
//   unsigned char *data = read_png_file(argv[1]);
//   write_png_file(argv[2], data);
//   delete[] data; 
//   return 0;
// }

#include <string>
#include <cstdio>
#include <cstring>
#include <iostream>
#include "core/FontTexture.hpp"
#include "core/Distmap.hpp"
#include "vendors/edtaa3func.hpp"

// ------------------------------------------------------- global variables ---
core::Atlas atlas(1024, 1024);
core::FontTexture  * font  = 0;

// ------------------------------------------------------ create_atlas_file ---
int
create_atlas_file(char const *font_file, core::Texture const &atlas)
{
    char output_file[strlen(font_file) + sizeof(".atlas") + 1];
    strcpy(output_file, font_file);
    strcat(output_file, ".atlas");

    fprintf(stdout, "    Create \"%s\"...", output_file);
    fflush( stdout );

    FILE *file_stream = fopen(output_file, "w");
    if( file_stream == NULL )
    {
        fprintf( stderr, "Error: Cannot open file \"%s\".\n", output_file );
        return 1;
    }
    fwrite( atlas.getData(), atlas.getWidth() * atlas.getHeight(), sizeof(unsigned char), file_stream );
    if( file_stream != NULL )
        fclose(file_stream);
    fprintf(stdout, "OK\n");
    return 0;
}

void
write_glyph( FILE * file_stream,
             ft::Glyph const * glyph,
             char is_last )
{
    fprintf(file_stream,
        "    \"%d\":{\"offset_x\":%d,\"offset_y\":%d,\"advance_x\":%f,\"advance_y\":%f,\"width\":%lu,\"height\":%lu,\"s0\":%f,\"t0\":%f,\"s1\":%f,\"t1\":%f}%s\n",
        glyph->charcode,
        glyph->offset_x, glyph->offset_y,
        glyph->advance_x, glyph->advance_y,
        glyph->width, glyph->height,
        glyph->s0, glyph->t0, glyph->s1, glyph->t1,
        is_last ? "" : ","
    );
}

// ----------------------------------------------------- create_python_file ---
int
create_json_file( const char *font_file )
{
    char output_file[strlen(font_file) + sizeof(".json") + 1];
    strcpy(output_file, font_file);
    strcat(output_file, ".json");

    fprintf( stdout, "    Create \"%s\"...", output_file);
    fflush( stdout );

    FILE *file_stream = fopen(output_file, "w");
    if( file_stream == NULL )
    {
        fprintf( stderr, "Error: Cannot open file \"%s\".\n", output_file );
        return 1;
    }
    fprintf(file_stream, "{\n");
    fprintf(file_stream, "  \"atlas_width\": %lu,\n", atlas.getWidth());
    fprintf(file_stream, "  \"atlas_height\": %lu,\n", atlas.getHeight());
    fprintf(file_stream, "  \"glyphs_number\": %lu,\n", font->_glyphs.size());
    fprintf(file_stream, "  \"glyphs\": {\n");
    for (size_t i = 1; i < font->_glyphs.size(); ++i) {
        write_glyph(
            file_stream,
            font->_glyphs[i],
            i + 1 == font->_glyphs.size()
        );
    }
    fprintf(file_stream, "  }\n");
    fprintf(file_stream, "}\n");
    if (file_stream != NULL)
        fclose(file_stream);
    fprintf(stdout, "OK\n");
    return 0;
}

int main( int argc, char **argv ) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0]
              << " FONT_FILE [RESOLUTION=40]" << std::endl;
    return 1;
  }

  wchar_t * font_cache = 
      L" !\"#$%&'()*+,-./0123456789:;<=>?"
      L"@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_"
      L"`abcdefghijklmnopqrstuvwxyz{|}~";

  // unsigned char *map;
  const char *font_file = argv[1];
  const size_t resolution = argc > 2 ? atoi(argv[2]) : 50;

  std::cout << std::endl << "  Generate \""
            << font_file << "\" distmap with resolution of "
            << resolution << "." << std::endl << std::endl;

  std::cout << std::endl << "    Generate font texture and atlas..."
            << std::flush;

  font = new core::FontTexture(atlas, font_file, resolution);
  font->setPadding(25);
  font->generate();

  std::cout << "OK" << std::endl;

  std::cout << "    Generate distance map..." << std::flush;

  core::Distmap distmap(1024, 1024);
  distmap.generate(atlas);
  std::cout << "OK" << std::endl;
  atlas.saveToPng("../atlas.png");
  distmap.saveToPng("../distmap.png");

  if (create_atlas_file(font_file, distmap) != 0)
      return 1;
  if (create_json_file(font_file) != 0)
      return 1;

  std::cout << std::endl;

  delete font;

  return 0;
}
