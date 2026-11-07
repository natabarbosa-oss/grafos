#ifndef COLORACAO_H
#define COLORACAO_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

GrafoLista *criar_grafo_lista(int n);
void inserir_aresta_lista(GrafoLista *g, int u, int v);
void liberar_grafo_lista(GrafoLista *g);

int *coloracao_gulosa(GrafoLista *g, int *num_cores);
int *coloracao_welsh_powell(GrafoLista *g, int *num_cores);
int eh_bipartido(GrafoLista *g);

#endif