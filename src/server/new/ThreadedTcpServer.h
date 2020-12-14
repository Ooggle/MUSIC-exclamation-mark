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
#include <unistd.h>
#include <string.h>
#include <cstdint>
#include <string>
#include <vector>
#include <thread>

#include "platform.h"

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

    // Get data from connected client. Get a max of "dataMaxSize" bytes. The getted bytes are stored in "data". Size read is returned.
    ssize_t getData(int32_t socket, void* data, uint32_t dataMaxSize);

    // Get data from connected client. Get a max of "dataMaxSize" bytes. The getted bytes are stored in "data". Size read is returned. Block until data are read .
    ssize_t getDataBlocking(int32_t socket, void* data, uint32_t dataMaxSize);

    // Send "nbBytes" first bytes of the "data" array to the connected client.
    // return 0 if good, -1 if client disconnected.
    int sendData(int32_t socket, void* data, uint32_t nbBytes);

    // Accept a client. Blocking method.
    int32_t connectAClient();

    // Disconnect the connected client.
    void disconnectAClient(int32_t socket);

};
#endif
