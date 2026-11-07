#ifndef DIJKSTRA_H
#define DIJKSTRA_H

#include <limits.h>

typedef struct {
    int vertice;
    int distancia;
} NoHeap;

typedef struct {
    NoHeap *dados;
    int capacidade, tamanho;
    int *posicao;
} MinHeap;

typedef struct NoPonderado {
    int destino, peso;
    struct NoPonderado *prox;
} NoPonderado;

typedef struct {
    int n;
    NoPonderado **adj;
} GrafoListaPonderado;

MinHeap *criar_min_heap(int capacidade);
int inserir_min_heap(MinHeap *heap, int vertice, int distancia);
/* Heap vazio/invalido: retorna {-1, INT_MAX}. */
NoHeap extrair_minimo(MinHeap *heap);
void diminuir_chave(MinHeap *heap, int vertice, int distancia);
void liberar_min_heap(MinHeap *heap);

GrafoListaPonderado *criar_grafo_lista_ponderado(int n);
/* Insere uma aresta dirigida; retorna 0 para entrada invalida ou falta de memoria. */
int inserir_aresta_ponderada(GrafoListaPonderado *g, int origem, int destino, int peso);
void liberar_grafo_lista_ponderado(GrafoListaPonderado *g);
/* dist/pred devem ter g->n elementos. Custos representaveis: 0..INT_MAX-1.
 * INT_MAX representa inalcançavel ou custo nao representavel.
 * Erros com g/dist/pred validos deixam dist=INT_MAX e pred=-1. */
void dijkstra(GrafoListaPonderado *g, int origem, int *dist, int *pred);
/* Imprime o caminho. Sem parametro de tamanho, origem/destino e todos os
 * predecessores nao negativos devem ser indices validos do vetor pred. */
void reconstruir_caminho(int origem, int destino, int *pred);

#endif
