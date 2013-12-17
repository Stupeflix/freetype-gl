CMAKE_MINIMUM_REQUIRED( VERSION 2.8.5 )

project(distmapgen)

set(CMAKE_CXX_FLAGS "-ggdb3")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake_modules"
                      "${CMAKE_MODULE_PATH}" )


# Find required libraries.
find_package(OpenGL REQUIRED)
find_package(Math REQUIRED)
find_package(FreeType REQUIRED)

file(GLOB HEADERS "*.h")

set( SOURCES
  texture-atlas.c
  texture-font.c
  font-manager.c
  vector.c       
  platform.c     
  edtaa3func.c
  main.cpp)

include_directories(${OPENGL_INCLUDE_DIRS}
                    ${FREETYPE_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME} ${FREETYPE_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${MATH_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${OPENGL_LIBRARIES})

# Copy font and shaders into build directory for in-place testing
file(GLOB FONTS "fonts/*.ttf")
file(COPY ${FONTS}   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts)
file(COPY "make_png.py" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
