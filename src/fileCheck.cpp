
#include <iostream>
#include <fileCheck.hpp>

// Function to check if the file's timestamp is within the requested time range
bool isInTimeRange(const std::string& filename, int startSec, int endSec) {
    size_t startPos = filename.find("backup[");
    size_t endPos = filename.find("].dat");

    if (startPos == std::string::npos || endPos == std::string::npos)
        return false;

    std::string timeStr = filename.substr(startPos + 7, endPos - (startPos + 7)); // Extract "HH-MM"
    int h, m;
    sscanf(timeStr.c_str(), "%2d-%2d", &h, &m);
    int totalSec = h * 3600 + m * 60;

    return totalSec >= startSec && totalSec <= endSec;
}

DateTimeInput getDateTimeInput() {
    DateTimeInput input;
    std::cout << "Enter DD MM YY: ";
    std::cin >> input.day >> input.month >> input.year;
    
    std::cout << "Enter start time (HH MM): ";
    std::cin >> input.startHour >> input.startMinute;
    
    std::cout << "Enter end time (HH MM): ";
    std::cin >> input.endHour >> input.endMinute;
    return input;
}

std::vector<std::filesystem::directory_entry> filterFiles(std::filesystem::path dataPath, int startSec, int endSec) {
    std::vector<std::filesystem::directory_entry>entryFiles;
    for (std::filesystem::directory_iterator dirIt(dataPath); dirIt != std::filesystem::directory_iterator(); ++dirIt) {
        std::filesystem::directory_entry entry = *dirIt;
        std::string fname = entry.path().filename().string();

        if (!entry.is_regular_file() || fname.find("backup[") == std::string::npos) {
            continue;
        }

        if (!isInTimeRange(fname, startSec, endSec)) {
            continue;
        }
        entryFiles.push_back(entry);
    }
    return entryFiles;
}