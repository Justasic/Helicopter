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

	#if 0
	libxmp_filelist = const_cast<const char **>(xmp_get_format_list());

	printf("Supported file types:\n");
	const char **filetypes = libxmp_filelist;
	const char *typeit = nullptr;
	while((typeit = *filetypes++))
	{
		printf(" %s\n", typeit);
	}

	struct xmp_test_info info;
	switch(xmp_test_module(const_cast<char*>(args[1].c_str()), &info))
	{
		case -XMP_ERROR_SYSTEM:
			QMessageBox::information(nullptr, "File Error",
			                         _("Error opening %s: %s (%d)", args[1], strerror(errno), errno).c_str());
			break;
		case -XMP_ERROR_FORMAT:
			QMessageBox::information(nullptr, "File Error", _("%s is not a valid tracker file!", args[1]).c_str());
			break;
		case 0:
		{
			char *path = strdup(args[1].c_str());
			printf("Filename: %s\nSong: %s\nTracker: %s\n", basename(path), info.name, info.type);
			free(path);
		}
		default:
			break;
	}
	#endif





	// We have a Qt application.
	QApplication application(argc, argv);
	QMessageBox::information(nullptr, "Scanning files...", "Scanning for files, this may take a few minutes...");
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
