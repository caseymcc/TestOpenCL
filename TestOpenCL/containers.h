#ifndef _containers_h_
#define _containers_h_

#include <GL/glew.h>
#include "CL/cl.hpp"

class ImageHolder
{
public:
	ImageHolder():m_image(nullptr), m_width(0), m_height(0){};
	ImageHolder(cl::Context openCLContext, cl_mem_flags flags, int width, int height);
	~ImageHolder();

	bool create(cl::Context openCLContext, cl_mem_flags flags, int width, int height);

	int width(){return m_width;}
	int height(){return m_height;}
	cl::Image2D *image(){return m_image;}

	bool write(cl::CommandQueue commandQueue, unsigned char *buffer);
	bool write(cl::CommandQueue commandQueue, unsigned char *buffer, cl::Event &event);

private:
	cl::Image2D *m_image;

	int m_width, m_height;
	cl::Context m_openCLContext;
};

class ImageGLHolder
{
public:
	ImageGLHolder():m_image(nullptr), m_width(0), m_height(0){};
	ImageGLHolder(cl::Context openCLContext, cl_mem_flags flags, int width, int height);
	~ImageGLHolder();

	bool create(cl::Context openCLContext, cl_mem_flags flags, int width, int height);

	int width(){return m_width;}
	int height(){return m_height;}
	cl::ImageGL *image(){return m_image;}
	GLuint texture(){return m_textureID;}

private:
	cl::ImageGL *m_image;
	GLuint m_textureID;

	int m_width, m_height;

	cl::Context m_openCLContext;
};

#endif // _containers_h_
