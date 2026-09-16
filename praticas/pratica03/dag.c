#include "dag.h"

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

int inserir_aresta_lista(GrafoLista *g, int u, int v)
{
    No *novo;

    if (g == NULL || u < 0 || u >= g->n || v < 0 || v >= g->n) {
        return 0;
    }

    novo = malloc(sizeof(*novo));

    if (novo == NULL) {
        return 0;
    }

    novo->destino = v;
    novo->prox = g->adj[u];
    g->adj[u] = novo;

    return 1;
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

int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho)
{
    int *grau_entrada;
    int *fila;
    int *ordem;
    int inicio = 0;
    int fim = 0;
    int quantidade = 0;
    int i;

    if (g == NULL || tamanho == NULL) {
        return NULL;
    }

    *tamanho = 0;

    grau_entrada = calloc((size_t)g->n, sizeof(*grau_entrada));
    fila = malloc((size_t)g->n * sizeof(*fila));
    ordem = malloc((size_t)g->n * sizeof(*ordem));

    if (grau_entrada == NULL || fila == NULL || ordem == NULL) {
        free(grau_entrada);
        free(fila);
        free(ordem);
        return NULL;
    }

    for (i = 0; i < g->n; i++) {
        No *atual = g->adj[i];

        while (atual != NULL) {
            grau_entrada[atual->destino]++;
            atual = atual->prox;
        }
    }

    for (i = 0; i < g->n; i++) {
        if (grau_entrada[i] == 0) {
            fila[fim++] = i;
        }
    }

    while (inicio < fim) {
        int u = fila[inicio++];
        No *atual = g->adj[u];

        ordem[quantidade++] = u;

        while (atual != NULL) {
            int v = atual->destino;

            grau_entrada[v]--;

            if (grau_entrada[v] == 0) {
                fila[fim++] = v;
            }

            atual = atual->prox;
        }
    }

    free(grau_entrada);
    free(fila);

    if (quantidade != g->n) {
        free(ordem);
        return NULL;
    }

    *tamanho = quantidade;
    return ordem;
}

static int dfs_topologica(GrafoLista *g, int u, int *estado,
                          int *ordem, int *posicao)
{
    No *atual;

    estado[u] = 1;
    atual = g->adj[u];

    while (atual != NULL) {
        int v = atual->destino;

        if (estado[v] == 1) {
            return 0;
        }

        if (estado[v] == 0) {
            if (!dfs_topologica(g, v, estado, ordem, posicao)) {
                return 0;
            }
        }

        atual = atual->prox;
    }

    estado[u] = 2;
    ordem[(*posicao)--] = u;

    return 1;
}

int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho)
{
    int *estado;
    int *ordem;
    int posicao;
    int i;

    if (g == NULL || tamanho == NULL) {
        return NULL;
    }

    *tamanho = 0;

    estado = calloc((size_t)g->n, sizeof(*estado));
    ordem = malloc((size_t)g->n * sizeof(*ordem));

    if (estado == NULL || ordem == NULL) {
        free(estado);
        free(ordem);
        return NULL;
    }

    posicao = g->n - 1;

    for (i = 0; i < g->n; i++) {
        if (estado[i] == 0) {
            if (!dfs_topologica(g, i, estado, ordem, &posicao)) {
                free(estado);
                free(ordem);
                return NULL;
            }
        }
    }

    free(estado);

    *tamanho = g->n;
    return ordem;
}

int eh_dag(GrafoLista *g)
{
    int tamanho;
    int *ordem;

    if (g == NULL) {
        return 0;
    }

    ordem = ordenacao_topologica_kahn(g, &tamanho);

    if (ordem == NULL) {
        return 0;
    }

    free(ordem);
    return 1;
}