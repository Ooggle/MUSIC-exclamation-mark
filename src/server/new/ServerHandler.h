#ifndef SERVERHANDLER_H
#define SERVERHANDLER_H

#include <iostream>
#include <string>
#include <unistd.h>
#include <mutex>
#include <vector>

#include "platform.h"

#if defined(WINDOWS)
#include "mingw/mingw.thread.h"
#elif defined(POSIX)
#include <thread>
#endif

#include "ThreadedTcpServer.h"
#include "SocketCommunication.h"

class ServerHandler
{
    private:
        ThreadedTcpServer *tcpServer;
        
        std::vector<std::thread> threads;

    public:
        ServerHandler(std::string address, uint16_t port, int32_t nbPlaces);

        ~ServerHandler();

        void handlerLoop();

        // Main client handler
        void clientHandler(int32_t sockClient);

};

#endif
