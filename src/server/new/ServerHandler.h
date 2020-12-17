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

class ServerHandler
{
    private:
        ThreadedTcpServer *tcpServer;
        
        std::vector<std::thread> threads;

    public:
        ServerHandler(std::string address, uint16_t port, int32_t nbPlaces) {
            tcpServer = new ThreadedTcpServer();
            tcpServer->openServer(address, port, nbPlaces);
            printf("Server opened\n");
        }

        ~ServerHandler() {
            tcpServer->closeServer();
            delete tcpServer;
        }

        void handlerLoop() {

            while(1) {
                printf("Wait for client.\n");
                int32_t sockClient = tcpServer->connectAClient();

                std::thread t1 = std::thread(&ServerHandler::clientHandler, this, sockClient);
                t1.detach();
                
            }
        }

        // Main client handler
        void clientHandler(int32_t sockClient) {
            printf("sock Client: %d\n", sockClient);



            printf("Disconnecting client.\n");
            tcpServer->disconnectAClient(sockClient);
        }

};

#endif
