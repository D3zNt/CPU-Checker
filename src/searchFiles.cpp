#include <fstream>
#include <unordered_map>
#include <Windows.h>
#include <direct.h>
#include <dataAnalysis.hpp>
#include <fileCheck.hpp>
#include <fileHandling.hpp>
#include <iostream>
#include <iomanip>
#include <sstream>

using json = nlohmann::json;

#define BASE_FOLDER_NAME "Analysis"

int main() {
    DateTimeInput input = getDateTimeInput();

    int startSec = input.startHour * 3600 + input.startMinute * 60;
    int endSec = input.endHour * 3600 + input.endMinute * 60;
    
    double maxCPUAvg = -1.0;
    std::string maxFileName;

    std::string strPath = "Backup/" + std::to_string(input.day) + '-' + input.month + '-' + std::to_string(input.year) + '/';
    std::filesystem::path dataPath = strPath;

    if (!std::filesystem::exists(dataPath) || !std::filesystem::is_directory(dataPath)) {
        std::cerr << "Error: The directory \"" << strPath << "\" does not exist or is not a valid directory.\n";
        return 1;
    }

    std::vector<std::filesystem::directory_entry> entries = filterFiles(dataPath, startSec, endSec);
    int numEntries = entries.size();

    if (numEntries == 0) {
        std::cout << "No files found in the time range.\n";
        return 0;
    }

    for (int i = 0; i < numEntries; ++i) {
        std::filesystem::directory_entry entry = entries[i];
        std::string fname = entry.path().filename().string();

        std::ifstream in(entry.path(), std::ios::binary);
        if (!in) {
            continue;
        }

        double fileCPUSum = 0.0;

        size_t logCount = deserializeBinaryFile(in, fileCPUSum);

        // Track the file with the highest average CPU usage
        double avgCPU = fileCPUSum / logCount;
        if (avgCPU > maxCPUAvg) {
            maxCPUAvg = avgCPU;
            maxFileName = fname;
        }
    }

    json result = analyzeClientStats(maxCPUAvg, maxFileName);
    
    std::string baseFolder = BASE_FOLDER_NAME;
    std::string dateFolderPath = createFolder(baseFolder) + '/';
    
    /* Output to JSON file */
    // Add zero-padding to the time
    std::ostringstream filenameStream;
    filenameStream << dateFolderPath
                << "analysis["
                << std::setw(2) << std::setfill('0') << input.startHour << '-'
                << std::setw(2) << std::setfill('0') << input.startMinute << '_'
                << std::setw(2) << std::setfill('0') << input.endHour << '-'
                << std::setw(2) << std::setfill('0') << input.endMinute
                << "].json";    
    
    std::string analysisFilename = filenameStream.str();
    std::ofstream ofile(analysisFilename);
    
    ofile << std::setw(4) << result << std::endl;
    if (!ofile.is_open()) {
        std::cout << "Failed to create file: " << analysisFilename << "\n";
        perror("Reason");
        return 1;
    }

    std::cout << analysisFilename << " created successfully.";

    return 0;
}