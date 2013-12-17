CMAKE_MINIMUM_REQUIRED( VERSION 2.8.5 )

set(CMAKE_CXX_FLAGS "-ggdb3")
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMakeModules"
                      "${CMAKE_MODULE_PATH}" )


# Get required and optional library
find_package( OpenGL REQUIRED )

INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_DIRS}
                    ${FREETYPE_INCLUDE_DIRS}
                    ${CMAKE_CURRENT_SOURCE_DIR})

SET( FREETYPE_GL_SRC freetype-gl.h
                     vec234.h
                     markup.h
                     mat4.c             mat4.h
                     texture-atlas.c    texture-atlas.h
                     texture-font.c     texture-font.h
                     font-manager.c     font-manager.h
                     vector.c           vector.h
                     platform.c         platform.h)

ADD_LIBRARY( freetype-gl STATIC ${FREETYPE_GL_SRC} )

link_directories(${PROJECT_SOURCE_DIR})

MACRO( DEMO _target _sources)
  add_executable(${_target} ${_sources})
  include_directories(${WINDOWS_DIR} ${GLUT_INCLUDE_DIR} ${GLEW_INCLUDE_DIR} ${FREETYPE_INCLUDE_DIR_ft2build} ${FREETYPE_INCLUDE_DIR_freetype2} ${ANT_TWEAK_BAR_INCLUDE_PATH})
  target_link_libraries(${_target} freetype-gl)
  target_link_libraries(${_target} ${FREETYPE_LIBRARY})
  target_link_libraries(${_target} ${MATH_LIBRARY})
  target_link_libraries(${_target} ${OPENGL_LIBRARIES})
ENDMACRO( DEMO )

DEMO(compute-distmap "compute-distmap.c;edtaa3func.c" )

# Copy font and shaders into build directory for in-place testing
file(GLOB Fonts   "fonts/*.ttf")

file(COPY ${Fonts}   DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/fonts)
file(COPY "make_png.py" DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
