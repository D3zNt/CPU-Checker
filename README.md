# CPU-Checker

CPU Checker is a multi-client server application that centralizes CPU performance monitoring and analysis.

## Prerequisites
Before building and running CPU Checker, ensure the following software is installed on your machine:

- **CMake** (version 3.5 or higher)  
  CMake is required to configure and build the project.
- **A C++ compiler supporting C++20**  
  Examples:  
  - MSVC (Visual Studio 2019 or later) on Windows  
  - GCC 10+ or Clang 10+ on Linux/macOS
- **Git** (to clone the repository)  

## Installation
Clone the repository and build the project using CMake:

```bash
git clone https://github.com/D3zNt/CPU-Checker.git
cd CPU-Checker
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build . --config Release
cmake --install . --prefix ./install
```

## Usage
After installation, navigate to the installed binaries directory:
```bash
cd install/bin
```

### Run the server-side application
Listens for client connections and logs received data to a binary file every 1 minute.
```bash
./ServerSide
```
### Run the client-side application
Collects machine performance data and sends it to the server every 5 seconds.
```bash
./ClientSide [PASS IPADDRESS or localhost as the second argument]
```
### Run the performance analysis tool
Reads the specified binary log files, analyzes the retrieved data, and exports the results to a JSON file.
```bash
./PerformanceAnalysis
```