#ifndef FILEHANDLING_HPP
#define FILEHANDLING_HPP

#include <iostream>
#include <fstream>
#include <thread> 
#include <chrono>
#include <sstream>
#include <iomanip>
#include <direct.h>
#include <database.hpp>

void convert_to_binary(const std::vector<CPU_DATA> &records, const std::string &timeframe, const std::string &folderPath);
std::string createFolder(std::string parentFolder);
size_t deserializeBinaryFile(std::ifstream &in, double &fileCPUSum);

#endif