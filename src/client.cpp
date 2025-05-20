#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <cstdlib>
#include <interrupt.hpp>
#include <iostream>
#include <nlohmann/json.hpp>
#include <database.hpp>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "27015"

using json = nlohmann::json;

std::string retrieveHostname() {
    std::string str;
    char hostname[256];
    if (gethostname(hostname, sizeof(hostname)) == 0) {
        str = hostname;
        str.shrink_to_fit();
        return str;
    }
    return NULL;
}

int main(int argc, char **argv) 
{
    signal(SIGINT, signal_handler);
    srand((unsigned) time(NULL));

    json j;
    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    int iResult;

    // Validate the parameters
    if (argc != 2) {
        std::cout << "usage: " << argv[0] << " server-name\n";
        return 1;
    }

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    std::string hostname = retrieveHostname();

    if (hostname.empty()) {
        std::cerr << "Error getting hostname" << std::endl;
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    
    // Resolve the server address and port
    iResult = getaddrinfo(argv[1], DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    while (!INTERRUPT_STATUS) {
        // Attempt to connect to an address until one succeeds
        for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {

            // Create a SOCKET for connecting to server
            ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
                ptr->ai_protocol);
            if (ConnectSocket == INVALID_SOCKET) {
                std::cerr << "Ssocket failed with error: " << WSAGetLastError() << std::endl;
                WSACleanup();
                return 1;
            }

            // Connect to server.
            iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
            if (iResult == SOCKET_ERROR) {
                closesocket(ConnectSocket);
                ConnectSocket = INVALID_SOCKET;
                continue;
            }
            break;
        }

        if (ConnectSocket == INVALID_SOCKET) {
            std::cerr << "Unable to connect to server!\n" << std::endl;
            WSACleanup();
            return 1;
        }

        /* TODO: CREATE A BETTER RANDOMIZATION ALGORITHM */
        time_t timestamp;
        time(&timestamp);

        CPU_DATA perfMetricDevice;
        perfMetricDevice.cpu = rand() % 100;
        perfMetricDevice.memory = rand() % 100;
        perfMetricDevice.timestamp = timestamp;

        j["id"] = hostname;
        j["cpu_usage"] = perfMetricDevice.cpu;
        j["memory_usage"] = perfMetricDevice.memory;
        j["timestamp"] = perfMetricDevice.timestamp;

        std::string strData = j.dump();

        std::cout << strData.c_str() << std::endl;

        iResult = send(ConnectSocket, strData.c_str(), (int)strData.size(), 0);

        if (iResult == SOCKET_ERROR) {
            std::cerr << "Send failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ConnectSocket);
            WSACleanup();
            return 1;
        }

        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    freeaddrinfo(result);

    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "shutdown failed with error: " << WSAGetLastError() << std::endl;

        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }
    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}