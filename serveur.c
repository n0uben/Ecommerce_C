#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#define PORT 6000
#define MAX_BUFFER 3000

int ouvrirUneSocketAttente();
static void decode_commande (uint16_t *recv_data, size_t recv_len);

int main(int argc, char const *argv[])
{
    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordonneesAppelant;
    char tampon[MAX_BUFFER];
    int nbRecu;

    fdSocketAttente = ouvrirUneSocketAttente();

    socklen_t tailleCoord = sizeof(coordonneesAppelant);

    //on accepte la communication
    fdSocketCommunication = accept(fdSocketAttente, (struct sockaddr *) &coordonneesAppelant, &tailleCoord);
    if (fdSocketCommunication == -1)
    {
        printf("erreur de accept\n");
        exit(EXIT_FAILURE);
    }

    printf("Client connecté\n");

    // on attend le message du client
    // la fonction recv est bloquante
    nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);

    if (nbRecu > 0)
    {
        tampon[nbRecu] = 0;
        printf("Recu : %d\n", atoi(tampon));
    }

    printf("Envoi du message au client.\n");


    strcpy(tampon, "Message renvoyé par le serveur vers le client !");
    // on envoie le message au client
    send(fdSocketCommunication, tampon, strlen(tampon), 0);

    close(fdSocketCommunication);
    close(fdSocketAttente);

    return EXIT_SUCCESS;
}


int ouvrirUneSocketAttente()
{
    int socketTemp;
    int longueurAdresse;
    struct sockaddr_in coordonneesServeur;

    socketTemp = socket(PF_INET, SOCK_STREAM, 0);

    if (socketTemp < 0)
    {
        printf("socket incorrecte\n");
        exit(EXIT_FAILURE);
    }

    // On prépare l’adresse d’attachement locale
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);

    // connexion de type TCP
    coordonneesServeur.sin_family = PF_INET;
    // toutes les interfaces locales disponibles
    coordonneesServeur.sin_addr.s_addr = htonl(INADDR_ANY);
    // le port d'écoute
    coordonneesServeur.sin_port = htons(PORT);

    //on initialise la socket sur la carte réseau
    int socketBind = bind(socketTemp, (struct sockaddr *) &coordonneesServeur, sizeof(coordonneesServeur));
    if (socketBind == -1)
    {
        printf("erreur de bind\n");
        exit(EXIT_FAILURE);
    }

    //on lance l'ecoute sur la socket
    int socketListen = listen(socketTemp, 5);
    if (socketListen == -1)
    {
        printf("erreur de listen\n");
        exit(EXIT_FAILURE);
    }

    printf("En attente de connexion...\n");

    return socketTemp;
}