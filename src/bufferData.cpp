#include <bufferData.hpp>
#include <database.hpp>

#define BUFFER_LEN 26

std::unordered_map<std::string, std::vector<CPU_DATA>> logBuffer;
std::unordered_map<std::string, time_t> lastWindowTime;

#define TIMEOUT 60

struct tm *getLocalTime() {
    time_t currentTime;
    time(&currentTime);

    struct tm *localTime = localtime(&currentTime);

    return localTime;
}

void addLogEntry(CPU_DATA &data) {
    char dateTime[BUFFER_LEN];

    int result = strftime(dateTime, sizeof(dateTime), "%a %b %d %H:%M %Y", getLocalTime());

    if (result == 0) return;

    std::string key(dateTime);
    logBuffer[key].push_back(data);
    
    lastWindowTime[key] = time(nullptr);
}

/*
First approach:
Function is going to be checked every second and run in the background in a seperate thread
Its going to flush the buffer if the given conditions are fulfilled:
- At least 1 client
- data is sent 1 minute ago

basically if the current server time is 15:46, all data stored 1 minute ago (15:45)
will be flushed to the binary file
{
    "2025-22-05 15:45": ["bas ad", 25], ["asdadsd ", 23],
    "2025-22-05 15: 46": ["bas ad", 19], ["asdadsd ", 10],
}

- Client is going to be sending data every 10 seconds / whatever u want
- Binary files are created based on server time to the nearest minute (not seconds)
- Considerations:
    * client timestamp is currently based on UTC for standardization (need conversion during analysis)
    * the time range of the data may differ from the server time 
    (basically if log file is for 14:45, the data inside may not correspond to this time) 

Second approach:
When a previously connected ID connects again (a cycle), write to binary file
so server checks id if it has seen it before
*/

void flushBuffer() {
    time_t now = time(nullptr);
    for (auto it = logBuffer.begin(); it != logBuffer.end(); ) {
        const std::string &key = it->first;

        time_t lastSeen = lastWindowTime[key];

        if (difftime(now, lastSeen) >= TIMEOUT && it->second.size() >= 1) {
            // CONVERT TO BINARY AND DELETE FROM HASHMAP
        } else {
            ++it;
        }
    }
}

