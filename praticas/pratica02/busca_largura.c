#include "busca_largura.h"

#include <stdlib.h>

static int iniciar_fila(Fila *fila, int capacidade)
{
    fila->dados = malloc((size_t)capacidade * sizeof(*fila->dados));

    if (fila->dados == NULL) {
        fila->capacidade = 0;
        fila->inicio = 0;
        fila->fim = 0;
        fila->tamanho = 0;
        return 0;
    }

    fila->capacidade = capacidade;
    fila->inicio = 0;
    fila->fim = 0;
    fila->tamanho = 0;

    return 1;
}

static void enfileirar(Fila *fila, int valor)
{
    fila->dados[fila->fim] = valor;
    fila->fim = (fila->fim + 1) % fila->capacidade;
    fila->tamanho++;
}

static int desenfileirar(Fila *fila)
{
    int valor = fila->dados[fila->inicio];

    fila->inicio = (fila->inicio + 1) % fila->capacidade;
    fila->tamanho--;

    return valor;
}

void bfs(GrafoLista *g, int origem, int *dist, int *pred)
{
    Fila fila;
    int i;

    if (g == NULL || dist == NULL || pred == NULL) {
        return;
    }

    for (i = 0; i < g->n; i++) {
        dist[i] = -1;
        pred[i] = -1;
    }

    if (origem < 0 || origem >= g->n || !iniciar_fila(&fila, g->n)) {
        return;
    }

    dist[origem] = 0;
    enfileirar(&fila, origem);

    while (fila.tamanho > 0) {
        int u = desenfileirar(&fila);
        No *atual = g->adj[u];

        while (atual != NULL) {
            int v = atual->destino;

            if (dist[v] == -1) {
                dist[v] = dist[u] + 1;
                pred[v] = u;
                enfileirar(&fila, v);
            }

            atual = atual->prox;
        }
    }

    free(fila.dados);
}