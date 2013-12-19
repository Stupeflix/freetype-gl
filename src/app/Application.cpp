
#include <iostream>
#include <fstream>
#include "core/FontTexture.hpp"
#include "core/Distmap.hpp"
#include "app/Application.hpp"

Application::Application() {}

Application::~Application() {}

void Application::start(Command const &cmd) {

  std::size_t size = cmd.getOption<std::size_t>("size");
  std::string path = cmd.getArg<std::string>(0);

  /* Generate texture font */
  core::Atlas atlas(size, size);
  core::FontTexture font(atlas, path,
      cmd.getOption<std::size_t>("resolution"));
  font.setPadding(cmd.getOption<std::size_t>("padding"));
  font.generate();

  /* Generate distmap and save it to a file */
  core::Distmap distmap(size, size);
  distmap.generate(atlas);
  std::string prefix = cmd.getOption<std::string>("output_dir") + "/";
  distmap.saveToPng(prefix + path + ".png");

  /* Save meta data */
  std::ofstream jsonFile(prefix + path + ".json");
  jsonFile << font.toJson();
  jsonFile.close();

}