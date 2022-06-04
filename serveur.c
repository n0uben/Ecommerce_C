#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

// ------------------------- Define
#define PORT 6000
#define MAX_BUFFER 3000
#define MAX_CLIENTS 3
#define NB_ARTICLES 3
#define EXIT "exit"


// ------------------------- Prototypes fonctions
void initialiserMagasin(void);

int getStockParArticle(int idArticle);

int getPrixParArticle(int idArticle);

char *getLibeleParArticle(int idArticle);

void afficherArticlesEtPrix(void);

void afficherStockParArticle(int idArticle);

int isIdProduitValide(int idProduit);

int isQuantiteDisponible(int quantitedemandee, int idProduit);

int ouvrirUneSocketAttente(void);

void creerFacture(int idProduit, int quantite, char facture[]);

// ------------------------- Structure MAGASIN ----------------------
struct {
    int idArticles[NB_ARTICLES];
    int stockParArticle[NB_ARTICLES];
    int prixParArticle[NB_ARTICLES];
} Magasin;

// --------------------- ---- -------------------------------------
// --------------------- MAIN -------------------------------------
// --------------------- ---- -------------------------------------
int main(int argc, char const *argv[]) {
    int fdSocketAttente;
    int fdSocketCommunication;
    struct sockaddr_in coordonneesAppelant;
    char tampon[MAX_BUFFER];
    int nbRecu;

    initialiserMagasin();

    fdSocketAttente = ouvrirUneSocketAttente();

    socklen_t tailleCoord = sizeof(coordonneesAppelant);

    if ((fdSocketCommunication = accept(fdSocketAttente, (struct sockaddr *) &coordonneesAppelant,
                                        &tailleCoord)) == -1) {
        printf("erreur de accept\n");
        exit(EXIT_FAILURE);
    }

    printf("Client connecté\n");
    printf("Envoi du catalogue au client.\n");
    //printf(creerCommande(1));

    //on crée une string du catalogue produit a envoyer au client
    sprintf(tampon, "[%d] %s - %d€\n"
                    "[%d] %s - %d€\n"
                    "[%d] %s - %d€\n",
            1, getLibeleParArticle(1), getPrixParArticle(1),
            2, getLibeleParArticle(2), getPrixParArticle(2),
            3, getLibeleParArticle(3), getPrixParArticle(3));

    //on envoie le catalogue au client
    send(fdSocketCommunication, tampon, strlen(tampon), 0);

    //TRAITEMENT DU PRODUIT DEMANDÉ PAR LUTILISATEUR
    int idProduit = -1;
    do {
        //on attend le produit demandé par le client
        nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);

        //réception  de l’id du produit demandé par le client
        if (nbRecu <= 0) {
            exit(EXIT_FAILURE);
        }
        tampon[nbRecu] = 0;
        idProduit = atoi(tampon);

        if (isIdProduitValide(idProduit)) {
            sprintf(tampon, "[%d] %s - %d€ - Stock : %d\n", idProduit, getLibeleParArticle(idProduit),
                    getPrixParArticle(idProduit), getStockParArticle(idProduit));
            send(fdSocketCommunication, tampon, strlen(tampon), 0);
            printf("Infos envoyées au client\n");
        } else {
            sprintf(tampon, "%d", -1);
            send(fdSocketCommunication, tampon, strlen(tampon), 0);
            printf("Message erreur envoyé au client\n");
        }
    } while (isIdProduitValide(idProduit) != 1);



    //TRAITEMENT DE LA QUANTITE DEMANDÉE PAR L'UTILISATEUR
    int quantiteDemandee = -1;
    do {
        // on attend la quantité demandée par le client
        nbRecu = recv(fdSocketCommunication, tampon, MAX_BUFFER, 0);

        //réception  de la quantité demandée par le client
        if (nbRecu <= 0) {
            exit(EXIT_FAILURE);
        }

        tampon[nbRecu] = 0;
        quantiteDemandee = atoi(tampon);

        printf("LE TAMPON : %s\n", tampon);
        printf("Lid : %d\n", idProduit);

        if (isQuantiteDisponible(quantiteDemandee, idProduit)) {
            printf("Reçu : %s\n", tampon);

            char facture[MAX_BUFFER];

            creerFacture(idProduit, quantiteDemandee, facture);
            send(fdSocketCommunication, facture, strlen(facture), 0);
            Magasin.stockParArticle[idProduit-1] = getStockParArticle(idProduit) - quantiteDemandee;
            printf("Quantite de %s restante : %d\n", getLibeleParArticle(idProduit) , Magasin.stockParArticle[idProduit-1]);

        } else {
            sprintf(tampon, "%d", -1);
            send(fdSocketCommunication, tampon, strlen(tampon), 0);
            printf("Message erreur envoyé au client\n");
        }
    } while (isQuantiteDisponible(quantiteDemandee, idProduit) != 1);


    close(fdSocketCommunication);
    close(fdSocketAttente);

    return EXIT_SUCCESS;
}

// ------------------------- Fonctions Magasin ----------------------


void initialiserMagasin(void) {
    for (int i = 0; i < NB_ARTICLES; ++i) {
        Magasin.idArticles[i] = i + 1;
        Magasin.stockParArticle[i] = 10;
        Magasin.prixParArticle[i] = 300 + ((i * 10) * 50);
    }
    printf("Magasin initialisé !\n");
}

int getStockParArticle(int idArticle) {
    int stockArticle = Magasin.stockParArticle[idArticle - 1];
    return stockArticle;
}

int getPrixParArticle(int idArticle) {
    int prixArticle = Magasin.prixParArticle[idArticle - 1];
    return prixArticle;
}

char *getLibeleParArticle(int idArticle) {
    if (idArticle == 1) {
        return "Projet en C";
    } else if (idArticle == 2) {
        return "Projet en Java";
    } else if (idArticle == 3) {
        return "Projet en CSS only (HTML fourni dans un pack additionnel)";
    }
}

void afficherArticlesEtPrix(void) {
    printf("[%d] %s - %d€ \n"
           "[%d] %s - %d€\n"
           "[%d] %s - %d€\n",
           1, getLibeleParArticle(1), getPrixParArticle(1),
           2, getLibeleParArticle(2), getPrixParArticle(2),
           3, getLibeleParArticle(3), getPrixParArticle(3));
}

void afficherStockParArticle(int idArticle) {
    if (idArticle == 1) {
        printf("[%d] %s - %d quantite en stock !\n", idArticle, getLibeleParArticle(idArticle),
               getStockParArticle(idArticle));
    } else if (idArticle == 2) {
        printf("[%d] %s - %d quantite en stock !\n", idArticle, getLibeleParArticle(idArticle),
               getStockParArticle(idArticle));
    } else if (idArticle == 3) {
        printf("[%d] %s - %d quantite en stock !\n", idArticle, getLibeleParArticle(idArticle),
               getStockParArticle(idArticle));
    }
}

int isIdProduitValide(int idProduit) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (idProduit == Magasin.idArticles[i]) {
            return 1;
        }
    }
    return 0;
}

int isQuantiteDisponible(int quantitedemandee, int idProduit) {
    if (getStockParArticle(idProduit) < quantitedemandee || quantitedemandee == 0) {
        return 0;
    }
    return 1;
}

// ------------------------- TCP ----------------------------------
int ouvrirUneSocketAttente(void) {
    int socketTemp;
    int longueurAdresse;
    struct sockaddr_in coordonneesServeur;

    socketTemp = socket(PF_INET, SOCK_STREAM, 0);

    if (socketTemp < 0) {
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

    if (bind(socketTemp, (struct sockaddr *) &coordonneesServeur, sizeof(coordonneesServeur)) == -1) {
        printf("erreur de bind\n");
        exit(EXIT_FAILURE);
    }

    if (listen(socketTemp, 5) == -1) {
        printf("erreur de listen\n");
        exit(EXIT_FAILURE);
    }

    printf("En attente de connexion...\n");

    return socketTemp;
}

void creerFacture(int idProduit, int quantite, char facture[]) {
    int prixTotal = getPrixParArticle(idProduit) * quantite;
    sprintf(facture, "Facture client\n"
                     "- Article choisi : %s \n"
                     "- Quantite achetée : %d - Prix unitaire : %d€ \n"
                     "- Prix Total : %d€\n",
            getLibeleParArticle(idProduit), quantite, getPrixParArticle(idProduit),
            prixTotal);
    facture[strlen(facture)] = 0;
}