#ifndef FILECHECK_HPP
#define FILECHECK_HPP

#include <filesystem>
#include <string>
#include <vector>
#include <database.hpp>

bool isInTimeRange(const std::string& filename, int startSec, int endSec);
DateTimeInput getDateTimeInput();
std::vector<std::filesystem::directory_entry> filterFiles(std::filesystem::path dataPath, int startSec, int endSec);

#endif