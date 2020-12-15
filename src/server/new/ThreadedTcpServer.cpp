#include "ThreadedTcpServer.h"

ThreadedTcpServer::ThreadedTcpServer():socketWaitClient(-1)
{}

ThreadedTcpServer::~ThreadedTcpServer()
{
	this->closeServer();
}

void ThreadedTcpServer::openServer(std::string address, uint16_t port, int32_t nbPlaces)
{
	#if defined(WINDOWS)
	WSADATA wsaData;

	struct addrinfo *result = NULL;
    struct addrinfo hints;
	const char * portNew = std::to_string(port).c_str();

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    //hints.ai_flags = AI_PASSIVE;

	// Initialize Winsock
    WSAStartup(MAKEWORD(2,2), &wsaData);

	// Resolve the server address and port
	getaddrinfo(NULL, portNew, &hints, &result);

	// Create a SOCKET for connecting to server
    this->socketWaitClient = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
	
	// Setup the TCP listening socket
	bind(this->socketWaitClient, result->ai_addr, (int)result->ai_addrlen);
	freeaddrinfo(result);

	// Setup non-blocking socket
	u_long iMode=1;
	ioctlsocket(this->socketWaitClient,FIONBIO,&iMode);
	
	#elif defined(POSIX)
	struct sockaddr_in serverAdress;

	// Create network interface
	this->socketWaitClient = socket(AF_INET , SOCK_STREAM , IPPROTO_TCP);
	
	// Set the serverAdress structure to zero
	#if defined(POSIX)
	bzero((char *)&serverAdress, sizeof(struct sockaddr_in));
	#endif
	
	// Init the serverAdress structure
	serverAdress.sin_family = AF_INET;
	serverAdress.sin_port = htons(port); // Htons the port number 
	serverAdress.sin_addr.s_addr = INADDR_ANY;
	#if defined(POSIX)
	inet_aton(address.c_str(), &serverAdress.sin_addr); // Htons to address
	#endif

	// Tell to the OS the waiting port of the program
	unsigned long _enable = 1;
    setsockopt(this->socketWaitClient, SOL_SOCKET, SO_REUSEADDR, (const char*)&_enable,sizeof(unsigned long));
	bind(this->socketWaitClient, (struct sockaddr *)&serverAdress, sizeof(struct sockaddr_in));

	// timeout (1s)
	/* struct timeval tv;
	tv.tv_sec = 1;
	tv.tv_usec = 0;
	setsockopt(this->socketWaitClient, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv); */

	#endif

	// Waiting for one client
	listen(this->socketWaitClient, nbPlaces);
}

void ThreadedTcpServer::closeServer()
{
	#if defined(WINDOWS)
	shutdown(this->socketWaitClient, SD_BOTH);
	closesocket(this->socketWaitClient);
	WSACleanup();
	#elif defined(POSIX)
	shutdown(this->socketWaitClient, SHUT_RDWR);
	close(this->socketWaitClient);
	#endif
	this->socketWaitClient=-1;
}

int32_t ThreadedTcpServer::connectAClient()
{
	int32_t socketClient = -1;
	socklen_t tailleAdresseClient;
	struct sockaddr_in adresseClient;
	tailleAdresseClient = sizeof(struct sockaddr_in);
	memset((char *) &adresseClient, 0, sizeof(struct sockaddr_in));
	do {
		socketClient = accept(this->socketWaitClient, (struct sockaddr *) &adresseClient, &tailleAdresseClient);
	} while(socketClient == -1);

	return socketClient;
}

void ThreadedTcpServer::disconnectAClient(int32_t socket)
{
	#if defined(WINDOWS)
	shutdown(socket, SD_BOTH);
	#else
	shutdown(socket, SHUT_RDWR);
	#endif
	close(socket);
}
