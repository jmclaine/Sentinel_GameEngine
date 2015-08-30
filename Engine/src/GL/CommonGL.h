#pragma once

#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#pragma comment (lib, "opengl32.lib")
#pragma comment (lib, "glu32.lib")
#pragma comment (lib, "glew32.lib")

#include <fstream>
#include <vector>

#include "Util.h"
#include "Types.h"

#ifdef _DEBUG
inline void HANDLE_GL_ERRORS()
{
	GLenum err = glGetError();

	if( err != GL_NO_ERROR )
		REPORT_ERROR( gluErrorUnicodeStringEXT(err), "OpenGL Error" );
}
#else
#define HANDLE_GL_ERRORS();
#endif
