//
// Created by justasic on 7/26/15.
//
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>
#include <string>
#include <vector>
#include <QMessageBox>
#include "tinyformat.h"

#include "file.h"


bool is_directory(const char *dir)
{
	struct stat st_buf;
	memset(&st_buf, 0, sizeof(struct stat));

	if (stat(dir, &st_buf) != 0)
		return false;

	return S_ISDIR(st_buf.st_mode);
}

bool is_file(const char *file)
{
	struct stat st_buf;
	memset(&st_buf, 0, sizeof(struct stat));

	if (stat(file, &st_buf) != 0)
		return false;

	return S_ISREG(st_buf.st_mode);
}

std::string basename(const std::string &path)
{
	char *pathtmp = strndup(path.c_str(), path.length());
	char *cpath = ::basename(pathtmp);
	std::string ret(cpath);
	free(pathtmp);
	return ret;
}

std::string realpath(const std::string &path)
{
	char *cpath = ::realpath(path.c_str(), nullptr);
	std::string ret(cpath);
	free(cpath);
	return ret;
}

static inline std::vector<std::string> getdir(const std::string &dir)
{
	std::vector<std::string> files;
	DIR *dp;
	struct dirent *dirp;
	if ((dp  = opendir(dir.c_str())) == NULL)
	{
		QMessageBox::information(nullptr, "File Error", _("Error opening %s: %s (%d)", dir, strerror(errno), errno).c_str());
		//std::cout << "Error(" << errno << ") opening " << dir << std::endl;
		return files;
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		std::string dirn = dirp->d_name;
		if (dirn != "." && dirn != "..")
			files.push_back(dir+"/"+dirn);
	}

	closedir(dp);
	return files;
}

std::vector<std::string> RecursiveDirectoryList(const std::string &dir)
{
	std::vector<std::string> files;
	std::vector<std::string> tmp;
	if (is_directory(dir.c_str()))
	{
		files = getdir(dir);
		for (auto file : files)
		{
			if (is_directory((file).c_str()))
			{
				std::vector<std::string> tmp2 = RecursiveDirectoryList(file);
				tmp.insert(tmp.end(), tmp2.begin(), tmp2.end());
			}
		}
	}

	// Append the vectors together
	files.insert(files.end(), tmp.begin(), tmp.end());

	return files;
}
