//
// Created by justasic on 7/26/15.
//
#include <string.h>
#include <sys/dir.h>
#include <sys/stat.h>

#include "file.h"

bool is_directory(const char *dir)
{
	struct stat st_buf;
	memset(&st_buf, 0, sizeof(struct stat));

	if (stat(dir, &st_buf) != 0)
		return false;

	if (S_ISDIR(st_buf.st_mode))
		return true;
	return false;
}

bool is_file(const char *file)
{
	struct stat st_buf;
	memset(&st_buf, 0, sizeof(struct stat));

	if (stat(file, &st_buf) != 0)
		return false;

	if (S_ISREG(st_buf.st_mode))
		return true;
	return false;
}
