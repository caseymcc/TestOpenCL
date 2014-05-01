#include "containers.h"
#include <assert.h> 

ImageHolder::ImageHolder(cl::Context openCLContext, cl_mem_flags flags, int width, int height)
{
	create(openCLContext, flags, width, height);
}

bool ImageHolder::create(cl::Context openCLContext, cl_mem_flags flags, int width, int height)
{
	cl_int error;

	m_openCLContext=openCLContext;
	m_width=width;
	m_height=height;
	m_image=new cl::Image2D(m_openCLContext, flags, cl::ImageFormat(CL_RGBA, CL_UNSIGNED_INT8), m_width, m_height, 0, NULL, &error);

	if(error != CL_SUCCESS)
	{
		m_image=nullptr;
		assert(false);
		return false;
	}
	return true;
}

ImageHolder::~ImageHolder()
{
	if(m_image != nullptr)
	{
		delete m_image;
	}
}

bool ImageHolder::write(cl::CommandQueue commandQueue, unsigned char *buffer)
{
	cl::Event event;
	bool status=write(commandQueue, buffer, event);

	if(status)
	{
		commandQueue.flush();
		event.wait();
	}
	return status;
}

bool ImageHolder::write(cl::CommandQueue commandQueue, unsigned char *buffer, cl::Event &event)
{
	cl_int status;
	cl::size_t<3> origin;
	cl::size_t<3> region;

	origin[0]=0;
	origin[1]=0;
	origin[2]=0;

	region[0]=m_width;
	region[1]=m_height;
	region[2]=1;

	status=commandQueue.enqueueWriteImage(*m_image, CL_FALSE, origin, region, 0, 0, buffer, NULL, &event);
	if(status != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}

ImageGLHolder::ImageGLHolder(cl::Context openCLContext, cl_mem_flags flags, int width, int height)
{
	create(openCLContext, flags, width, height);
}

ImageGLHolder::~ImageGLHolder()
{
	if(m_image != nullptr)
	{
		delete m_image;
	}
}

bool ImageGLHolder::create(cl::Context openCLContext, cl_mem_flags flags, int width, int height)
{
	cl_int error;

	m_openCLContext=openCLContext;
	m_width=width;
	m_height=height;
	//create output image and texture
	//for opengl to be able to read the opencl image it need to be declared with the below formats
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8UI, m_width, m_height, 0, GL_RGBA_INTEGER, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 

	m_image=new cl::ImageGL(m_openCLContext, flags, GL_TEXTURE_2D, 0, m_textureID, &error);
	if(error != CL_SUCCESS)
	{
		m_image=nullptr;
		assert(false);
		return false;
	}
	return true;
}
