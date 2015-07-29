// Include standard C/C++ libraries
#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <libgen.h>

// Qt standard libraries
#include <QApplication>
#include <QMessageBox>

// Include libxmp to identify files.
#include <xmp.h>
#include <QtCore/qtimer.h>
#include <functional>
// Include our project files.
#include "mainwidget.h"
#include "file.h"
#include "tinyformat.h"
#include "pulseaudio_interface.h"

std::vector<std::string> musicfiles;
Pulseaudio *pulse;
static const char **libxmp_filelist;

bool ValidTrackerFile(const std::string &tracker)
{
	struct xmp_test_info info;
	return xmp_test_module(const_cast<char *>(tracker.c_str()), &info) == 0;
}

std::vector<std::string> ScanForTrackerFiles(std::vector<std::string> directories)
{
	std::vector<std::string> validtrackerfiles;
	for (auto it : directories)
	{
		// Get a directory listing.

		// If this arg is a directory then scan it, otherwise check it as a file.
		if (is_directory(it.c_str()))
		{
			std::vector<std::string> files = RecursiveDirectoryList(realpath(it));

			for (auto it2 : files)
			{
				// Find all valid files.
				if (ValidTrackerFile(it2))
					validtrackerfiles.push_back(it2);
			}
		}

		if (is_file(it.c_str()))
		{
			// Find all valid files.
			if (ValidTrackerFile(it))
				validtrackerfiles.push_back(it);
		}
	}

	return validtrackerfiles;
}

int main(int argc, char **argv)
{
	std::vector<std::string> args(argv, argv+argc);

	// We have a Qt application.
	QApplication application(argc, argv);
	//QMessageBox::information(nullptr, "Scanning files...", "Scanning for files, this may take a few minutes...");
	musicfiles = ScanForTrackerFiles(args);
	tfm::printf("Found %d valid tracker files\n", musicfiles.size());

	// Initialize Pulseaudio
	pulse = new Pulseaudio();
	pulse->Initialize();

	// Form our widget.
	MainWidget w;

	QTimer *pulsetimer = new QTimer;
	MainWidget::connect(pulsetimer, &QTimer::timeout, pulse, std::bind(&Pulseaudio::DoIteration, pulse));

	// Start the pulsetimer so pulseaudio runs.
	pulsetimer->start();

	// Show the window
	w.show();

	// Run our application.
	int ret = application.exec();

	// Clean up.
	delete pulsetimer;
	delete pulse;

	return ret;
}
