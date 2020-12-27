#ifndef THREADEDTCPSERVER_H
#define THREADEDTCPSERVER_H

//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <sys/stat.h>
//#include <fcntl.h>

// Global headers
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <string>
#include <vector>
#include <thread>

#include "platform.h"
#include "SocketCommunication.h"

#if defined(WINDOWS)
// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#elif defined(POSIX)
// Linux header
#include <arpa/inet.h>

#endif

// TCP server Class, mono client.
class ThreadedTcpServer {
    private:
        // Socket used to wait for client.
        int32_t socketWaitClient;
        int maxClient;

    public:
        ThreadedTcpServer();
        ~ThreadedTcpServer();
        
        // Open a TCP server on "address" with the port "port". And with a waiting room of "nbPlaces" places.
        void openServer(std::string address, uint16_t port, int32_t nbPlaces = 1000);
        
        // Close the TCP server. Close connection with client if necessary.
        void closeServer();

        // Accept a client. Blocking method.
        int32_t connectAClient();

        // Disconnect the connected client.
        void disconnectAClient(int32_t socket);

};

#endif
