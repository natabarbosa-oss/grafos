#include "agm.h"

#include <limits.h>
#include <stdint.h>
#include <stdlib.h>

typedef struct NoAdjacente {
    int destino;
    int peso;
    struct NoAdjacente *proximo;
} NoAdjacente;

typedef struct {
    int *vertices;
    int *posicao;
    long long *chave;
    int tamanho;
} MinHeap;

int inicializar_dsu(DSU *d, int n)
{
    int i;

    if (!d || n <= 0 || (size_t)n > SIZE_MAX / sizeof(int)) {
        return 0;
    }
    d->pai = malloc((size_t)n * sizeof(*d->pai));
    d->rank = calloc((size_t)n, sizeof(*d->rank));
    d->n = 0;
    if (!d->pai || !d->rank) {
        free(d->pai);
        free(d->rank);
        d->pai = NULL;
        d->rank = NULL;
        return 0;
    }
    d->n = n;
    for (i = 0; i < n; ++i) {
        d->pai[i] = i;
    }
    return 1;
}

void liberar_dsu(DSU *d)
{
    if (!d) {
        return;
    }
    free(d->pai);
    free(d->rank);
    d->pai = NULL;
    d->rank = NULL;
    d->n = 0;
}

int find(DSU *d, int x)
{
    if (!d || !d->pai || !d->rank || x < 0 || x >= d->n) {
        return -1;
    }
    if (d->pai[x] != x) {
        d->pai[x] = find(d, d->pai[x]);
    }
    return d->pai[x];
}

void unir(DSU *d, int x, int y)
{
    int raiz_x = find(d, x);
    int raiz_y = find(d, y);

    if (raiz_x < 0 || raiz_y < 0 || raiz_x == raiz_y) {
        return;
    }
    if (d->rank[raiz_x] < d->rank[raiz_y]) {
        d->pai[raiz_x] = raiz_y;
    } else if (d->rank[raiz_x] > d->rank[raiz_y]) {
        d->pai[raiz_y] = raiz_x;
    } else {
        d->pai[raiz_y] = raiz_x;
        ++d->rank[raiz_x];
    }
}

static int comparar_arestas(const void *a, const void *b)
{
    const Aresta *aresta_a = a;
    const Aresta *aresta_b = b;

    if (aresta_a->peso != aresta_b->peso) {
        return aresta_a->peso < aresta_b->peso ? -1 : 1;
    }
    if (aresta_a->u != aresta_b->u) {
        return aresta_a->u < aresta_b->u ? -1 : 1;
    }
    if (aresta_a->v != aresta_b->v) {
        return aresta_a->v < aresta_b->v ? -1 : 1;
    }
    return 0;
}

static int entrada_valida(int n, const Aresta *arestas,
                          int quantidade_arestas, const Aresta *resultado,
                          const int *quantidade_resultado,
                          const long long *peso_total)
{
    int i;

    if (n <= 0 || quantidade_arestas < 0 || !quantidade_resultado ||
        !peso_total || (quantidade_arestas > 0 && !arestas) ||
        (n > 1 && !resultado)) {
        return 0;
    }
    for (i = 0; i < quantidade_arestas; ++i) {
        if (arestas[i].u < 0 || arestas[i].u >= n || arestas[i].v < 0 ||
            arestas[i].v >= n) {
            return 0;
        }
    }
    return 1;
}

int kruskal(int n, const Aresta *arestas, int quantidade_arestas,
            Aresta *resultado, int *quantidade_resultado,
            long long *peso_total)
{
    Aresta *ordenadas = NULL;
    DSU d = {NULL, NULL, 0};
    int componentes;
    int i;

    if (quantidade_resultado) {
        *quantidade_resultado = 0;
    }
    if (peso_total) {
        *peso_total = 0;
    }
    if (!entrada_valida(n, arestas, quantidade_arestas, resultado,
                        quantidade_resultado, peso_total) ||
        (size_t)quantidade_arestas > SIZE_MAX / sizeof(*ordenadas)) {
        return -1;
    }
    if (quantidade_arestas > 0) {
        ordenadas = malloc((size_t)quantidade_arestas * sizeof(*ordenadas));
        if (!ordenadas) {
            return -1;
        }
        for (i = 0; i < quantidade_arestas; ++i) {
            ordenadas[i] = arestas[i];
        }
        qsort(ordenadas, (size_t)quantidade_arestas, sizeof(*ordenadas),
              comparar_arestas);
    }
    if (!inicializar_dsu(&d, n)) {
        free(ordenadas);
        return -1;
    }

    componentes = n;
    for (i = 0; i < quantidade_arestas && componentes > 1; ++i) {
        int raiz_u = find(&d, ordenadas[i].u);
        int raiz_v = find(&d, ordenadas[i].v);

        if (raiz_u != raiz_v) {
            resultado[*quantidade_resultado] = ordenadas[i];
            ++*quantidade_resultado;
            *peso_total += ordenadas[i].peso;
            unir(&d, raiz_u, raiz_v);
            --componentes;
        }
    }

    liberar_dsu(&d);
    free(ordenadas);
    return componentes;
}

static void trocar_heap(MinHeap *heap, int a, int b)
{
    int temporario = heap->vertices[a];

    heap->vertices[a] = heap->vertices[b];
    heap->vertices[b] = temporario;
    heap->posicao[heap->vertices[a]] = a;
    heap->posicao[heap->vertices[b]] = b;
}

static int menor_no_heap(const MinHeap *heap, int a, int b)
{
    int vertice_a = heap->vertices[a];
    int vertice_b = heap->vertices[b];

    if (heap->chave[vertice_a] != heap->chave[vertice_b]) {
        return heap->chave[vertice_a] < heap->chave[vertice_b];
    }
    return vertice_a < vertice_b;
}

static void subir_heap(MinHeap *heap, int indice)
{
    while (indice > 0) {
        int pai = (indice - 1) / 2;

        if (menor_no_heap(heap, pai, indice)) {
            break;
        }
        trocar_heap(heap, pai, indice);
        indice = pai;
    }
}

static void descer_heap(MinHeap *heap, int indice)
{
    for (;;) {
        int esquerdo = 2 * indice + 1;
        int menor = indice;

        if (esquerdo < heap->tamanho && menor_no_heap(heap, esquerdo, menor)) {
            menor = esquerdo;
        }
        if (esquerdo + 1 < heap->tamanho &&
            menor_no_heap(heap, esquerdo + 1, menor)) {
            menor = esquerdo + 1;
        }
        if (menor == indice) {
            break;
        }
        trocar_heap(heap, indice, menor);
        indice = menor;
    }
}

static int extrair_minimo(MinHeap *heap)
{
    int minimo = heap->vertices[0];

    --heap->tamanho;
    heap->posicao[minimo] = -1;
    if (heap->tamanho > 0) {
        heap->vertices[0] = heap->vertices[heap->tamanho];
        heap->posicao[heap->vertices[0]] = 0;
        descer_heap(heap, 0);
    }
    return minimo;
}

static void diminuir_chave(MinHeap *heap, int vertice, long long nova_chave)
{
    int indice = heap->posicao[vertice];

    heap->chave[vertice] = nova_chave;
    subir_heap(heap, indice);
}

static void liberar_adjacencias(NoAdjacente **adjacencias, int n)
{
    int i;

    if (!adjacencias) {
        return;
    }
    for (i = 0; i < n; ++i) {
        NoAdjacente *atual = adjacencias[i];

        while (atual) {
            NoAdjacente *proximo = atual->proximo;
            free(atual);
            atual = proximo;
        }
    }
    free(adjacencias);
}

static int adicionar_adjacencia(NoAdjacente **adjacencias, int origem,
                                int destino, int peso)
{
    NoAdjacente *novo = malloc(sizeof(*novo));

    if (!novo) {
        return 0;
    }
    novo->destino = destino;
    novo->peso = peso;
    novo->proximo = adjacencias[origem];
    adjacencias[origem] = novo;
    return 1;
}

int prim(int n, const Aresta *arestas, int quantidade_arestas,
         Aresta *resultado, int *quantidade_resultado,
         long long *peso_total)
{
    NoAdjacente **adjacencias = NULL;
    MinHeap heap = {NULL, NULL, NULL, 0};
    int *pai = NULL;
    int componentes = 0;
    int i;

    if (quantidade_resultado) {
        *quantidade_resultado = 0;
    }
    if (peso_total) {
        *peso_total = 0;
    }
    if (!entrada_valida(n, arestas, quantidade_arestas, resultado,
                        quantidade_resultado, peso_total) ||
        (size_t)n > SIZE_MAX / sizeof(int) ||
        (size_t)n > SIZE_MAX / sizeof(long long) ||
        (size_t)n > SIZE_MAX / sizeof(*adjacencias)) {
        return -1;
    }

    adjacencias = calloc((size_t)n, sizeof(*adjacencias));
    heap.vertices = malloc((size_t)n * sizeof(*heap.vertices));
    heap.posicao = malloc((size_t)n * sizeof(*heap.posicao));
    heap.chave = malloc((size_t)n * sizeof(*heap.chave));
    pai = malloc((size_t)n * sizeof(*pai));
    if (!adjacencias || !heap.vertices || !heap.posicao || !heap.chave ||
        !pai) {
        goto erro;
    }
    for (i = 0; i < quantidade_arestas; ++i) {
        if (!adicionar_adjacencia(adjacencias, arestas[i].u, arestas[i].v,
                                  arestas[i].peso) ||
            !adicionar_adjacencia(adjacencias, arestas[i].v, arestas[i].u,
                                  arestas[i].peso)) {
            goto erro;
        }
    }
    heap.tamanho = n;
    for (i = 0; i < n; ++i) {
        heap.vertices[i] = i;
        heap.posicao[i] = i;
        heap.chave[i] = i == 0 ? 0 : LLONG_MAX;
        pai[i] = -1;
    }

    while (heap.tamanho > 0) {
        int u = extrair_minimo(&heap);
        NoAdjacente *vizinho;

        if (pai[u] == -1) {
            ++componentes;
        } else {
            resultado[*quantidade_resultado] =
                (Aresta){pai[u], u, (int)heap.chave[u]};
            ++*quantidade_resultado;
            *peso_total += heap.chave[u];
        }
        for (vizinho = adjacencias[u]; vizinho; vizinho = vizinho->proximo) {
            int v = vizinho->destino;

            if (heap.posicao[v] >= 0 && vizinho->peso < heap.chave[v]) {
                pai[v] = u;
                diminuir_chave(&heap, v, vizinho->peso);
            }
        }
    }

    liberar_adjacencias(adjacencias, n);
    free(heap.vertices);
    free(heap.posicao);
    free(heap.chave);
    free(pai);
    return componentes;

erro:
    liberar_adjacencias(adjacencias, n);
    free(heap.vertices);
    free(heap.posicao);
    free(heap.chave);
    free(pai);
    *quantidade_resultado = 0;
    *peso_total = 0;
    return -1;
}
