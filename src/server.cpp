#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <iostream>
#include <thread>
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include <interrupt.hpp>
#include <database.hpp>
#include <mutex>

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFFER_LEN 512

using json = nlohmann::json;

std::mutex dataMutex;

/*
USE MERGE SORT/QUICK SORT COMBINED WITH OPENMP
*/
void sortData() {
    /* DO THE SORTING HERE */
    std::lock_guard<std::mutex> recordLock(dataMutex);    
    int count = 0;

    /* RESULT */
    for (auto &entry : DATA_RECORDS) {
        std::cout << 
        "ELEMENT: " << count++ << "\n" << 
        "ID: " << entry.id << "\n" <<
        "CPU USAGE (%): " << entry.cpu <<  "\n" 
        << "MEMORY USAGE (%): " << entry.memory << "\n\n";
    }
}

/* NOTE:
CHECK THE CONTENT OF THE BIN FILE IN BUILD/DEBUG FOLDER
*/
void convertToBSON(json JSONValue) {

}
void searchData() {

}

// void convertToJSON() {

// }

int validateJSON(json JSON) {
    if (!JSON.contains("id") || !JSON.contains("cpu_usage") || !JSON.contains("memory_usage") || !JSON.contains("timestamp")) {
        std::cerr << "Missing required fields\n";
        return -1;
    }

    if (!JSON["cpu_usage"].is_number() || !JSON["memory_usage"].is_number()) {
        std::cerr << "Invalid field types\n";
        return -1;
    }

    if ((JSON["cpu_usage"] < 0 || JSON["cpu_usage"] > 100) || 
        (JSON["memory_usage"] < 0 || JSON["memory_usage"] > 100)) {
        std::cerr << "Data out of range\n";
        return -1;
    }
    return 0;
}

int handleClientRequest(SOCKET ClientSocket) {
    int iResult, iSendResult;
    char buffer[DEFAULT_BUFFER_LEN];
    iResult = recv(ClientSocket, buffer, DEFAULT_BUFFER_LEN, 0);

    if (iResult > 0) {
        /* VALIDATE CLIENT'S REQUEST */

        std::string data = buffer;
        data.resize(iResult);

        json JSONValue = json::parse(data);

        if (validateJSON(JSONValue) < 0) {
            closesocket(ClientSocket);
            WSACleanup();
            return 1;        
        }

        CPU_DATA machinePerformance = {JSONValue["id"], JSONValue["cpu_usage"], JSONValue["memory_usage"]};    
        
        {
            std::lock_guard<std::mutex> recordlock(dataMutex);
            DATA_RECORDS.emplace_back(machinePerformance);
        }

        /* TODO: PROCESS THE DATA
        IMPORTANT: (THE DATA IS ALREADY IN A VECTOR CALLED DATA_RECORDS GLOBALLY DEFINED IN DATABASE.CPP) 
        */

        // convert to BSON
        std::thread threadToStoreBinary(convertToBSON, JSONValue);
        threadToStoreBinary.detach();

        // search

        // sorting
        std::thread threadToSort(sortData);
        threadToSort.detach();

        // convert to JSON

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

            std::thread thr(handleClientRequest, ClientSocket);
            thr.detach();
        }    
    }

    // cleanup
    closesocket(ListenSocket);
    WSACleanup();

    return 0;
}
