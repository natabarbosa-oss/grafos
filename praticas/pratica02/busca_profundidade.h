#ifndef BUSCA_PROFUNDIDADE_H
#define BUSCA_PROFUNDIDADE_H

#include "grafo_lista.h"

/* Pilha LIFO obrigatoria para a busca em profundidade. */
typedef struct {
    int *dados;
    int topo, capacidade;
} Pilha;

void dfs_recursiva(GrafoLista *g, int u, int *visitado,
                   int *tempo_entrada, int *tempo_saida, int *tempo);

int contar_componentes(GrafoLista *g);
int tem_ciclo(GrafoLista *g);
int eh_bipartido(GrafoLista *g);

#endif