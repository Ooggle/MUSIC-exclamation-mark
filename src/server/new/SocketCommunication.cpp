
#include "SocketCommunication.h"

SocketCommunication::SocketCommunication(int32_t socketClient)
{
    this->socket = socketClient;
}

ssize_t SocketCommunication::getData(void* data, uint32_t dataMaxSize)
{
    #if defined(WINDOWS)
    return recv(this->socket, (char *) data, dataMaxSize, 0);
    #elif defined(POSIX)
    return recv(this->socket, data, dataMaxSize, MSG_DONTWAIT);
    #endif
}

ssize_t SocketCommunication::getDataBlocking(void* data, uint32_t dataMaxSize)
{
    #if defined(WINDOWS)
    return recv(this->socket, (char *) data, dataMaxSize, MSG_WAITALL);
    #elif defined(POSIX)
    return recv(this->socket, data, dataMaxSize, MSG_WAITALL);
    #endif
}

int SocketCommunication::sendData(void* data, uint32_t nbBytes)
{
    int returnCode = 0;

    #if defined(WINDOWS)
    returnCode = send(this->socket, (const char *) data, nbBytes, 0);
    #elif defined(POSIX)
    returnCode = send(this->socket, data, nbBytes, MSG_NOSIGNAL);
    #endif

    if(returnCode == -1) {
        return -1;
    }
    return 0;
}
