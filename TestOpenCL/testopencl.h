#ifndef TESTOPENCL_H
#define TESTOPENCL_H

#include <QtWidgets/QMainWindow>
#include <QtCore/QThread>

#include "ui_testopencl.h"
#include "containers.h"

#include <boost/shared_ptr.hpp>
#include <boost/thread.hpp>

#include "CL/cl.hpp"

#include "glhiddenwidget.h"
#include "glwidget.h"

class TestOpenCL;

class TestOpenCLThread:public QThread
{
	Q_OBJECT
public:
	TestOpenCLThread(TestOpenCL *parent):m_parent(parent){}
	void run() Q_DECL_OVERRIDE;

private:
	TestOpenCL *m_parent;
};

class TestOpenCL : public QMainWindow
{
	Q_OBJECT

public:
	TestOpenCL(QWidget *parent = 0);
	~TestOpenCL();

	void process();

private:
	bool initOpenCl();
	bool initTexture();
	bool initKernel();

	Ui::TestOpenCLClass ui;

	bool m_process;
	TestOpenCLThread *m_processThread;

	GLHiddenWidget *m_hiddenGl;
	GLWidget *m_mainView;

	QDockWidget *m_auxViewDockWidget;
	GLWidget *m_auxView;

	//opencl
	cl::Platform m_openCLPlatform;
	cl::Device m_openCLDevice;
	cl::Context m_openCLContext;
	cl::CommandQueue m_openCLComandQueue;
	cl::Kernel m_kernel;

	ImageHolder m_testImage;
	std::vector<ImageGLHolder> m_outputImage;
//	GLuint m_testTextureID;
};

#endif // TESTOPENCL_H
