#include "testopencl.h"
#include "loadKernel.h"

#include <QtWidgets/QDockWidget>

#include <boost/filesystem.hpp>
#include <boost/math/constants/constants.hpp>

#include <assert.h> 

void TestOpenCLThread::run(){m_parent->process();}

TestOpenCL::TestOpenCL(QWidget *parent):
QMainWindow(parent),
m_process(true)
{
	ui.setupUi(this);

	QGLFormat qglFormat;
	
	//create opengl core 3.3 context
	qglFormat.setVersion(3, 3); 
	qglFormat.setProfile(QGLFormat::CoreProfile);

	//create hidden QGLWidget
	m_hiddenGl=new GLHiddenWidget(qglFormat);
	m_hiddenGl->setVisible(false);

	//create main view
	m_mainView=new GLWidget(qglFormat, m_hiddenGl);
	m_mainView->setMinimumSize(QSize(500, 250));
	setCentralWidget(m_mainView);

	//create aux view
	m_auxView=new GLWidget(qglFormat, m_hiddenGl);
	m_auxView->setMinimumSize(QSize(400, 200));
	m_auxViewDockWidget=new QDockWidget;
	m_auxViewDockWidget->setWidget(m_auxView);
	addDockWidget(Qt::TopDockWidgetArea, m_auxViewDockWidget);

	m_processThread=new TestOpenCLThread(this);

	//move the hidden widget to the new thread id
	m_hiddenGl->doneCurrent();
	m_hiddenGl->context()->moveToThread(m_processThread);
		
	m_processThread->start();
}

TestOpenCL::~TestOpenCL()
{
	m_process=false;
	if(!m_processThread->wait(2000))
		m_processThread->terminate();
}

bool TestOpenCL::initOpenCl()
{
	//locate platform and GPU device
	std::vector<cl::Platform> platforms;
	std::vector<cl::Device> devices;
	bool found=false;

	cl::Platform::get(&platforms);

	for(size_t i=0; i<platforms.size(); ++i)
	{
		platforms[i].getDevices(CL_DEVICE_TYPE_GPU, &devices);

		if(devices.empty())
			continue;

		m_openCLPlatform=platforms[i];
		m_openCLDevice=devices[0];
		found=true;
		break;
	}

	if(!found)
	{
		//no platform or device found for GPU
		assert(false);
		return false;
	}

	//make sure opengl context current
	m_hiddenGl->makeCurrent();

#ifdef _WINDOWS
	HDC hdc;
	HGLRC glHandle;

	hdc=wglGetCurrentDC();
	glHandle=wglGetCurrentContext();

	cl_context_properties clContextProps[]={ 
		CL_CONTEXT_PLATFORM, (cl_context_properties)m_openCLPlatform(),
		CL_WGL_HDC_KHR, (intptr_t) hdc,
		CL_GL_CONTEXT_KHR, (intptr_t) glHandle, 0
	};
#else //_WINDOWS
	assert(false);
	return false;
#endif //_WINDOWS

	cl_int error;

	//open openCL context with openGL interop
	m_openCLContext=cl::Context(m_openCLDevice, clContextProps, NULL, NULL, &error);

	if(error != CL_SUCCESS)
	{
		assert(false);
		return false;
	}

	m_openCLComandQueue=cl::CommandQueue(m_openCLContext, m_openCLDevice, 0, &error);

	if(error != CL_SUCCESS)
	{
		assert(false);
		return false;
	}
	return true;
}

bool TestOpenCL::initTexture()
{
	//make sure opengl context current
	m_hiddenGl->makeCurrent();

	QImage testImage;

	if(testImage.load("testImage.jpg"))
	{
		QImage::Format format=testImage.format();

		if((format != QImage::Format_RGB32) && (format != QImage::Format_ARGB32))
		{
			//needs to be 32 bit per pixel, as it is assumed later
			assert(false);
			return false;
		}

		//create input image
		m_testImage.create(m_openCLContext, CL_MEM_READ_ONLY, testImage.width(), testImage.height());
		//write image to device
		m_testImage.write(m_openCLComandQueue, testImage.bits());
	
		m_outputImage.resize(3);
		//create output image and texture
		m_outputImage[0].create(m_openCLContext, CL_MEM_WRITE_ONLY, testImage.width(), testImage.height());
		m_outputImage[1].create(m_openCLContext, CL_MEM_WRITE_ONLY, testImage.width(), testImage.height());
		m_outputImage[2].create(m_openCLContext, CL_MEM_WRITE_ONLY, testImage.width(), testImage.height());
	}
	else
	{
		assert(false);
		return false;
	}
	return true;
}

bool TestOpenCL::initKernel()
{
	boost::filesystem::path kernelDirectory=boost::filesystem::current_path();

	kernelDirectory/="kernels";
	std::string kernelPath=kernelDirectory.string()+"/roll.cl";

	return loadKernelFile(m_openCLContext, m_openCLDevice, kernelPath, "rollHorizontal", m_kernel);
}

void TestOpenCL::process()
{
	//create opencl context and command queue
	if(!initOpenCl())
		return;

	//create texture, must be done after openCl context created (at least for AMD)
	if(!initTexture())
		return;

	//load and compile kernel
	if(!initKernel())
		return;

	cl::Event event;
	cl_int status;
	cl::NDRange globalThreads(m_testImage.width(), m_testImage.height());
	int pos=0;
	int outputIndex=0;

	while(m_process)
	{
		//setup kernel arguments
		status=m_kernel.setArg(0, *m_testImage.image());
		if(status != CL_SUCCESS)
			assert(false);
		
		status=m_kernel.setArg(1, *m_outputImage[outputIndex].image());
		if(status != CL_SUCCESS)
			assert(false);

		status=m_kernel.setArg(2, pos);
		if(status != CL_SUCCESS)
			assert(false);

		//execute kernel
		status=m_openCLComandQueue.enqueueNDRangeKernel(m_kernel, cl::NullRange, globalThreads, cl::NullRange, NULL, &event);
		if(status != CL_SUCCESS)
			assert(false);

		m_openCLComandQueue.flush();

		pos++;
		if(pos > m_testImage.width())
			pos=0;

		event.wait();
		m_openCLComandQueue.finish();

		m_mainView->display(&m_outputImage[outputIndex]);
		m_auxView->display(&m_outputImage[outputIndex]);

		outputIndex++;
		if(outputIndex >= m_outputImage.size())
			outputIndex=0;
	}
	m_openCLComandQueue.finish();
}
