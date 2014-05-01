#ifndef _GLHiddenWidget_h
#define _GLHiddenWidget_h

#include <GL/glew.h>
#include <QtOpenGL/QGLWidget>

class GLHiddenWidget : public QGLWidget
{
	Q_OBJECT
public:
	GLHiddenWidget(QGLFormat format);
	~GLHiddenWidget();

	std::vector<GLuint> textures(int count);

protected:
	virtual void glInit();
	virtual void glDraw();

	virtual void initializeGL();
	virtual void resizeGL(int width, int height);
	virtual void paintGL();

    void resizeEvent(QResizeEvent *event);
    void paintEvent(QPaintEvent *);

private:
};

#endif // _GLHiddenWidget_h
