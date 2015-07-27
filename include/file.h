//
// Created by justasic on 7/26/15.
//
#include <string>
#include <vector>
#pragma once
extern bool is_directory(const char *dir);
extern bool is_file(const char *file);
extern std::vector<std::string> RecursiveDirectoryList(const std::string &dir);
extern std::string basename(const std::string &path);
extern std::string realpath(const std::string &path);
