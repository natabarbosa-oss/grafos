#include "bellman_ford.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

GrafoListaPonderado *criar_grafo_lista_ponderado(int n)
{
    GrafoListaPonderado *g;

    if (n <= 0 || (size_t)n > SIZE_MAX / sizeof(NoPonderado *)) {
        return NULL;
    }

    g = malloc(sizeof(*g));
    if (!g) {
        return NULL;
    }

    g->n = n;
    g->adj = calloc((size_t)n, sizeof(*g->adj));
    if (!g->adj) {
        free(g);
        return NULL;
    }

    return g;
}

int inserir_aresta_ponderada(GrafoListaPonderado *g, int origem, int destino,
                             int peso)
{
    NoPonderado *novo;

    if (!g || !g->adj || origem < 0 || origem >= g->n || destino < 0
        || destino >= g->n) {
        return 0;
    }

    novo = malloc(sizeof(*novo));
    if (!novo) {
        return 0;
    }

    novo->destino = destino;
    novo->peso = peso;
    novo->prox = g->adj[origem];
    g->adj[origem] = novo;
    return 1;
}

void liberar_grafo_lista_ponderado(GrafoListaPonderado *g)
{
    int i;

    if (!g) {
        return;
    }

    if (g->adj) {
        for (i = 0; i < g->n; ++i) {
            NoPonderado *atual = g->adj[i];
            while (atual) {
                NoPonderado *proximo = atual->prox;
                free(atual);
                atual = proximo;
            }
        }
    }

    free(g->adj);
    free(g);
}

void reconstruir_ciclo_negativo(int vertice, int n, const int *pred)
{
    int inicio;
    int atual;
    int *ciclo;
    int tamanho = 0;
    int i;

    if (!pred || n <= 0 || vertice < 0 || vertice >= n) {
        puts("Nao foi possivel reconstruir o ciclo negativo.");
        return;
    }

    inicio = vertice;
    for (i = 0; i < n; ++i) {
        if (inicio < 0 || inicio >= n) {
            puts("Nao foi possivel reconstruir o ciclo negativo.");
            return;
        }
        inicio = pred[inicio];
    }

    if (inicio < 0 || inicio >= n) {
        puts("Nao foi possivel reconstruir o ciclo negativo.");
        return;
    }

    ciclo = malloc(((size_t)n + 1U) * sizeof(*ciclo));
    if (!ciclo) {
        puts("Memoria insuficiente para reconstruir o ciclo negativo.");
        return;
    }

    atual = inicio;
    do {
        if (atual < 0 || atual >= n || tamanho >= n) {
            free(ciclo);
            puts("Nao foi possivel reconstruir o ciclo negativo.");
            return;
        }
        ciclo[tamanho++] = atual;
        atual = pred[atual];
    } while (atual != inicio);

    ciclo[tamanho] = inicio;
    printf("Ciclo negativo: ");
    for (i = tamanho; i >= 0; --i) {
        printf("%d%s", ciclo[i], i > 0 ? " -> " : "\n");
    }

    free(ciclo);
}

static int distancia_para_int(int64_t distancia)
{
    if (distancia >= INT_MAX) {
        return INT_MAX - 1;
    }
    if (distancia < INT_MIN) {
        return INT_MIN;
    }
    return (int)distancia;
}

int bellman_ford(GrafoListaPonderado *g, int origem, int *dist, int *pred)
{
    int64_t *dist64;
    const int64_t infinito = INT64_MAX;
    int i;
    int u;
    int vertice_ciclo = -1;

    if (!g || g->n <= 0 || !dist || !pred) {
        return 1;
    }

    for (i = 0; i < g->n; ++i) {
        dist[i] = INT_MAX;
        pred[i] = -1;
    }

    if (!g->adj || origem < 0 || origem >= g->n
        || (size_t)g->n > SIZE_MAX / sizeof(*dist64)) {
        return 1;
    }

    dist64 = malloc((size_t)g->n * sizeof(*dist64));
    if (!dist64) {
        return 1;
    }

    for (i = 0; i < g->n; ++i) {
        dist64[i] = infinito;
    }
    dist64[origem] = 0;

    /* Ate n - 1 iteracoes de relaxacao de todas as arestas. */
    for (i = 0; i < g->n - 1; ++i) {
        int alterou = 0;

        for (u = 0; u < g->n; ++u) {
            NoPonderado *aresta;

            /* Arestas que partem de vertices inalcançaveis nao sao relaxadas. */
            if (dist64[u] == infinito) {
                continue;
            }

            for (aresta = g->adj[u]; aresta; aresta = aresta->prox) {
                int v = aresta->destino;
                int64_t candidata;

                if (v < 0 || v >= g->n) {
                    free(dist64);
                    return 1;
                }

                candidata = dist64[u] + (int64_t)aresta->peso;
                if (candidata < dist64[v]) {
                    dist64[v] = candidata;
                    pred[v] = u;
                    alterou = 1;
                }
            }
        }

        if (!alterou) {
            break;
        }
    }

    /* Iteracao adicional: qualquer relaxacao indica ciclo negativo alcançavel. */
    for (u = 0; u < g->n; ++u) {
        NoPonderado *aresta;

        if (dist64[u] == infinito) {
            continue;
        }

        for (aresta = g->adj[u]; aresta; aresta = aresta->prox) {
            int v = aresta->destino;
            int64_t candidata;

            if (v < 0 || v >= g->n) {
                free(dist64);
                return 1;
            }

            candidata = dist64[u] + (int64_t)aresta->peso;
            if (candidata < dist64[v]) {
                dist64[v] = candidata;
                pred[v] = u;
                vertice_ciclo = v;
            }
        }
    }

    for (i = 0; i < g->n; ++i) {
        if (dist64[i] != infinito) {
            dist[i] = distancia_para_int(dist64[i]);
        }
    }

    free(dist64);

    if (vertice_ciclo != -1) {
        reconstruir_ciclo_negativo(vertice_ciclo, g->n, pred);
        return 0;
    }

    return 1;
}
