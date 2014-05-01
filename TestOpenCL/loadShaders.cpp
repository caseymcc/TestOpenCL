#include "loadShaders.h"

#include <boost/format.hpp>

#include <string>
#include <vector>


GLuint loadShaderFiles(const std::string &vertexFilePath, const std::string &fragmentFilePath)
{
	FILE *vertextFile=fopen(vertexFilePath.c_str(), "r");

	if(vertextFile == NULL)
		return 0;

	std::string vertexShader;
	char buffer[1024];
	int read;
	
	while((read=fread(buffer, sizeof(char), 1023, vertextFile)) != EOF)
		vertexShader.append(buffer);

	fclose(vertextFile);

	FILE *fragFile=fopen(fragmentFilePath.c_str(), "r");
	std::string fragShader;

	if(fragFile == NULL)
		return 0;

	while((read=fread(buffer, sizeof(char), 1023, fragFile)) != EOF)
		fragShader.append(buffer);

	fclose(fragFile);

	return loadShaders(vertexShader, fragShader);
}

GLuint loadShaders(const std::string &vertexShader, const std::string &fragShader)
{
	GLint result = GL_FALSE;
	int logLength;

	//create vertex and frag shaders
	GLuint vertexID=glCreateShader(GL_VERTEX_SHADER);
	GLuint fragID=glCreateShader(GL_FRAGMENT_SHADER);

	//compile vertex
	char const *vertex=vertexShader.c_str();

	glShaderSource(vertexID, 1, &vertex , NULL);
	glCompileShader(vertexID);

	//check log
	glGetShaderiv(vertexID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(vertexID, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		std::vector<char> errorMessage(logLength+1);

		glGetShaderInfoLog(vertexID, logLength, NULL, &errorMessage[0]);
//		OutputDebugStringA((boost::format("%s\n")%errorMessage[0]).str().c_str());
		assert(false);
	}

	//compile frag
	char const *frag=fragShader.c_str();

	glShaderSource(fragID, 1, &frag , NULL);
	glCompileShader(fragID);

	//check log
	glGetShaderiv(fragID, GL_COMPILE_STATUS, &result);
	glGetShaderiv(fragID, GL_INFO_LOG_LENGTH, &logLength);
	if(logLength > 0)
	{
		std::vector<char> errorMessage(logLength+1);

		glGetShaderInfoLog(vertexID, logLength, NULL, &errorMessage[0]);
//		OutputDebugStringA((boost::format("%s\n")%errorMessage[0]).str().c_str());
		assert(false);
	}

	//link
	GLuint programID=glCreateProgram();

	glAttachShader(programID, vertexID);
	glAttachShader(programID, fragID);
	glLinkProgram(programID);

	//check log
	glGetProgramiv(programID, GL_LINK_STATUS, &result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &logLength);
	
	if(logLength > 0)
	{
		std::vector<char> errorMessage(logLength+1);

		glGetShaderInfoLog(vertexID, logLength, NULL, &errorMessage[0]);
//		OutputDebugStringA((boost::format("%s\n")%errorMessage[0]).str().c_str());
		assert(false);
	}

	glDeleteShader(vertexID);
	glDeleteShader(fragID);

	return programID;
}

