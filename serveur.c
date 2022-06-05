#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
//#include <bits/semaphore.h>
#include <semaphore.h>

// ------------------------- Define
#define PORT 6000
#define MAX_BUFFER 3000
#define MAX_CLIENTS 3
#define NB_ARTICLES 3
#define EXIT "exit"

typedef struct
{
    int idArticles[NB_ARTICLES];
    int stockParArticle[NB_ARTICLES];
    int prixParArticle[NB_ARTICLES];
    int compteurNombreClient;
    int fdSocketCommunication;

} Magasin;

sem_t semaphore;
pthread_mutex_t mutexCompteurClients;

// ------------------------- Prototypes fonctions
void initialiserMagasin(Magasin *magasin);
void *accueillirClient(void *arg);
int getStockParArticle(Magasin *magasin, int idArticle);
int getPrixParArticle(Magasin *magasin, int idArticle);
char *getLibeleParArticle(int idArticle);
//void afficherArticlesEtPrix(void);
//void afficherStockParArticle(int idArticle);
int isIdProduitValide(Magasin *magasin, int idProduit);
int isQuantiteDisponible(Magasin *magasin, int quantitedemandee, int idProduit);
int ouvrirUneSocketAttente(void);
void creerFacture(Magasin *magasin, int idProduit, int quantite, char facture[]);

// ------------------------- Structure MAGASIN ----------------------


// --------------------- ---- -------------------------------------
// --------------------- MAIN -------------------------------------
// --------------------- ---- -------------------------------------
int main(void)
{
    if (sem_init(&semaphore, 0, MAX_CLIENTS) != 0)
    {
        printf("erreur de creation de semaphore\n");
        return EXIT_FAILURE;
    }
    pthread_mutex_init(&mutexCompteurClients, NULL);

    Magasin magasin;
    initialiserMagasin(&magasin);

    int fdSocketAttente;
    int *fdSocketCommunication = &magasin.fdSocketCommunication;
    struct sockaddr_in coordonneesAppelant;

//    pthread_t vendeur;
    pthread_t clients[MAX_CLIENTS];


    fdSocketAttente = ouvrirUneSocketAttente();

    socklen_t tailleCoord = sizeof(coordonneesAppelant);

    while (1)
    {
        if ((*fdSocketCommunication = accept(fdSocketAttente, (struct sockaddr *) &coordonneesAppelant,
                                             &tailleCoord)) == -1)
        {
            printf("erreur de accept");
            exit(EXIT_FAILURE);
        }

        if (pthread_create(&clients[magasin.compteurNombreClient], NULL, accueillirClient, &magasin) != 0)
        {
            printf("erreur de creation de thread vendeur\n");
            return EXIT_FAILURE;
        }
    }
}

// ------------------------- Fonctions Magasin ----------------------


void initialiserMagasin(Magasin *magasin)
{
    for (int i = 0; i < NB_ARTICLES; ++i)
    {
        magasin->idArticles[i] = i + 1;
        magasin->stockParArticle[i] = 10;
        magasin->prixParArticle[i] = 300 + ((i * 10) * 50);
        magasin->compteurNombreClient = 0;
    }
    printf("Magasin initialisé !\n");
}

int getStockParArticle(Magasin *magasin, int idArticle)
{
    int stockArticle = magasin->stockParArticle[idArticle - 1];

    return stockArticle;
}

int getPrixParArticle(Magasin *magasin, int idArticle)
{
    int prixArticle = magasin->prixParArticle[idArticle - 1];
    return prixArticle;

}

char *getLibeleParArticle(int idArticle)
{
    if (idArticle == 1)
    {
        return "Projet en C";
    } else if (idArticle == 2)
    {
        return "Projet en Java";
    } else if (idArticle == 3)
    {
        return "Projet en CSS only (HTML fourni dans un pack additionnel)";
    }
}

//--------------------------------------------------

void *accueillirClient(void *arg)
{
    sem_wait(&semaphore);
    Magasin *magasin = (Magasin *) arg;
    int fdSocketCommunication = magasin->fdSocketCommunication;
    char tampon[MAX_BUFFER];
    int nbRecu;

    pthread_mutex_lock(&mutexCompteurClients);
    magasin->compteurNombreClient += 1;
    pthread_mutex_unlock(&mutexCompteurClients);

    printf("Client connecté\n");
    printf("Envoi du catalogue au client.\n");
    //printf(creerCommande(1));

    //on crée une string du catalogue produit a envoyer au client
    sprintf(tampon, "[%d] %s - %d€\n"
                    "[%d] %s - %d€\n"
                    "[%d] %s - %d€\n",
            1, getLibeleParArticle(1), getPrixParArticle(magasin, 1),
            2, getLibeleParArticle(2), getPrixParArticle(magasin, 2),
            3, getLibeleParArticle(3), getPrixParArticle(magasin, 3));

    //on envoie le catalogue au client
    send(fdSocketCommunication, tampon, strlen(tampon), 0);

    //TRAITEMENT DU PRODUIT DEMANDÉ PAR LUTILISATEUR
    int idProduit = -1;
    do
    {
        //on attend le produit demandé par le client
        nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);

        //réception  de l’id du produit demandé par le client
        if (nbRecu <= 0)
        {
            exit(EXIT_FAILURE);
        }
        tampon[nbRecu] = 0;
        idProduit = atoi(tampon);

        if (isIdProduitValide(magasin, idProduit))
        {
            sprintf(tampon, "[%d] %s - %d€ - Stock : %d\n", idProduit, getLibeleParArticle(idProduit),
                    getPrixParArticle(magasin, idProduit), getStockParArticle(magasin, idProduit));
            send(fdSocketCommunication, tampon, strlen(tampon), 0);
            printf("Infos envoyées au client\n");
        } else
        {
            sprintf(tampon, "%d", -1);
            send(fdSocketCommunication, tampon, strlen(tampon), 0);
            printf("Message erreur envoyé au client\n");
        }
    } while (isIdProduitValide(magasin, idProduit) != 1);



    //TRAITEMENT DE LA QUANTITE DEMANDÉE PAR L'UTILISATEUR
    int quantiteDemandee = -1;
    do
    {
        // on attend la quantité demandée par le client
        nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);

        //réception  de la quantité demandée par le client
        if (nbRecu <= 0)
        {
            exit(EXIT_FAILURE);
        }

        tampon[nbRecu] = 0;
        quantiteDemandee = atoi(tampon);

        if (isQuantiteDisponible(magasin, quantiteDemandee, idProduit))
        {
            printf("Reçu : %s\n", tampon);

            char facture[MAX_BUFFER];

            creerFacture(magasin, idProduit, quantiteDemandee, facture);
            send(fdSocketCommunication, facture, strlen(facture), 0);
            magasin->stockParArticle[idProduit - 1] = getStockParArticle(magasin, idProduit) - quantiteDemandee;
            printf("Quantite de %s restante : %d\n", getLibeleParArticle(idProduit),
                   magasin->stockParArticle[idProduit - 1]);

        } else
        {
            sprintf(tampon, "%d", -1);
            send(fdSocketCommunication, tampon, strlen(tampon), 0);
            printf("Message erreur envoyé au client\n");
        }
    } while (isQuantiteDisponible(magasin, quantiteDemandee, idProduit) != 1);

    pthread_mutex_lock(&mutexCompteurClients);
    magasin->compteurNombreClient -= 1;
    pthread_mutex_unlock(&mutexCompteurClients);

    sem_post(&semaphore);
    pthread_exit(NULL);
}


int isIdProduitValide(Magasin *magasin, int idProduit)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (idProduit == magasin->idArticles[i])
        {
            return 1;
        }
    }
    return 0;
}

int isQuantiteDisponible(Magasin *magasin, int quantitedemandee, int idProduit)
{
    if (getStockParArticle(magasin, idProduit) < quantitedemandee || quantitedemandee == 0)
    {
        return 0;
    }
    return 1;
}

void creerFacture(Magasin *magasin, int idProduit, int quantite, char facture[])
{
    int prixTotal = getPrixParArticle(magasin, idProduit) * quantite;
    sprintf(facture, "Facture client\n"
                     "- Article choisi : %s \n"
                     "- Quantite achetée : %d - Prix unitaire : %d€ \n"
                     "- Prix Total : %d€\n",
            getLibeleParArticle(idProduit), quantite, getPrixParArticle(magasin, idProduit),
            prixTotal);
    facture[strlen(facture)] = 0;
}

// ------------------------- TCP ----------------------------------
int ouvrirUneSocketAttente(void)
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

    if (bind(socketTemp, (struct sockaddr *) &coordonneesServeur, sizeof(coordonneesServeur)) == -1)
    {
        printf("erreur de bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(socketTemp, MAX_CLIENTS) == -1)
    {
        printf("erreur de listen\n");
        exit(EXIT_FAILURE);
    }

    printf("En attente de connexion...\n");

    return socketTemp;
}

