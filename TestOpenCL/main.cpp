#include "testopencl.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	TestOpenCL w;
	w.show();
	return a.exec();
}
