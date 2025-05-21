#ifndef BINARYBACKUP_HPP
#define BINARYBACKUP_HPP

#include <database.hpp>
#include <vector>
#include <iostream>
#include <fstream>
#include <thread> 
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <direct.h>
#include <string>

std::ostringstream folderName;

int createFolder();
void convert_to_binary();

#endif

