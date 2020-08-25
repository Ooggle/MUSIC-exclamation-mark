#ifndef _CLIENTTCP_H
#define _CLIENTTCP_H

#include <cstdint>
#include <string>

//Client TCP
class ClientTcp {
  private:
    //socket de communication avec le serveur.
    uint32_t socketDialogue;


  public:
	ClientTcp();
	~ClientTcp();
  
    //Etablit la connexion avec le serveur d'adresse "adresse" sur le port "port".
    void connecter(std::string adresse, uint16_t port);

    //Se déconnecte du serveur.
    void clore();

    //Emmet au serveur connecté les "nbOctets" premier octets du tableau "data".
    void emettreData(void* data, uint32_t nbOctets);

    //Receptionne depuis le serveur "nbOctets" octets et stocke ces octets dans le tableau "data".
    void recevoirData(void* data, uint32_t nbOctets);

};
#endif
