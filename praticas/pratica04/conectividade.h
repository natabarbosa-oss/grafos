#ifndef CONECTIVIDADE_H
#define CONECTIVIDADE_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

typedef struct {
    int u;
    int v;
} Aresta;

GrafoLista *criar_grafo_lista(int n);
int inserir_aresta_lista(GrafoLista *g, int u, int v);
void liberar_grafo_lista(GrafoLista *g);

void dfs_articulacoes(GrafoLista *g, int u, int *tempo,
                      int descoberta[], int low[], int pai[],
                      int articulacao[]);
int *detectar_articulacoes(GrafoLista *g);
Aresta *detectar_pontes(GrafoLista *g, int *quantidade);

#endif