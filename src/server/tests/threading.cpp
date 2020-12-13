#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>
#include <mutex>
#include <vector>

#include "ThreadedTcpServer.h"

class WebHandler
{
    private:
        ThreadedTcpServer *tcpServer;
        std::vector<std::thread> threads;

    public:

        WebHandler(std::string address, uint16_t port, int32_t nbPlaces) {
            tcpServer = new ThreadedTcpServer();
            tcpServer->openServer(address, port, nbPlaces);
            printf("Server opened\n");
        }

        ~WebHandler() {
            tcpServer->closeServer();
            delete tcpServer;
        }

        void run() {

            while(1) {
                printf("Wait for client.\n");
                int32_t sockClient = tcpServer->connectAClient();
                std::thread t1 = std::thread(&WebHandler::clientHandler, this, sockClient);
                
                t1.detach();
            }
        }

        void clientHandler(int32_t sockClient) {
            printf("sock Client: %d\n", sockClient);
            sleep(3);

            printf("Disconnecting client.\n");
            tcpServer->disconnectAClient(sockClient);
        }

};

int main()  
{
    int x = 10;
    /* std::string str = "Sample String";
    std::thread threadObj(threadCallback, x, str);
    threadObj.join(); */

    WebHandler *handler = new WebHandler("0.0.0.0", 81, 1000);
    printf("Run\n");
    handler->run();

    while(1){}
    

    return 0;
}
