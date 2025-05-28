cmake_minimum_required(VERSION 3.5)
project(CPUChecker)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json
  GIT_TAG v3.12.0
)

FetchContent_MakeAvailable(json)

include_directories(${PROJECT_SOURCE_DIR}/include)


# Client executable
add_executable(ClientSide src/client.cpp src/interrupt.cpp src/database.cpp src/cpuLoad.cpp)
target_include_directories(ClientSide PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ClientSide PRIVATE nlohmann_json::nlohmann_json ws2_32)

# Server executable
add_executable(ServerSide src/server.cpp src/interrupt.cpp 
  src/database.cpp src/binaryConvert.cpp src/bufferData.cpp src/validation.cpp)
target_include_directories(ServerSide PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ServerSide PRIVATE nlohmann_json::nlohmann_json ws2_32)

# Performance Analysis executable
add_executable(PerformanceAnalysis src/interrupt.cpp src/database.cpp src/binaryConvert.cpp 
src/bufferData.cpp src/searchFiles.cpp src/sortData.cpp)
target_include_directories(PerformanceAnalysis PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(PerformanceAnalysis PRIVATE nlohmann_json::nlohmann_json ws2_32)