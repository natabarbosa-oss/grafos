#include "coloracao.h"

#include <stdlib.h>

static int vertice_valido(const GrafoLista *g, int v)
{
    return g != NULL && v >= 0 && v < g->n;
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

void inserir_aresta_lista(GrafoLista *g, int u, int v)
{
    No *novo_u;
    No *novo_v;

    if (!vertice_valido(g, u) || !vertice_valido(g, v) || u == v) {
        return;
    }

    novo_u = malloc(sizeof(*novo_u));
    novo_v = malloc(sizeof(*novo_v));

    if (novo_u == NULL || novo_v == NULL) {
        free(novo_u);
        free(novo_v);
        return;
    }

    novo_u->destino = v;
    novo_u->prox = g->adj[u];
    g->adj[u] = novo_u;

    novo_v->destino = u;
    novo_v->prox = g->adj[v];
    g->adj[v] = novo_v;
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

static int pode_usar_cor(GrafoLista *g, int vertice,
                         int cor, const int cores[])
{
    No *atual;

    for (atual = g->adj[vertice]; atual != NULL; atual = atual->prox) {
        if (cores[atual->destino] == cor) {
            return 0;
        }
    }

    return 1;
}

int *coloracao_gulosa(GrafoLista *g, int *num_cores)
{
    int *cores;
    int v;
    int maior_cor = -1;

    if (g == NULL || num_cores == NULL) {
        return NULL;
    }

    cores = malloc((size_t)g->n * sizeof(*cores));

    if (g->n > 0 && cores == NULL) {
        return NULL;
    }

    for (v = 0; v < g->n; v++) {
        cores[v] = -1;
    }

    for (v = 0; v < g->n; v++) {
        int cor = 0;

        while (!pode_usar_cor(g, v, cor, cores)) {
            cor++;
        }

        cores[v] = cor;

        if (cor > maior_cor) {
            maior_cor = cor;
        }
    }

    *num_cores = maior_cor + 1;

    return cores;
}

static int grau_vertice(GrafoLista *g, int v)
{
    No *atual;
    int grau = 0;

    for (atual = g->adj[v]; atual != NULL; atual = atual->prox) {
        grau++;
    }

    return grau;
}

int *coloracao_welsh_powell(GrafoLista *g, int *num_cores)
{
    int *cores;
    int *ordem;
    int i;
    int j;
    int maior_cor = -1;

    if (g == NULL || num_cores == NULL) {
        return NULL;
    }

    cores = malloc((size_t)g->n * sizeof(*cores));
    ordem = malloc((size_t)g->n * sizeof(*ordem));

    if (g->n > 0 && (cores == NULL || ordem == NULL)) {
        free(cores);
        free(ordem);
        return NULL;
    }

    for (i = 0; i < g->n; i++) {
        cores[i] = -1;
        ordem[i] = i;
    }

    for (i = 0; i < g->n - 1; i++) {
        for (j = i + 1; j < g->n; j++) {
            if (grau_vertice(g, ordem[j]) >
                grau_vertice(g, ordem[i])) {
                int temp = ordem[i];
                ordem[i] = ordem[j];
                ordem[j] = temp;
            }
        }
    }

    for (i = 0; i < g->n; i++) {
        int vertice = ordem[i];
        int cor = 0;

        while (!pode_usar_cor(g, vertice, cor, cores)) {
            cor++;
        }

        cores[vertice] = cor;

        if (cor > maior_cor) {
            maior_cor = cor;
        }
    }

    *num_cores = maior_cor + 1;

    free(ordem);

    return cores;
}

int eh_bipartido(GrafoLista *g)
{
    int *cores;
    int *fila;
    int inicio;
    int fim;
    int origem;

    if (g == NULL) {
        return 0;
    }

    cores = malloc((size_t)g->n * sizeof(*cores));
    fila = malloc((size_t)g->n * sizeof(*fila));

    if (g->n > 0 && (cores == NULL || fila == NULL)) {
        free(cores);
        free(fila);
        return 0;
    }

    for (origem = 0; origem < g->n; origem++) {
        cores[origem] = -1;
    }

    for (origem = 0; origem < g->n; origem++) {
        if (cores[origem] != -1) {
            continue;
        }

        inicio = 0;
        fim = 0;

        fila[fim++] = origem;
        cores[origem] = 0;

        while (inicio < fim) {
            int u = fila[inicio++];
            No *atual;

            for (atual = g->adj[u]; atual != NULL;
                 atual = atual->prox) {
                int v = atual->destino;

                if (cores[v] == -1) {
                    cores[v] = 1 - cores[u];
                    fila[fim++] = v;
                } else if (cores[v] == cores[u]) {
                    free(cores);
                    free(fila);
                    return 0;
                }
            }
        }
    }

    free(cores);
    free(fila);

    return 1;
}