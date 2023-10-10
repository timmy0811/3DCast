#include "glpch.h"
#include "Debug.h"

#include <GLEW/glew.h>

void GLCLearError()
{
}

bool GLLogCall(const char* function, const char* file, int line)
{
	while (GLenum error = glGetError()) {
		std::stringstream ss;
		ss << "[OpenGL Error] (" << error << "): " << function << " " << file << ": line " << line << std::endl;
		LOG_GL_ERROR("{}", ss.str());
		return false;
	}
	return true;
}