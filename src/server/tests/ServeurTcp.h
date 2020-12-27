#ifndef _SERVEURTCP_H
#define _SERVEURTCP_H

#include <cstdint>
#include <string>

//Classe serveur TCP mono client.
class ServeurTcp {
  public:
	ServeurTcp();
	~ServeurTcp();
  
    //Ouvre le serveur TCP sur l'interface aillant pour adresse ou non "adresse" avec comme port "port". Le nombre de place pour la file d'attente est "nbPlaces".
    void ouvrir(std::string adresse, uint16_t port, int32_t nbPlaces = 10);


  private:
    //Socket permettant d'attendre des clients TCP.
    int32_t socketAttenteClient;


  public:
    //Ferme le serveur TCP. Coupe si necessaire la communication en cours avec un client.
    void fermer();


  private:
    int32_t socketDialogueClient;


  public:
    //Recoit des données du client connecté. Receptionne "tailleData" octets. Les octets recus sont stocké dans le tableau "data".
    void recevoirData(void* data, uint32_t tailleDataMax);

    //Emet au client connecté les "nbOctets" premiers octets du tableau "data".
    void emettreData(void* data, uint32_t nbOctets);

    //Accepte un client. Méthode bloquante.
    void connecterUnClient();

    //Déconnecte le client connecté.
    void deconnecterUnClient();

};
#endif
