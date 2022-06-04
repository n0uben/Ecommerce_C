#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define PORT 6000
#define MAX_BUFFER 1000
#define MAX_CLIENTS 3
#define NB_ARTICLES 3
#define EXIT "exit"

struct {
    int idArticles[NB_ARTICLES];
    int stockParArticle[NB_ARTICLES];
    int prixParArticle[NB_ARTICLES];

}Magasin;

void initialiserMagasin()
{
    for (int i = 0; i < NB_ARTICLES; ++i) {
        Magasin.idArticles[i] = i+1;
        Magasin.stockParArticle[i] = 10;
        Magasin.prixParArticle[i] = 100 + (i * 50);
    }
    printf("Magasin initilise !\n");
}

void getListeArticles()
{
    return char *listeArticles = "[1] Projet en C - 500€ \n"
                                 "[2] Projet en java - 1000€\n"
                                 "[3] Projet en CSS only (HTML fourni dans un pack additionnel) - 2999€\n";
}

char concatener(int valeur)
{
    const char* str1 = "hello there";
    int n1 = 1234;

    char *num;
    char buffer[MAX];

    if (asprintf(&num, "%d", n1) == -1) {
        perror("asprintf");
    } else {
        return strcat(strcpy(buffer, str1), num);
    }
}

void getStockParArticle(int idArticle)
{
    int stockArticle = Magasin.stockParArticle[idArticle - 1];
    printf("Voici le stock de l'article %d : %d\n", idArticle, stockArticle);
}

void getPrixParArticle(int idArticle)
{
    int prixArticle = Magasin.prixParArticle[idArticle - 1];
    printf("Voici le prix de l'article %d : %d\n", idArticle, prixArticle);
}


int main(void) {
    initialiserMagasin();
    getStockParArticle(3);
    getPrixParArticle(3);

}
