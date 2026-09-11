#include <stdlib.h>
#include "grafo_matriz.h"

GrafoMatriz *criar_grafo_matriz(int n) {
    if (n <= 0) {
        return NULL;
    }

    GrafoMatriz *grafo = malloc(sizeof(GrafoMatriz));

    if (grafo == NULL) {
        return NULL;
    }

    grafo->n = n;
    grafo->adj = malloc(n * sizeof(int *));

    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    for (int i = 0; i < n; i++) {
        grafo->adj[i] = calloc(n, sizeof(int));

        if (grafo->adj[i] == NULL) {
            for (int j = 0; j < i; j++) {
                free(grafo->adj[j]);
            }

            free(grafo->adj);
            free(grafo);

            return NULL;
        }
    }

    return grafo;
}

void inserir_aresta_matriz(GrafoMatriz *grafo, int u, int v) {
    if (grafo == NULL || u < 0 || v < 0 ||
        u >= grafo->n || v >= grafo->n || u == v) {
        return;
    }

    grafo->adj[u][v] = 1;
    grafo->adj[v][u] = 1;
}

void remover_aresta_matriz(GrafoMatriz *grafo, int u, int v) {
    if (grafo == NULL || u < 0 || v < 0 ||
        u >= grafo->n || v >= grafo->n) {
        return;
    }

    grafo->adj[u][v] = 0;
    grafo->adj[v][u] = 0;
}

int grau_matriz(GrafoMatriz *grafo, int u) {
    if (grafo == NULL || u < 0 || u >= grafo->n) {
        return -1;
    }

    int grau = 0;

    for (int v = 0; v < grafo->n; v++) {
        grau += grafo->adj[u][v];
    }

    return grau;
}

int sao_adjacentes_matriz(GrafoMatriz *grafo, int u, int v) {
    if (grafo == NULL || u < 0 || v < 0 ||
        u >= grafo->n || v >= grafo->n) {
        return 0;
    }

    return grafo->adj[u][v];
}

void liberar_grafo_matriz(GrafoMatriz *grafo) {
    if (grafo == NULL) {
        return;
    }

    for (int i = 0; i < grafo->n; i++) {
        free(grafo->adj[i]);
    }

    free(grafo->adj);
    free(grafo);
}