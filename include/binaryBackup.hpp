#ifndef BINARYBACKUP_HPP
#define BINARYBACKUP_HPP

#include <iostream>
#include <fstream>
#include <thread> 
#include <chrono>
#include <sstream>
#include <iomanip>
#include <direct.h>
#include <binaryBackup.hpp>
#include <database.hpp>

void convert_to_binary(const std::vector<CPU_DATA> &records, const std::string &timeframe, const std::string &folderPath);
std::string createFolder(std::string parentFolder);

#endif