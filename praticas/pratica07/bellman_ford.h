#ifndef BELLMAN_FORD_H
#define BELLMAN_FORD_H

#include <limits.h>

typedef struct NoPonderado {
    int destino;
    int peso;
    struct NoPonderado *prox;
} NoPonderado;

typedef struct {
    int n;
    NoPonderado **adj;
} GrafoListaPonderado;

GrafoListaPonderado *criar_grafo_lista_ponderado(int n);
int inserir_aresta_ponderada(GrafoListaPonderado *g, int origem, int destino,
                             int peso);
void liberar_grafo_lista_ponderado(GrafoListaPonderado *g);

/*
 * Calcula as menores distancias a partir de origem.
 * INT_MAX representa um vertice inalcançavel.
 * Retorna 1 quando nao ha ciclo negativo alcançavel e 0 quando ha.
 */
int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred);

/*
 * Retrocede n vezes a partir de vertice e imprime o ciclo em ordem de arestas.
 */
void reconstruir_ciclo_negativo(int vertice, int n, const int *pred);

#endif
