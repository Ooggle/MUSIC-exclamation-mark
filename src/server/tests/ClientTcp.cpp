
#include "ClientTcp.h"

#include <sys/types.h>   
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

ClientTcp::ClientTcp():socketDialogue(-1){
}
ClientTcp::~ClientTcp(){
	this->clore();
}

//Etablit la connexion avec le serveur d'adresse "adresse" sur le port "port".
void ClientTcp::connecter(std::string adresse, uint16_t port) {
	struct sockaddr_in adresseServeur ;
	this->socketDialogue=socket (AF_INET , SOCK_STREAM , 0) ;
	//Mettre a zero l'adresse du serveur
	bzero((char *) &adresseServeur, sizeof(struct sockaddr_in));
	//Initialiser l'adresse du serveur
	adresseServeur.sin_family = AF_INET;
	adresseServeur.sin_port = htons(port); 
	inet_aton (adresse.c_str() , &adresseServeur.sin_addr);
	//Se connecter au serveur
	connect (this->socketDialogue , (struct sockaddr *) &adresseServeur , sizeof(struct sockaddr_in));	
}

//Se déconnecte du serveur.
void ClientTcp::clore() {
	close(this->socketDialogue);
}

//Emmet au serveur connecté les "nbOctets" premier octets du tableau "data".
void ClientTcp::emettreData(void* data, uint32_t nbOctets) {
	send(this->socketDialogue,data,nbOctets,0);
}

//Receptionne depuis le serveur "nbOctets" octets et stocke ces octets dans le tableau "data".
void ClientTcp::recevoirData(void* data, uint32_t nbOctets) {
	recv (this->socketDialogue , data , nbOctets , MSG_WAITALL); 
}

#ifdef ClientTcpUT
#ifndef NDEBUG
#include <cassert>
// g++ -o ClientTcpUT ClientTcp.cpp
int main(void)
{
	ClientTcp clientTcp;
	clientTcp.connecter(std::string("127.0.0.1"),55555);
	clientTcp.emettreData((void*)"A que coucou\n",13);
	char msg[256]={0};
	clientTcp.recevoirData(msg,12);
	clientTcp.clore();
	msg[12]=0;
	assert(strcmp(msg,"A que coucou")==0);
}
#endif
#endif

