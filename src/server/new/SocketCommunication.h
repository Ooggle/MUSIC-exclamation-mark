#ifndef SOCKETCOMMUNICATION_H
#define SOCKETCOMMUNICATION_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "platform.h"

#if defined(WINDOWS)
// Windows headers
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include <BaseTsd.h>
#define ssize_t SSIZE_T
// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#elif defined(POSIX)
// Linux header
#include <arpa/inet.h>

#endif

class SocketCommunication
{
    private:
        int32_t socket;

    public:
        SocketCommunication(int32_t socketClient);

        // Get data from connected client. Get a max of "dataMaxSize" bytes. The getted bytes are stored in "data". Size read is returned.
        ssize_t getData(void* data, uint32_t dataMaxSize);

        // Get data from connected client. Get a max of "dataMaxSize" bytes. The getted bytes are stored in "data". Size read is returned. Block until data are read .
        ssize_t getDataBlocking(void* data, uint32_t dataMaxSize);

        // Send "nbBytes" first bytes of the "data" array to the connected client.
        // return 0 if good, -1 if client disconnected.
        int sendData(void* data, uint32_t nbBytes);

};

#endif
