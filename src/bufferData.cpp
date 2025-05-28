#include <bufferData.hpp>
#include <omp.h>

#define BUFFER_LEN 26
#define TIMEOUT 60

std::unordered_map<std::string, std::vector<CPU_DATA>> logBuffer;
std::unordered_map<std::string, time_t> lastWindowTime;

struct tm *getLocalTime() {
    time_t currentTime;
    time(&currentTime);

    struct tm *localTime = localtime(&currentTime);

    return localTime;
}

void addLogEntry(CPU_DATA &data) {
    char dateTime[BUFFER_LEN];

    size_t result = strftime(dateTime, sizeof(dateTime), "%H-%M", getLocalTime());

    if (result == 0) return;

    std::string key(dateTime);
    logBuffer[key].push_back(data);
    
    lastWindowTime[key] = time(nullptr);
}

void flushBuffer() {
    std::unordered_map<std::string, std::vector<CPU_DATA>> entriesToFlush;
    std::vector<std::string> keysToErase;
    std::string folderPath;
    bool isFolderCreated = false;

    {
        std::lock_guard<std::mutex> lock(dataMutex);

        if (logBuffer.empty()) return;

        time_t now = time(nullptr);

        for (auto it = logBuffer.begin(); it != logBuffer.end(); ++it) {
            const std::string &key = it->first;
            time_t lastSeen = lastWindowTime[key];

            if (difftime(now, lastSeen) >= TIMEOUT && !it->second.empty()) {
                entriesToFlush[key] = std::move(it->second);
                keysToErase.push_back(key);
            }
        }

        for (const auto& key : keysToErase) {
            logBuffer.erase(key);
            lastWindowTime.erase(key);
        }
    }

    if (entriesToFlush.empty()) return;

    folderPath = createFolder("Backup");

    #pragma omp parallel for schedule(dynamic)
    for (int i = 0; i < static_cast<int>(entriesToFlush.size()); ++i) {
        auto it = std::next(entriesToFlush.begin(), i);
        const std::string& key = it->first;
        std::vector<CPU_DATA>& records = it->second;

        convert_to_binary(records, key, folderPath);
    }
}


