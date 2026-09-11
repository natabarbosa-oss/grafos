#include "grafo_lista.h"

#include <stdio.h>
#include <stdlib.h>

GrafoLista *criar_grafo_lista(int n)
{
    GrafoLista *g;
    int i;

    g = malloc(sizeof(*g));

    if (g == NULL) {
        return NULL;
    }

    g->n = n;
    g->adj = malloc((size_t)n * sizeof(*g->adj));

    if (g->adj == NULL) {
        free(g);
        return NULL;
    }

    for (i = 0; i < n; i++) {
        g->adj[i] = NULL;
    }

    return g;
}

void liberar_grafo_lista(GrafoLista *g)
{
    int i;

    if (g == NULL) {
        return;
    }

    for (i = 0; i < g->n; i++) {
        No *atual = g->adj[i];

        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }

    free(g->adj);
    free(g);
}

void adicionar_aresta_lista(GrafoLista *g, int origem, int destino)
{
    No *novo;

    if (g == NULL || origem < 0 || origem >= g->n ||
        destino < 0 || destino >= g->n) {
        return;
    }

    novo = malloc(sizeof(*novo));

    if (novo == NULL) {
        return;
    }

    novo->destino = destino;
    novo->prox = g->adj[origem];
    g->adj[origem] = novo;

    novo = malloc(sizeof(*novo));

    if (novo == NULL) {
        return;
    }

    novo->destino = origem;
    novo->prox = g->adj[destino];
    g->adj[destino] = novo;
}

void imprimir_grafo_lista(GrafoLista *g)
{
    int i;

    if (g == NULL) {
        return;
    }

    for (i = 0; i < g->n; i++) {
        No *atual = g->adj[i];

        printf("%d:", i);

        while (atual != NULL) {
            printf(" %d", atual->destino);
            atual = atual->prox;
        }

        printf("\n");
    }
}