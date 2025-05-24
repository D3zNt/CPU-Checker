#include <iostream>
#include <thread> 
#include <chrono>
#include <cpuLoad.hpp> 
// https://learn.microsoft.com/en-us/windows/win32/perfctrs/performance-counters-portal

float getMemoryUsage() {
    _MEMORYSTATUSEX lpBuffer;
    lpBuffer.dwLength = sizeof(lpBuffer);

    if(GlobalMemoryStatusEx(&lpBuffer)) {
        DWORDLONG totalPhys = lpBuffer.ullTotalPhys;
        DWORDLONG availablePhys = lpBuffer.ullAvailPhys;
        DWORDLONG usedPhys = totalPhys - availablePhys;

        float memUsage = static_cast<float> (usedPhys) / totalPhys;

        return memUsage * 100;
    }
    return -1.0f;
}

unsigned long long convertFiletime(FILETIME &filetime) {
    ULARGE_INTEGER num;
    num.LowPart = filetime.dwLowDateTime;
    num.HighPart = filetime.dwHighDateTime;

    return num.QuadPart;
}

float getCPUUsage() {
    FILETIME idleTime, kernelTime, userTime;
    FILETIME idleTimeTwo, kernelTimeTwo, userTimeTwo;
    
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    std::this_thread::sleep_for(std::chrono::seconds(1));
    GetSystemTimes(&idleTimeTwo, &kernelTimeTwo, &userTimeTwo);
    
    unsigned long long idle = convertFiletime(idleTimeTwo) - convertFiletime(idleTime);
    unsigned long long kernel = convertFiletime(kernelTimeTwo) - convertFiletime(kernelTime);
    unsigned long long user = convertFiletime(userTimeTwo) - convertFiletime(userTime);

    unsigned long long systemTime = (kernel + user) - idle;
    unsigned long long totalTime = kernel + user;

    float cpuUsage = static_cast<float>(systemTime) / static_cast<float>(totalTime);

    return cpuUsage * 100;
}