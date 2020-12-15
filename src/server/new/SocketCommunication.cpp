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
        SocketCommunication(int32_t socketClient) {
            this->socket = socketClient;
        }

        ssize_t getData(int32_t socket, void* data, uint32_t dataMaxSize)
        {
            #if defined(WINDOWS)
            return recv(socket, (char *) data, dataMaxSize, 0);
            #elif defined(POSIX)
            return recv(socket, data, dataMaxSize, MSG_DONTWAIT);
            #endif
        }

        ssize_t getDataBlocking(int32_t socket, void* data, uint32_t dataMaxSize)
        {
            #if defined(WINDOWS)
            return recv(socket, (char *) data, dataMaxSize, MSG_WAITALL);
            #elif defined(POSIX)
            return recv(socket, data, dataMaxSize, MSG_WAITALL);
            #endif
        }

        int sendData(int32_t socket, void* data, uint32_t nbBytes)
        {
            int returnCode = 0;

            #if defined(WINDOWS)
            returnCode = send(socket, (const char *) data, nbBytes, 0);
            #elif defined(POSIX)
            returnCode = send(socket, data, nbBytes, MSG_NOSIGNAL);
            #endif

            if(returnCode == -1) {
                return -1;
            }
            return 0;
        }

};
