
#include "TcpServer.h"

#include <sys/types.h>   
#include <sys/socket.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <arpa/inet.h>


TcpServer::TcpServer():socketWaitClient(-1),socketDialogueClient(-1){
}

TcpServer::~TcpServer(){
	this->closeServer();
}

void TcpServer::openServer(std::string address, uint16_t port, int32_t nbPlaces) {
	int ctrl;
	struct sockaddr_in serverAdress;

	// Create network interface
	this->socketWaitClient = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP) ;
	
	// Set the serverAdress structure to zero
	bzero((char *)&serverAdress, sizeof(struct sockaddr_in));
	
	// Init the serverAdress structure
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(port); // Htons the port number 
	serverAdress.sin_addr.s_addr = INADDR_ANY;
	inet_aton(address.c_str(), &serverAdress.sin_addr); // Htons to address

	// Tell to the OS the waiting port of the program
	unsigned long _enable = 1;
    setsockopt(this->socketWaitClient, SOL_SOCKET, SO_REUSEADDR, (const char*)&_enable,sizeof(unsigned long));
	bind(this->socketWaitClient, (struct sockaddr *)&serverAdress, sizeof(struct sockaddr_in));

	// timeout (1s)
	/* struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(this->socketWaitClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv); */


	// Waiting for one client
	listen(this->socketWaitClient, nbPlaces);
}

void TcpServer::closeServer() {
	this->disconnectAClient();
	shutdown(this->socketWaitClient,SHUT_RDWR);
	close(this->socketWaitClient);
	this->socketWaitClient=-1;
}

ssize_t TcpServer::getData(void* data, uint32_t dataMaxSize) {
	return recv(this->socketDialogueClient, data, dataMaxSize, MSG_DONTWAIT);
}

ssize_t TcpServer::getDataBlocking(void* data, uint32_t dataMaxSize) {
	return recv(this->socketDialogueClient, data, dataMaxSize, MSG_WAITALL);
}

int TcpServer::sendData(void* data, uint32_t nbBytes) {
	if(send(this->socketDialogueClient, data, nbBytes, MSG_WAITALL) == -1) {
		return -1;
	}
	return 0;
}

void TcpServer::connectAClient() {
	socklen_t tailleAdresseClient;
	struct sockaddr_in adresseClient;
	tailleAdresseClient = sizeof(struct sockaddr_in);
	bzero((char *) &adresseClient, sizeof(struct sockaddr_in));
	do {
		this->socketDialogueClient = accept(this->socketWaitClient , (struct sockaddr *) &adresseClient , &tailleAdresseClient);
	} while(this->socketDialogueClient == -1);
}

void TcpServer::disconnectAClient() {
	shutdown(this->socketDialogueClient,SHUT_RDWR);
	close(this->socketDialogueClient);
	this->socketDialogueClient=-1;
}
