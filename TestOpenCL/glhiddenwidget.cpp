#include "glhiddenwidget.h"

GLHiddenWidget::GLHiddenWidget(QGLFormat format):
QGLWidget(format)
{
	setAutoBufferSwap(false);
	doneCurrent();
}

GLHiddenWidget::~GLHiddenWidget()
{
}

void GLHiddenWidget::glInit()
{
}

void GLHiddenWidget::glDraw()
{
}

void GLHiddenWidget::initializeGL()
{
}

void GLHiddenWidget::resizeGL(int width, int height)
{
}

void GLHiddenWidget::paintGL()
{
}

void GLHiddenWidget::paintEvent(QPaintEvent *)
{
}

void GLHiddenWidget::resizeEvent(QResizeEvent *event)
{
}