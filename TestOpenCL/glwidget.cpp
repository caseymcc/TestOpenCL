#include "glwidget.h"
#include <boost/foreach.hpp>
#include <boost/format.hpp>

#include <QtCore/QEvent>

#define _USE_MATH_DEFINES
#include <math.h>
#include <GL/glu.h>
#include "CL/cl.hpp"

#include "loadShaders.h"

std::string vertexShader="\
#version 330\n\
\n\
layout(location = 0) in vec3 vertex;\n\
layout(location = 1) in vec2 vertexTexCoord;\n\
out vec2 texCoord;\n\
\n\
void main()\n\
{\n\
	texCoord=vertexTexCoord;\n\
	gl_Position=vec4(vertex, 1);\n\
}";

std::string fragmentShader="\
#version 330\n\
\n\
in vec2 texCoord;\n\
out vec3 color;\n\
uniform usampler2D textureSampler;\n\
\n\
void main()\n\
{\n\
uvec4 texel=texture2D(textureSampler, texCoord);\
vec4 normalizedColor=texel/255.0;\
color=normalizedColor.rgb;\
\n}";

GLWidget::GLWidget(QGLFormat format, const QGLWidget *shareWidget, QWidget *parent):
QGLWidget(format, parent, shareWidget),
m_init(0),
m_format(format),
m_shareWidget(shareWidget),
m_threadRun(true),
m_resize(true),
m_calculateQuadPos(true),
m_image(nullptr)
{
	setAutoBufferSwap(false);

	doneCurrent();
	m_thread=boost::thread(boost::bind(&GLWidget::drawThread, this));
}

GLWidget::~GLWidget()
{
	m_threadRun=false;
	
	while(!m_thread.try_join_for(boost::chrono::microseconds(100)))
		m_event.notify_one();
}

bool GLWidget::initialize()
{
	if(m_init == 0)
	{
		m_init=1;

		m_vertexBufferData.resize(18);

		m_vertexBufferData[0]=-1.0f;
		m_vertexBufferData[1]=1.0f;
		m_vertexBufferData[2]=0.0f;
		m_vertexBufferData[3]=-1.0f;
		m_vertexBufferData[4]=1.0f;
		m_vertexBufferData[5]=0.0f;
		m_vertexBufferData[6]=-1.0f;
		m_vertexBufferData[7]=-1.0f;
		m_vertexBufferData[8]=0.0f;
		m_vertexBufferData[9]=1.0f;
		m_vertexBufferData[10]=1.0f;
		m_vertexBufferData[11]=0.0f;
		m_vertexBufferData[12]=1.0f;
		m_vertexBufferData[13]=-1.0f;
		m_vertexBufferData[14]=-0.0f;
		m_vertexBufferData[15]=-1.0f;
		m_vertexBufferData[16]=-1.0f;
		m_vertexBufferData[17]=0.0f;

		m_textureBufferData.resize(12);

		m_textureBufferData[0]=0.0;
		m_textureBufferData[1]=0.0;
		m_textureBufferData[2]=1.0;
		m_textureBufferData[3]=0.0;
		m_textureBufferData[4]=0.0;
		m_textureBufferData[5]=1.0;
		m_textureBufferData[6]=1.0;
		m_textureBufferData[7]=0.0;
		m_textureBufferData[8]=1.0;
		m_textureBufferData[9]=1.0;
		m_textureBufferData[10]=0.0;
		m_textureBufferData[11]=1.0;
	
		initializeGLContext();
	}
	return true;
}


void GLWidget::initializeGLContext()
{
	//create context from inside thread
	QGLContext *glContext=new QGLContext(m_format, this);

	//share context with another QGLWidget
	if(m_shareWidget != NULL)
		glContext->create(m_shareWidget->context());
	setContext(glContext);

	//make sure new context is current
	makeCurrent();

	//load extensions with glew
	glewInit();

	glClearColor(0.2, 0.2, 0.2, 1.0f);
	glClearDepth(1.0f);

	//load shader and get uniforms
	m_programID=loadShaders(vertexShader, fragmentShader);
	m_textureSamplerID=glGetUniformLocation(m_programID, "textureSampler");

	//create and bind vertex array
	glGenVertexArrays(1, &m_vertexArrayID);
	glBindVertexArray(m_vertexArrayID);

	//create and bind texture
	glGenTextures(1, &m_textureID);
	glBindTexture(GL_TEXTURE_2D, m_textureID);

	//create and bind/load vertex array buffer
	glGenBuffers(1, &m_vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBufferData.size()*sizeof(GLfloat), m_vertexBufferData.data(), GL_STATIC_DRAW);

	//create and bind/load tex coord array buffer
	glGenBuffers(1, &m_texCoordBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_textureBufferData.size()*sizeof(GLfloat), m_textureBufferData.data(), GL_STATIC_DRAW);
}

void GLWidget::glInit()
{
	//stop QGLWidget standard behavior
}

void GLWidget::glDraw()
{
	//notify thread of draw request
	m_event.notify_one();
}

void GLWidget::initializeGL()
{
	//stop QGLWidget standard behavior
}

void GLWidget::resizeGL(int width, int height)
{
	//stop QGLWidget standard behavior
}

void GLWidget::paintGL()
{
	//stop QGLWidget standard behavior
}

void GLWidget::display(ImageGLHolder *clImageGL)
{
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);

		m_image=clImageGL;
		//check if we need to resize quad
		if((m_image->width() != m_imageWidth) || (m_image->height() != m_imageHeight))
		{
			//notify thread to resize quad
			m_calculateQuadPos=true;
		}
	}
	m_event.notify_one();
}

void GLWidget::resizeEvent(QResizeEvent *evt)
{
	//notify thread of resize event
	m_resize=true;
	m_event.notify_one();
}

void GLWidget::closeEvent(QCloseEvent *evt)
{
	//notify thread of close event
	m_threadRun=false;
	m_event.notify_one();
	QGLWidget::closeEvent(evt);
}

bool GLWidget::event(QEvent *e)
{
	if(e->type() == QEvent::Show)
		m_event.notify_one();
	else if(e->type() == QEvent::ParentChange) //The glContext will be changed, need to reinit openGl
	{
		//wait for thread to finish current work
		boost::unique_lock<boost::mutex> lock(m_mutex);
		bool ret=QGLWidget::event(e);
		
		//notify thread of re-init context
		m_init=false;
		m_event.notify_one();

		return ret;
	}
	return QGLWidget::event(e);
}

void GLWidget::drawThread()
{
	while(true)
	{
		boost::unique_lock<boost::mutex> lock(m_mutex);

		m_event.wait(lock); //wait for event

		if(!m_threadRun)
			break;

		if(m_init == 0)
			initialize();

		makeCurrent();

		if(m_calculateQuadPos)
			calculateQuadPos();

		if(m_resize)
			resizeMedia();

		glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
		glDepthRange (0.0, 1.0);

		if(m_image != nullptr)
		{
			glUseProgram(m_programID);
			glActiveTexture(GL_TEXTURE0);

			glBindTexture(GL_TEXTURE_2D, m_image->texture());
			glUniform1i(m_textureSamplerID, 0);

			glEnableVertexAttribArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

			glEnableVertexAttribArray(1);
			glBindBuffer(GL_ARRAY_BUFFER, m_texCoordBuffer);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (void*)0);
					
			glDrawArrays(GL_TRIANGLES, 0, 6);

			glDisableVertexAttribArray(0);
			glDisableVertexAttribArray(1);
		}
		swapBuffers();
		doneCurrent();		
	}
}

void GLWidget::resizeMedia()
{
	m_resize=false;

	makeCurrent();

	int localWidth=width();
	int localHeight=height();
	float ratio=(float)width()/height();

	glViewport(0, 0, width(), height());
	calculateQuadPos();
}

void GLWidget::calculateQuadPos()
{
	if(m_image == nullptr) //can't calculate pos without and image
		return;

	m_calculateQuadPos=false;
	m_imageWidth=m_image->width();
	m_imageHeight=m_image->height();

	int localWidth=width();
	int localHeight=height();
	float aspect;
	float positionX, positionY;

	aspect=(float)m_image->height()/m_image->width();

	if(localWidth*aspect > localHeight)
	{
		positionY=1.0;
		positionX=localHeight/(aspect*localWidth);
	}
	else
	{
		positionX=1.0;
		positionY=(localWidth*aspect)/localHeight;
	}

	m_vertexBufferData[0]=-positionX;
	m_vertexBufferData[1]=positionY;
	m_vertexBufferData[3]=positionX;
	m_vertexBufferData[4]=positionY;
	m_vertexBufferData[6]=-positionX;
	m_vertexBufferData[7]=-positionY;
	m_vertexBufferData[9]=positionX;
	m_vertexBufferData[10]=positionY;
	m_vertexBufferData[12]=positionX;
	m_vertexBufferData[13]=-positionY;
	m_vertexBufferData[15]=-positionX;
	m_vertexBufferData[16]=-positionY;

	//update vertex array
	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, m_vertexBufferData.size()*sizeof(GLfloat), m_vertexBufferData.data(), GL_STATIC_DRAW);
}