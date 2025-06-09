# CPU-Checker

CPU Checker is a multi-client server application that centralizes CPU performance monitoring and analysis.

## Prerequisites

Before building and running CPU Checker, ensure the following software is installed:

- **CMake** (version 3.10 or higher)
- **C++ compiler** (supporting C++20 or later)
  - _Windows_: MSVC (Visual Studio 2019+) or MinGW-w64
- **Git** (to clone the repository)

## Building the Project

### Windows (MinGW)

```bash
git clone https://github.com/D3zNt/CPU-Checker.git
cd CPU-Checker
mkdir build && cd build
cmake -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --config Release
```

### Windows (Visual Studio)

```bash
git clone https://github.com/D3zNt/CPU-Checker.git
cd CPU-Checker
mkdir build && cd build
cmake -G "Visual Studio 17 2022" ..
cmake --build . --config Release
```

## Installation

After building, you can install to a local directory:

```bash
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
