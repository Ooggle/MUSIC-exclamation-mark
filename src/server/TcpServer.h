#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <cstdint>
#include <string>

// TCP server Class, mono client.
class TcpServer {
  public:
	TcpServer();
	~TcpServer();
  
    // Open a TCP server on "address" with the port "port". And with a waiting room of "nbPlaces" places.
    void openServer(std::string address, uint16_t port, int32_t nbPlaces = 10);


  private:
    // Socket used to wait for client.
    int32_t socketWaitClient;


  public:
    // Close the TCP server. Close connection with client if necessary.
    void closeServer();


  private:
    int32_t socketDialogueClient;


  public:
    // Get data from connected client. Get a max of "dataMaxSize" bytes. The getted bytes are stored in "data". Size read is returned.
    ssize_t getData(void* data, uint32_t dataMaxSize);

    // Get data from connected client. Get a max of "dataMaxSize" bytes. The getted bytes are stored in "data". Size read is returned. Block until data are read.
    ssize_t getDataBlocking(void* data, uint32_t dataMaxSize);

    // Send "nbBytes" first bytes of the "data" array to the connected client.
    // return 0 if good, -1 if client disconnected.
    int sendData(void* data, uint32_t nbBytes);

    // Accept a client. Blocking method.
    void connectAClient();

    // Disconnect the connected client.
    void disconnectAClient();

};
#endif
