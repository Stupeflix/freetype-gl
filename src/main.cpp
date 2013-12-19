
#include <iostream>
#include <fstream>
#include "core/FontTexture.hpp"
#include "core/Distmap.hpp"

int main( int argc, char **argv ) {
  if (argc < 2) {
    std::cout << "Usage: " << argv[0]
              << " FONT_FILE [RESOLUTION=40]" << std::endl;
    return 1;
  }

  const char *font_file = argv[1];
  const size_t resolution = argc > 2 ? atoi(argv[2]) : 50;

  /* Generate texture font */
  core::Atlas atlas(1024, 1024);
  core::FontTexture font(atlas, font_file, resolution);
  font.setPadding(25);
  font.generate();

  /* Generate distmap and save it to a file */
  core::Distmap distmap(1024, 1024);
  distmap.generate(atlas);
  distmap.saveToPng("../distmap.png");

  /* Save meta data */
  std::ofstream jsonFile("../meta.json");
  jsonFile << font.toJson();
  jsonFile.close();

  return 0;
}
