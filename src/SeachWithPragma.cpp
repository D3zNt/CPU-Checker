#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <map>
#include <nlohmann/json.hpp>
#include <limits>
#include <mutex>
#include <omp.h>

using json = nlohmann::json;

bool isInTimeRange(const std::string& filename, int startSec, int endSec) {
    size_t startPos = filename.find("data[");
    size_t endPos = filename.find("].json");

    if (startPos == std::string::npos || endPos == std::string::npos)
        return false;

    std::string timeStr = filename.substr(startPos + 5, endPos - startPos - 5); // Extract "HH-MM-SS"
    int h, m, s;
    sscanf(timeStr.c_str(), "%2d-%2d-%2d", &h, &m, &s);
    int totalSec = h * 3600 + m * 60 + s;

    return totalSec >= startSec && totalSec <= endSec;
}

int main() {
    int sh, sm, eh, em;
    std::cout << "Enter start time (HH MM): ";
    std::cin >> sh >> sm;
    std::cout << "Enter end time (HH MM): ";
    std::cin >> eh >> em;

    int startSec = sh * 3600 + sm * 60;
    int endSec = eh * 3600 + em * 60;

    std::map<std::string, std::pair<double, int>> clientStats;

    double maxFileAvg = -1.0;
    std::string maxFileName;
    int filesRead = 0;

    std::filesystem::path dataPath = "../testJson";
    std::vector<std::filesystem::directory_entry> entries;

    for (std::filesystem::directory_iterator dirIt(dataPath); dirIt != std::filesystem::directory_iterator(); ++dirIt) {
        std::filesystem::directory_entry entry = *dirIt;
        std::string fname = entry.path().filename().string();
        if (entry.is_regular_file() && fname.find("data[") != std::string::npos) {
            entries.push_back(entry);
        }
    }

    int numEntries = entries.size();

    #pragma omp parallel for default(none) shared(entries, clientStats, startSec, endSec, maxFileAvg, maxFileName, filesRead) 
    for (int i = 0; i < numEntries; ++i) {
        std::filesystem::directory_entry entry = entries[i];
        std::string fname = entry.path().filename().string();

        if (!isInTimeRange(fname, startSec, endSec)) {
            continue;
        }

        std::ifstream in(entry.path());
        if (!in) {
            continue;
        }

        json j;
        in >> j;

        double fileSum = 0.0;
        int fileCount = 0;

        std::map<std::string, double> localUsage;
        std::map<std::string, int> localCount;

        for (json::iterator jt = j.begin(); jt != j.end(); ++jt) {
            std::string client = jt.key();
            double usage = jt.value();

            localUsage[client] += usage;
            localCount[client] += 1;

            fileSum += usage;
            fileCount++;
        }

        // Merge local maps into shared one
        #pragma omp critical
        {
            for (std::map<std::string, double>::iterator it = localUsage.begin(); it != localUsage.end(); ++it) {
                clientStats[it->first].first += it->second;
                clientStats[it->first].second += localCount[it->first];
            }

            if (fileCount > 0) {
                double avg = fileSum / fileCount;
                if (avg > maxFileAvg) {
                    maxFileAvg = avg;
                    maxFileName = fname;
                }
            }

            filesRead++;
        }
    }

    if (filesRead == 0) {
        std::cout << "No files found in the time range.\n";
        return 0;
    }

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

    std::cout << "\n====== CPU USAGE REPORT ======\n";
    std::cout << "Most CPU usage client (avg): " << topClient << " with " << highestAvg << "%\n";
    std::cout << "Least connected client: " << leastClient << " (in " << fewestFiles << " files)\n";
    std::cout << "File with highest overall average CPU: " << maxFileName << " (" << maxFileAvg << "%)\n";

    return 0;
}
