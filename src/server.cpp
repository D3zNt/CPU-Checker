#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <database.hpp>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <nlohmann/json.hpp>
#include <interrupt.hpp>
#include <validation.hpp>
#include <bufferData.hpp>

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LEN 512

using json = nlohmann::json;

int handleClientRequest(SOCKET ClientSocket) {
    int iResult, iSendResult;
    char buffer[DEFAULT_BUFFER_LEN];
    iResult = recv(ClientSocket, buffer, DEFAULT_BUFFER_LEN, 0);
    
    if (iResult > 0) { 
        printf("Bytes received: %d\n", iResult);

        std::string data = buffer;
        data.resize(iResult);

        json JSONValue;
        try {
            JSONValue = json::parse(data);
        } catch (const std::exception& e) {
            std::cerr << "JSON parse error: " << e.what() << '\n';
            closesocket(ClientSocket);
            return 1;
        }

        CPU_DATA machinePerformance = {JSONValue["id"], JSONValue["timestamp"], JSONValue["cpu_usage"], JSONValue["memory_usage"]};
        
        {
            std::lock_guard<std::mutex> lock(dataMutex);
            addLogEntry(machinePerformance);
        }
        
    } else if (iResult == 0) {
        std::cout << "Connection closing...\n";
    } else {
        std::cerr << "recv failed: " << WSAGetLastError() << std::endl;
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }
    closesocket(ClientSocket);
    return 0;
}

int main(void) 
{
    signal(SIGINT, signal_handler);

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        std::cerr << "WSAStartup failed with error: " << iResult << std::endl;
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &result);
    if ( iResult != 0 ) {
        std::cerr << "getaddrinfo failed with error: " << iResult << std::endl;
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        std::cerr << "Socket failed with error: " << WSAGetLastError() << std::endl;
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "bind failed with error: " << WSAGetLastError() << std::endl;

        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "listen failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    std::thread([] {
        try {
            while (!INTERRUPT_STATUS) {
                flushBuffer();
                std::this_thread::sleep_for(std::chrono::seconds(60));
            }
        } catch (const std::exception &except) {
            std::cerr << "Worker thread crashed: " << except.what() << '\n';
        }
    }).detach();

    // Accept a client socket
    while (!INTERRUPT_STATUS) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(ListenSocket, &readfds);

        timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int selectResult = select(0, &readfds, NULL, NULL, &timeout);

        if (selectResult == SOCKET_ERROR) {
            std::cerr << "select failed with error: " << WSAGetLastError() << std::endl;
            break;
        }  
        if (selectResult == 0) continue;

        if (FD_ISSET(ListenSocket, &readfds)) {
            ClientSocket = accept(ListenSocket, NULL, NULL);
            if (ClientSocket == INVALID_SOCKET) {
                std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
                closesocket(ListenSocket);
                WSACleanup();
                return 1;
            }

            std::thread([ClientSocket] {
                try{
                    handleClientRequest(ClientSocket);
                } catch (const std::exception &except) {
                    std::cerr << "Worker thread crashed: " << except.what() << '\n';
                }
            }).detach();
        }    
    }

    flushBuffer();

    // cleanup
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}