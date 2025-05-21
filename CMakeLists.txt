cmake_minimum_required(VERSION 3.10)
project(CPUChecker)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.2
)

FetchContent_MakeAvailable(json)

include_directories(${PROJECT_SOURCE_DIR}/include)


# Client executable
add_executable(ClientSide src/client.cpp src/interrupt.cpp src/database.cpp)
target_include_directories(ClientSide PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ClientSide PRIVATE nlohmann_json::nlohmann_json ws2_32)

# Server executable
add_executable(ServerSide src/server.cpp src/interrupt.cpp src/database.cpp src/binaryConvert.cpp)
target_include_directories(ServerSide PRIVATE ${CMAKE_SOURCE_DIR})
target_link_libraries(ServerSide PRIVATE nlohmann_json::nlohmann_json ws2_32)