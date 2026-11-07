#include "dijkstra.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void trocar(MinHeap *h, int a, int b)
{
    /* Toda troca atualiza o mapa inverso usado pela diminuicao de chave. */
    NoHeap tmp = h->dados[a];
    h->dados[a] = h->dados[b];
    h->dados[b] = tmp;
    h->posicao[h->dados[a].vertice] = a;
    h->posicao[h->dados[b].vertice] = b;
}

static void subir(MinHeap *h, int i)
{
    while (i > 0) {
        int pai = (i - 1) / 2;
        if (h->dados[pai].distancia <= h->dados[i].distancia) break;
        trocar(h, pai, i);
        i = pai;
    }
}

MinHeap *criar_min_heap(int capacidade)
{
    if (capacidade <= 0 || (size_t)capacidade > SIZE_MAX / sizeof(NoHeap)
        || (size_t)capacidade > SIZE_MAX / sizeof(int)) return NULL;
    MinHeap *h = calloc(1, sizeof(*h));
    if (!h) return NULL;
    h->dados = malloc((size_t)capacidade * sizeof(*h->dados));
    h->posicao = malloc((size_t)capacidade * sizeof(*h->posicao));
    if (!h->dados || !h->posicao) {
        liberar_min_heap(h);
        return NULL;
    }
    h->capacidade = capacidade;
    for (int i = 0; i < capacidade; ++i) h->posicao[i] = -1;
    return h;
}

void liberar_min_heap(MinHeap *h)
{
    if (!h) return;
    free(h->dados);
    free(h->posicao);
    free(h);
}

int inserir_min_heap(MinHeap *h, int vertice, int distancia)
{
    if (!h || vertice < 0 || vertice >= h->capacidade
        || h->tamanho == h->capacidade || h->posicao[vertice] != -1) return 0;
    int i = h->tamanho++;
    h->dados[i] = (NoHeap){vertice, distancia};
    h->posicao[vertice] = i;
    subir(h, i);
    return 1;
}

void diminuir_chave(MinHeap *h, int vertice, int distancia)
{
    if (!h || vertice < 0 || vertice >= h->capacidade) return;
    int i = h->posicao[vertice];
    if (i < 0 || distancia >= h->dados[i].distancia) return;
    h->dados[i].distancia = distancia;
    subir(h, i);
}

NoHeap extrair_minimo(MinHeap *h)
{
    if (!h || h->tamanho == 0) return (NoHeap){-1, INT_MAX};
    NoHeap minimo = h->dados[0];
    --h->tamanho;
    h->posicao[minimo.vertice] = -1;
    if (h->tamanho == 0) return minimo;
    h->dados[0] = h->dados[h->tamanho];
    h->posicao[h->dados[0].vertice] = 0;
    int i = 0;
    while (h->tamanho > 1 && i <= (h->tamanho - 2) / 2) {
        int filho = 2 * i + 1;
        if (filho + 1 < h->tamanho
            && h->dados[filho + 1].distancia < h->dados[filho].distancia) ++filho;
        if (h->dados[i].distancia <= h->dados[filho].distancia) break;
        trocar(h, i, filho);
        i = filho;
    }
    return minimo;
}

GrafoListaPonderado *criar_grafo_lista_ponderado(int n)
{
    if (n <= 0 || (size_t)n > SIZE_MAX / sizeof(NoPonderado *)) return NULL;
    GrafoListaPonderado *g = malloc(sizeof(*g));
    if (!g) return NULL;
    g->n = n;
    g->adj = calloc((size_t)n, sizeof(*g->adj));
    if (!g->adj) { free(g); return NULL; }
    return g;
}

int inserir_aresta_ponderada(GrafoListaPonderado *g, int origem, int destino, int peso)
{
    if (!g || !g->adj || origem < 0 || origem >= g->n
        || destino < 0 || destino >= g->n || peso < 0) return 0;
    NoPonderado *no = malloc(sizeof(*no));
    if (!no) return 0;
    *no = (NoPonderado){destino, peso, g->adj[origem]};
    g->adj[origem] = no;
    return 1;
}

void liberar_grafo_lista_ponderado(GrafoListaPonderado *g)
{
    if (!g) return;
    if (g->adj) for (int i = 0; i < g->n; ++i) {
        NoPonderado *no = g->adj[i];
        while (no) { NoPonderado *prox = no->prox; free(no); no = prox; }
    }
    free(g->adj);
    free(g);
}

void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred)
{
    /* O(V + E) de validacao; O((V + E) log V) com heap e O(V) auxiliar.
     * Cada vertice ocupa uma unica posicao, sem entradas duplicadas. */
    if (!g || g->n <= 0 || !dist || !pred) return;
    for (int i = 0; i < g->n; ++i) { dist[i] = INT_MAX; pred[i] = -1; }
    if (!g->adj || origem < 0 || origem >= g->n) return;
    for (int i = 0; i < g->n; ++i)
        for (NoPonderado *a = g->adj[i]; a; a = a->prox)
            if (a->peso < 0 || a->destino < 0 || a->destino >= g->n) return;
    MinHeap *h = criar_min_heap(g->n);
    if (!h) return;
    dist[origem] = 0;
    for (int i = 0; i < g->n; ++i) inserir_min_heap(h, i, dist[i]);
    while (h->tamanho > 0) {
        NoHeap minimo = extrair_minimo(h);
        int u = minimo.vertice;
        if (dist[u] == INT_MAX) break;
        for (NoPonderado *a = g->adj[u]; a; a = a->prox) {
            int v = a->destino;
            /* Vertices extraidos estao finalizados; evita overflow na soma. */
            if (h->posicao[v] != -1 && a->peso <= INT_MAX - dist[u]
                && dist[u] + a->peso < dist[v]) {
                dist[v] = dist[u] + a->peso;
                pred[v] = u;
                diminuir_chave(h, v, dist[v]);
            }
        }
    }
    liberar_min_heap(h);
}

void reconstruir_caminho(int origem, int destino, int *pred)
{
    if (!pred || origem < 0 || destino < 0) { puts("Entrada invalida."); return; }
    int lento = destino, rapido = destino;
    do {
        if (rapido == origem || rapido < 0) break;
        rapido = pred[rapido];
        if (rapido == origem || rapido < 0) break;
        rapido = pred[rapido];
        lento = pred[lento];
        if (lento == rapido && rapido >= 0 && rapido != origem) {
            puts("Predecessores invalidos: ciclo."); return;
        }
    } while (rapido >= 0);
    size_t tamanho = 1;
    int v = destino;
    while (v != origem && v >= 0) { v = pred[v]; ++tamanho; }
    if (v != origem) { puts("Sem caminho."); return; }
    if (tamanho > SIZE_MAX / sizeof(int)) return;
    int *caminho = malloc(tamanho * sizeof(*caminho));
    if (!caminho) { puts("Memoria insuficiente."); return; }
    v = destino;
    for (size_t i = tamanho; i > 0; --i) {
        caminho[i - 1] = v;
        if (v != origem) v = pred[v];
    }
    for (size_t i = 0; i < tamanho; ++i) printf("%s%d", i ? " -> " : "", caminho[i]);
    putchar('\n');
    free(caminho);
}
