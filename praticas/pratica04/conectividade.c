#include "conectividade.h"

#include <stdlib.h>

static int vertice_valido(const GrafoLista *g, int v)
{
    return g != NULL && v >= 0 && v < g->n;
}

static int sao_adjacentes(const GrafoLista *g, int u, int v)
{
    No *atual;

    if (!vertice_valido(g, u) || !vertice_valido(g, v)) {
        return 0;
    }
    for (atual = g->adj[u]; atual != NULL; atual = atual->prox) {
        if (atual->destino == v) {
            return 1;
        }
    }
    return 0;
}

GrafoLista *criar_grafo_lista(int n)
{
    GrafoLista *g;

    if (n < 0) {
        return NULL;
    }
    g = malloc(sizeof(*g));
    if (g == NULL) {
        return NULL;
    }
    g->n = n;
    g->adj = calloc((size_t)n, sizeof(*g->adj));
    if (n > 0 && g->adj == NULL) {
        free(g);
        return NULL;
    }
    return g;
}

int inserir_aresta_lista(GrafoLista *g, int u, int v)
{
    No *uv;
    No *vu;

    if (!vertice_valido(g, u) || !vertice_valido(g, v) || u == v ||
        sao_adjacentes(g, u, v)) {
        return 0;
    }

    uv = malloc(sizeof(*uv));
    vu = malloc(sizeof(*vu));

    if (uv == NULL || vu == NULL) {
        free(uv);
        free(vu);
        return 0;
    }

    uv->destino = v;
    uv->prox = g->adj[u];
    vu->destino = u;
    vu->prox = g->adj[v];
    g->adj[u] = uv;
    g->adj[v] = vu;

    return 1;
}

void liberar_grafo_lista(GrafoLista *g)
{
    int u;

    if (g == NULL) {
        return;
    }

    for (u = 0; u < g->n; u++) {
        No *atual = g->adj[u];

        while (atual != NULL) {
            No *proximo = atual->prox;
            free(atual);
            atual = proximo;
        }
    }

    free(g->adj);
    free(g);
}

void dfs_articulacoes(GrafoLista *g, int u, int *tempo,
                      int descoberta[], int low[], int pai[],
                      int articulacao[])
{
    No *atual;
    int filhos = 0;

    descoberta[u] = ++(*tempo);
    low[u] = descoberta[u];

    for (atual = g->adj[u]; atual != NULL; atual = atual->prox) {
        int v = atual->destino;

        if (descoberta[v] == 0) {
            filhos++;
            pai[v] = u;

            dfs_articulacoes(g, v, tempo, descoberta, low, pai,
                             articulacao);

            if (low[v] < low[u]) {
                low[u] = low[v];
            }

            /* A raiz precisa de dois filhos; os demais usam
               low[v] >= descoberta[u]. */
            if ((pai[u] == -1 && filhos > 1) ||
                (pai[u] != -1 && low[v] >= descoberta[u])) {
                articulacao[u] = 1;
            }
        } else if (v != pai[u] && descoberta[v] < low[u]) {
            low[u] = descoberta[v];
        }
    }
}

int *detectar_articulacoes(GrafoLista *g)
{
    int *descoberta;
    int *low;
    int *pai;
    int *articulacao;
    int tempo = 0;
    int u;

    if (g == NULL) {
        return NULL;
    }

    descoberta = calloc((size_t)g->n, sizeof(*descoberta));
    low = calloc((size_t)g->n, sizeof(*low));
    pai = malloc((size_t)g->n * sizeof(*pai));
    articulacao = calloc((size_t)g->n, sizeof(*articulacao));

    if (g->n > 0 && (descoberta == NULL || low == NULL ||
                     pai == NULL || articulacao == NULL)) {
        free(descoberta);
        free(low);
        free(pai);
        free(articulacao);
        return NULL;
    }

    for (u = 0; u < g->n; u++) {
        pai[u] = -1;
    }

    for (u = 0; u < g->n; u++) {
        if (descoberta[u] == 0) {
            dfs_articulacoes(g, u, &tempo, descoberta, low, pai,
                             articulacao);
        }
    }

    free(descoberta);
    free(low);
    free(pai);

    return articulacao;
}

static int adicionar_ponte(Aresta **pontes, int *quantidade,
                           int *capacidade, int u, int v)
{
    if (*quantidade == *capacidade) {
        int nova_capacidade = *capacidade == 0 ? 4 : *capacidade * 2;
        Aresta *novo = realloc(*pontes,
                               (size_t)nova_capacidade * sizeof(*novo));

        if (novo == NULL) {
            return 0;
        }

        *pontes = novo;
        *capacidade = nova_capacidade;
    }

    (*pontes)[*quantidade].u = u;
    (*pontes)[*quantidade].v = v;
    (*quantidade)++;

    return 1;
}

static int dfs_pontes(GrafoLista *g, int u, int *tempo,
                      int descoberta[], int low[], int pai[],
                      Aresta **pontes, int *quantidade,
                      int *capacidade)
{
    No *atual;

    descoberta[u] = ++(*tempo);
    low[u] = descoberta[u];

    for (atual = g->adj[u]; atual != NULL; atual = atual->prox) {
        int v = atual->destino;

        if (descoberta[v] == 0) {
            pai[v] = u;

            if (!dfs_pontes(g, v, tempo, descoberta, low, pai,
                            pontes, quantidade, capacidade)) {
                return 0;
            }

            if (low[v] < low[u]) {
                low[u] = low[v];
            }

            if (low[v] > descoberta[u] &&
                !adicionar_ponte(pontes, quantidade,
                                 capacidade, u, v)) {
                return 0;
            }
        } else if (v != pai[u] && descoberta[v] < low[u]) {
            low[u] = descoberta[v];
        }
    }

    return 1;
}

Aresta *detectar_pontes(GrafoLista *g, int *quantidade)
{
    int *descoberta;
    int *low;
    int *pai;
    Aresta *pontes = NULL;
    int capacidade = 0;
    int tempo = 0;
    int u;

    if (quantidade == NULL) {
        return NULL;
    }

    *quantidade = 0;

    if (g == NULL) {
        return NULL;
    }

    descoberta = calloc((size_t)g->n, sizeof(*descoberta));
    low = calloc((size_t)g->n, sizeof(*low));
    pai = malloc((size_t)g->n * sizeof(*pai));

    if (g->n > 0 &&
        (descoberta == NULL || low == NULL || pai == NULL)) {
        free(descoberta);
        free(low);
        free(pai);
        return NULL;
    }

    for (u = 0; u < g->n; u++) {
        pai[u] = -1;
    }

    for (u = 0; u < g->n; u++) {
        if (descoberta[u] == 0 &&
            !dfs_pontes(g, u, &tempo, descoberta, low, pai,
                        &pontes, quantidade, &capacidade)) {
            free(pontes);
            pontes = NULL;
            *quantidade = 0;
            break;
        }
    }

    free(descoberta);
    free(low);
    free(pai);

    return pontes;
}