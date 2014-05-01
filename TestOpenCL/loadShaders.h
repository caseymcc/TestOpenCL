#ifndef _loadShaders_h_
#define _loadShaders_h_

#include <GL/glew.h>
#include <string>

GLuint loadShaderFiles(const std::string &vertexFilePath, const std::string &fragmentFilePath);
GLuint loadShaders(const std::string &vertexShader, const std::string &fragShader);

#endif //_loadShaders_h_