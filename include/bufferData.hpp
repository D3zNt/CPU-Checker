#ifndef BUFFERDATA_HPP
#define BUFFERDATA_HPP

#include <vector>
#include <unordered_map>
#include <string>
#include <iostream>
#include <chrono>
#include <ctime> 
#include <database.hpp>
#include <fileHandling.hpp>

void addLogEntry(CPU_DATA &data);
void flushBuffer();

#endif

