#ifndef _GLWidget_h_
#define _GLWidget_h_

#include <GL/glew.h>
#include <QtOpenGL/QGLWidget>
#include <boost/thread.hpp>
#include <boost/atomic.hpp>

#include "containers.h"

class GLWidget : public QGLWidget
{
	Q_OBJECT

public:
	GLWidget(QGLFormat format, const QGLWidget *shareWidget=0, QWidget *parent=0);
	~GLWidget();

	bool initialize();

	void display(ImageGLHolder *image);
	void drawThread();

protected:
	void initializeGLContext();

	virtual void glInit();
	virtual void glDraw();

	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

	void resizeEvent(QResizeEvent *evt);
	void closeEvent(QCloseEvent *evt);

	bool event(QEvent *e);

	void resizeMedia();
	void calculateQuadPos();

private:
	boost::atomic<int> m_init;
	QGLFormat m_format;
	const QGLWidget *m_shareWidget;

	boost::thread m_thread;
	bool m_threadRun;
	boost::condition_variable m_event;
	boost::mutex m_mutex;

	bool m_resize;
	bool m_calculateQuadPos;

	GLuint m_programID;
	GLuint m_textureSamplerID;
	GLuint m_textureID;
	GLuint m_vertexArrayID;
	GLuint m_vertexBuffer;
	GLuint m_texCoordBuffer;

	std::vector<GLfloat> m_vertexBufferData;
	std::vector<GLfloat> m_textureBufferData;

	ImageGLHolder *m_image;
	int m_imageWidth;
	int m_imageHeight;
};

#endif // _GLWidget_h_
