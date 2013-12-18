
#pragma once

/**
 * OpenGL includes according system.
 */
#if defined(__APPLE__)
#  ifdef GL_ES_VERSION_2_0
#    include <OpenGLES/ES2/gl.h>
#  else
#    include <OpenGL/gl.h>
#  endif
#else
#  include <GL/glew.h>
#  include <GL/gl.h>
#endif