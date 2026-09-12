#ifndef BUSCA_LARGURA_H
#define BUSCA_LARGURA_H

#include "grafo_lista.h"

/* Fila FIFO obrigatoria para a busca em largura. */
typedef struct {
    int *dados;
    int capacidade, inicio, fim, tamanho;
} Fila;

void bfs(GrafoLista *g, int origem, int *dist, int *pred);

#endif