
#include "ServeurTcp.h"

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


ServeurTcp::ServeurTcp():socketAttenteClient(-1),socketDialogueClient(-1){
}

ServeurTcp::~ServeurTcp(){
	this->fermer();
}

//Ouvre le serveur TCP sur l'interface aillant pour adresse ou non "adresse" avec comme port "port". Le nombre de place pour la file d'attente est "nbPlaces".
void ServeurTcp::ouvrir(std::string adresse, uint16_t port, int32_t nbPlaces) {
	int ctrl;
	struct sockaddr_in adresseServeur;

	//Créer de l'interface réseau
	this->socketAttenteClient = socket (AF_INET , SOCK_STREAM , 0) ;
	
	//Mettre à zéro la structure adresseServeur
	bzero((char *) &adresseServeur, sizeof(struct sockaddr_in));
	
	//Initialiser la structure adresseServeur
	adresseServeur.sin_family = AF_INET;
	adresseServeur.sin_port = htons(port); //On passe le numero de port en reseau
	adresseServeur.sin_addr.s_addr = INADDR_ANY;
	inet_aton (adresse.c_str() , &adresseServeur.sin_addr); //On passe l'adresse en réseau

	//Indiquer a l'os l'adresse et le port d'attente de client du programme
	bind (this->socketAttenteClient , (struct sockaddr *) &adresseServeur , sizeof(struct sockaddr_in));

	//Permettre la mise en attente par l'OS d'un seul client
	listen (this->socketAttenteClient , nbPlaces) ;
}

//Ferme le serveur TCP. Coupe si necessaire la communication en cours avec un client.
void ServeurTcp::fermer() {
	this->deconnecterUnClient();
	shutdown(this->socketAttenteClient,SHUT_RDWR);
	close(this->socketAttenteClient);
	this->socketAttenteClient=-1;
}

//Recoit des données du client connecté. Receptionne "tailleData" octets. Les octets recus sont stocké dans le tableau "data".
void ServeurTcp::recevoirData(void* data, uint32_t tailleDataMax) {
	recv(this->socketDialogueClient,data,tailleDataMax,MSG_WAITALL);
}

//Emet au client connecté les "nbOctets" premiers octets du tableau "data".
void ServeurTcp::emettreData(void* data, uint32_t nbOctets) {
	send(this->socketDialogueClient,data,nbOctets,0);
}

//Accepte un client. Méthode bloquante.
void ServeurTcp::connecterUnClient() {
	socklen_t tailleAdresseClient;
	struct sockaddr_in adresseClient;
	tailleAdresseClient = sizeof(struct sockaddr_in);
	bzero((char *) &adresseClient, sizeof(struct sockaddr_in));
	this->socketDialogueClient = accept(this->socketAttenteClient , (struct sockaddr *) &adresseClient , &tailleAdresseClient);	
}

//Déconnecte le client connecté.
void ServeurTcp::deconnecterUnClient() {
	shutdown(this->socketDialogueClient,SHUT_RDWR);
	close(this->socketDialogueClient);
	this->socketDialogueClient=-1;
}

#ifdef ServeurTcpUT
#ifndef NDEBUG
#include <cassert>
// g++ -o ServeurTcpUT ServeurTcp.cpp -DServeurTcpUT
int main(void) {
	ServeurTcp serveurTcp;
	char msg[256]={0};
	uint32_t nbRecus;
	serveurTcp.ouvrir(std::string("127.0.0.1"),55554,20);
	serveurTcp.connecterUnClient();
	serveurTcp.emettreData((void*)"A que coucou\n",13);
	serveurTcp.recevoirData(msg,13);
	serveurTcp.deconnecterUnClient();
	serveurTcp.fermer();
	assert(strcmp(msg,"A que coucou\n")==0); // nc envoie le retour chariot
	return 0;
}
#endif
#endif

