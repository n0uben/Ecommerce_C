#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 6000
#define MAX_BUFFER 1000
#define ADRESSE_SERVEUR "127.0.0.1"

const char *EXIT = "exit";

void lireCommande(char tampon[], char *message);

int testQuitter(char tampon[]);

int main(int argc, char const *argv[])
{
    int fdSocket;
    int nbRecu;
    struct sockaddr_in coordonneesServeur;
    int longueurAdresse;
    char tampon[MAX_BUFFER];

    fdSocket = socket(AF_INET, SOCK_STREAM, 0);

    if (fdSocket < 0)
    {
        printf("socket incorrecte\n");
        exit(EXIT_FAILURE);
    }

    // On prépare les coordonnées du serveur
    longueurAdresse = sizeof(struct sockaddr_in);
    memset(&coordonneesServeur, 0x00, longueurAdresse);

    // connexion de type TCP
    coordonneesServeur.sin_family = PF_INET;
    // adresse du serveur
    inet_aton(ADRESSE_SERVEUR, &coordonneesServeur.sin_addr);
    // le port d'écoute du serveur
    coordonneesServeur.sin_port = htons(PORT);

    //on tente de se connecter
    int maConnexion = connect(fdSocket, (struct sockaddr *) &coordonneesServeur, sizeof(coordonneesServeur));
    if (maConnexion == -1)
    {
        printf("connexion impossible\n");
        exit(EXIT_FAILURE);
    }

    char *entete = "('-. .-.                _ (`-.                               _ .-') _     ('-.\n"
                   "( OO )  /               ( (OO  )                             ( (  OO) )  _(  OO)\n"
                   ",--. ,--. .-'),-----.  _.`     \\         .-----.  .-'),-----. \\     .'_ (,------.\n"
                   "|  | |  |( OO'  .-.  '(__...--''        '  .--./ ( OO'  .-.  ',`'--..._) |  .---'\n"
                   "|   .|  |/   |  | |  | |  /  | |        |  |('-. /   |  | |  ||  |  \\  ' |  |\n"
                   "|       |\\_) |  |\\|  | |  |_.' |       /_) |OO  )\\_) |  |\\|  ||  |   ' |(|  '--.\n"
                   "|  .-.  |  \\ |  | |  | |  .___.'       ||  |`-'|   \\ |  | |  ||  |   / : |  .--'\n"
                   "|  | |  |   `'  '-'  ' |  |           (_'  '--'\\    `'  '-'  '|  '--'  / |  `---.\n"
                   "`--' `--'     `-----'  `--'              `-----'      `-----' `-------'  `------'\n";

    char *choixProduit = "[1] Projet en C - 500€ \n"
                         "[2] Projet en java - 1000€\n"
                         "[3] Projet en CSS only (HTML fourni dans un pack additionnel) - 2999€\n";

    printf("%s", entete);
    printf("%s", choixProduit);

    lireCommande(tampon, "Veuillez choisir un produit (saisissez le numéro 1, 2 ou 3 :");

    // on envoie le message au serveur
    send(fdSocket, tampon, strlen(tampon), 0);

    // on attend la réponse du serveur
    nbRecu = recv(fdSocket, tampon, MAX_BUFFER, 0);

    if (nbRecu > 0)
    {
        tampon[nbRecu] = 0;
        printf("Recu : %s\n", tampon);

        if (testQuitter(tampon))
        {
//            break; // on quitte la boucle
        }
    }

    close(fdSocket);

    return EXIT_SUCCESS;
}

void lireCommande(char tampon[], char *message)
{
    printf("%s\n", message);
    fgets(tampon, MAX_BUFFER, stdin);
    strtok(tampon, "\n");
}

int testQuitter(char tampon[])
{
    return strcmp(tampon, EXIT) == 0;
}