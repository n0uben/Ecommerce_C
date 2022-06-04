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
        Magasin.prixParArticle[i] = 300 + ((i * 10) * 50);
    }
    printf("Magasin initilise !\n");
}

int getStockParArticle(int idArticle)
{
    int stockArticle = Magasin.stockParArticle[idArticle - 1];
    return stockArticle;
}

int getPrixParArticle(int idArticle)
{
    int prixArticle = Magasin.prixParArticle[idArticle - 1];
    return prixArticle;
}

void afficherArticles()
{
    printf("[%d] Projet en C - %d€ \n"
           "[%d] Projet en Java - %d€\n"
           "[%d] Projet en CSS only (HTML fourni dans un pack additionnel) - %d€\n",
           1, getPrixParArticle(1),
           2, getPrixParArticle(2),
           3, getPrixParArticle(3));
}

void afficherStockParArticle(int idArticle)
{
    if (idArticle == 1)
    {
        printf("[%d] Projet en C - %d quantite en stock !", idArticle, getStockParArticle(idArticle));
    }
    else if (idArticle == 2)
    {
        printf("[%d] Projet en Java - %d quantite en stock !", idArticle, getStockParArticle(idArticle));
    }
    else if (idArticle == 3)
    {
        printf("[%d] Projet en CSS only (HTML fourni dans un pack additionnel) - %d quantite en stock !", idArticle, getStockParArticle(idArticle));
    }
}



int main(void) {
    initialiserMagasin();
    afficherArticles();
}