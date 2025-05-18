#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <string>
#include <nlohmann/json.hpp>
#include <interrupt.hpp>

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LEN 512

using json = nlohmann::json;

int handleClientRequest(SOCKET ClientSocket) {
    int iResult, iSendResult;
    char buffer[DEFAULT_BUFFER_LEN];
    iResult = recv(ClientSocket, buffer, DEFAULT_BUFFER_LEN, 0);

    if (iResult > 0) {
        std::string data = buffer;
        data.resize(iResult);

        std::cout << data << std::endl;
        /* TODO: PROCESS THE DATA */

        iSendResult = send(ClientSocket, "Performance metrics sucessfully received.\n", iResult, 0);
        if (iSendResult == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
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
    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
    
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

    // Accept a client socket
    while (!INTERRUPT_STATUS) {
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            std::cerr << "accept failed with error: " << WSAGetLastError() << std::endl;
            closesocket(ListenSocket);
            WSACleanup();
            return 1;
        }

        std::thread thr(handleClientRequest, ClientSocket);
        thr.detach();
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }

    // cleanup
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}