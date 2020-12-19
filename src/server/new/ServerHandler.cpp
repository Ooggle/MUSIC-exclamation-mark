
#include "ServerHandler.h"

ServerHandler::ServerHandler(std::string address, uint16_t port, int32_t nbPlaces)
{
    tcpServer = new ThreadedTcpServer();
    tcpServer->openServer(address, port, nbPlaces);
    printf("Server opened\n");
}

ServerHandler::~ServerHandler()
{
    tcpServer->closeServer();
    delete tcpServer;
}

void ServerHandler::handlerLoop()
{

    while(1) {
        printf("Wait for client.\n");
        int32_t sockClient = tcpServer->connectAClient();

        std::thread t1 = std::thread(&ServerHandler::clientHandler, this, sockClient);
        t1.detach();
        
    }
}

// Main client handler
void ServerHandler::clientHandler(int32_t sockClient)
{
    printf("sock Client: %d\n", sockClient);

    SocketCommunication* client = new SocketCommunication(sockClient);

    char buff[5] = {0};
    int ret = client->getDataBlocking(&buff, 5);
    printf("bytes collected: %d\n", ret);
    if(ret == -1) {
        printf("recv failed: %d\n", WSAGetLastError());
    }
    printf("data: %s\n", buff);
    client->sendData(buff, 5);

    printf("Disconnecting client.\n");
    tcpServer->disconnectAClient(sockClient);
}


int main()  
{
    int x = 10;
    /* std::string str = "Sample String";
    std::thread threadObj(threadCallback, x, str);
    threadObj.join(); */

    ServerHandler *handler = new ServerHandler("0.0.0.0", 81, 1000);
    printf("Run\n");
    handler->handlerLoop();

    while(1){}
    

    return 0;
}
