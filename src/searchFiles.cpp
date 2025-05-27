#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Function to check if the file's timestamp is within the requested time range
bool isInTimeRange(const std::string& filename, int startSec, int endSec) {
    size_t startPos = filename.find("backup[");
    size_t endPos = filename.find("].dat");

    if (startPos == std::string::npos || endPos == std::string::npos)
        return false;

    std::string timeStr = filename.substr(startPos + 5, endPos - startPos - 5); // Extract "HH-MM"
    int h, m;
    sscanf(timeStr.c_str(), "%2d-%2d", &h, &m);
    int totalSec = h * 3600 + m * 60;

    return totalSec >= startSec && totalSec <= endSec;
}

int main() {
    int date, month, year;
    std::cout << "Enter DD/MM/YY: ";
    std::cin >> date >> month >> year;

    int sh, sm, eh, em;
    std::cout << "Enter start time (HH MM): ";
    std::cin >> sh >> sm;
    std::cout << "Enter end time (HH MM): ";
    std::cin >> eh >> em;

    int startSec = sh * 3600 + sm * 60;
    int endSec = eh * 3600 + em * 60;

    // Store total CPU usage and count of appearances for each client
    std::map<std::string, std::pair<double, int>> clientStats;

    double maxFileAvg = -1.0;
    std::string maxFileName;
    int filesRead = 0;

    std::string strPath = std::to_string(date) + '-' + std::to_string(month) + '-' + std::to_string(year);
    // Loop through files in the current directory
    std::filesystem::path dataPath = strPath;
    for (std::filesystem::directory_iterator dirIt(dataPath); dirIt != std::filesystem::directory_iterator(); ++dirIt) {
        std::filesystem::directory_entry entry = *dirIt;
        std::string fname = entry.path().filename().string();

        if (!entry.is_regular_file() || fname.find("backup[") == std::string::npos) {
            continue;
        }

        // Only include files within the time range
        if (!isInTimeRange(fname, startSec, endSec)) {
            continue;
        }

        std::ifstream in(entry.path());
        if (!in) {
            continue; // Skip unreadable files
        }

        json j;
        in >> j;

        double fileSum = 0.0;
        int fileCount = 0;

        // Loop through all entries (clients) in the JSON file
        for (json::iterator jt = j.begin(); jt != j.end(); ++jt) {
            std::string client = jt.key();
            double usage = jt.value();

            clientStats[client].first += usage; // Add to total CPU
            clientStats[client].second++;       // Increment file count

            fileSum += usage;
            fileCount++;
        }

        // Track the file with the highest average CPU usage
        if (fileCount > 0) {
            double avg = fileSum / fileCount;
            if (avg > maxFileAvg) {
                maxFileAvg = avg;
                maxFileName = fname;
            }
        }

        filesRead++;
    }

    if (filesRead == 0) {
        std::cout << "No files found in the time range.\n";
        return 0;
    }

    // Find top client (highest average) and least connected client
    std::string topClient;
    double highestAvg = -1.0;

    std::string leastClient;
    int fewestFiles = std::numeric_limits<int>::max();

    for (std::map<std::string, std::pair<double, int>>::iterator it = clientStats.begin(); it != clientStats.end(); ++it) {
        std::string client = it->first;
        double total = it->second.first;
        int count = it->second.second;
        double avg = total / count;

        if (avg > highestAvg) {
            highestAvg = avg;
            topClient = client;
        }

        if (count < fewestFiles) {
            fewestFiles = count;
            leastClient = client;
        }
    }

    // Output the report
    std::cout << "\n====== CPU USAGE REPORT ======\n";
    std::cout << "Most CPU usage client (avg): " << topClient << " with " << highestAvg << "%\n";
    std::cout << "Least connected client: " << leastClient << " (in " << fewestFiles << " files)\n";
    std::cout << "File with highest overall average CPU: " << maxFileName << " (" << maxFileAvg << "%)\n";

    return 0;
}