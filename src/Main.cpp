// Include standard C/C++ libraries
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>

// Qt standard libraries
#include <QApplication>

// Include our project files.
#include "mainwidget.h"

int main(int argc, char **argv)
{
	std::vector<std::string> args(argv, argv+argc);

	// We have a Qt application.
	QApplication application(argc, argv);
	// Form our widget.
	MainWidget w;

	// Show the window
	w.show();

	return application.exec();
}
