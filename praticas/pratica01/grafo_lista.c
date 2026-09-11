#include <stdlib.h>
#include "grafo_lista.h"

GrafoLista *criar_grafo_lista(int n) {
    if (n <= 0) {
        return NULL;
    }

    GrafoLista *grafo = malloc(sizeof(GrafoLista));

    if (grafo == NULL) {
        return NULL;
    }

    grafo->n = n;
    grafo->adj = calloc(n, sizeof(No *));

    if (grafo->adj == NULL) {
        free(grafo);
        return NULL;
    }

    return grafo;
}

int sao_adjacentes_lista(GrafoLista *grafo, int u, int v) {
    if (grafo == NULL || u < 0 || v < 0 ||
        u >= grafo->n || v >= grafo->n) {
        return 0;
    }

    No *atual = grafo->adj[u];

    while (atual != NULL) {
        if (atual->destino == v) {
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

void inserir_aresta_lista(GrafoLista *grafo, int u, int v) {
    if (grafo == NULL || u < 0 || v < 0 ||
        u >= grafo->n || v >= grafo->n || u == v) {
        return;
    }

    if (sao_adjacentes_lista(grafo, u, v)) {
        return;
    }

    No *novo_u = malloc(sizeof(No));
    No *novo_v = malloc(sizeof(No));

    if (novo_u == NULL || novo_v == NULL) {
        free(novo_u);
        free(novo_v);
        return;
    }

    novo_u->destino = v;
    novo_u->prox = grafo->adj[u];
    grafo->adj[u] = novo_u;

    novo_v->destino = u;
    novo_v->prox = grafo->adj[v];
    grafo->adj[v] = novo_v;
}

static void remover_da_lista(No **inicio, int destino) {
    No *atual = *inicio;
    No *anterior = NULL;

    while (atual != NULL && atual->destino != destino) {
        anterior = atual;
        atual = atual->prox;
    }

    if (atual == NULL) {
        return;
    }

    if (anterior == NULL) {
        *inicio = atual->prox;
    } else {
        anterior->prox = atual->prox;
    }

    free(atual);
}

void remover_aresta_lista(GrafoLista *grafo, int u, int v) {
    if (grafo == NULL || u < 0 || v < 0 ||
        u >= grafo->n || v >= grafo->n) {
        return;
    }

    remover_da_lista(&grafo->adj[u], v);
    remover_da_lista(&grafo->adj[v], u);
}

int grau_lista(GrafoLista *grafo, int u) {
    if (grafo == NULL || u < 0 || u >= grafo->n) {
        return -1;
    }

    int grau = 0;
    No *atual = grafo->adj[u];

    while (atual != NULL) {
        grau++;
        atual = atual->prox;
    }

    return grau;
}

void liberar_grafo_lista(GrafoLista *grafo) {
    if (grafo == NULL) {
        return;
    }

    for (int i = 0; i < grafo->n; i++) {
        No *atual = grafo->adj[i];

        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }

    free(grafo->adj);
    free(grafo);
}