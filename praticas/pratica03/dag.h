#ifndef DAG_H
#define DAG_H

typedef struct No {
    int destino;
    struct No *prox;
} No;

typedef struct {
    int n;
    No **adj;
} GrafoLista;

GrafoLista *criar_grafo_lista(int n);
int inserir_aresta_lista(GrafoLista *g, int u, int v);
void liberar_grafo_lista(GrafoLista *g);

int *ordenacao_topologica_kahn(GrafoLista *g, int *tamanho);
int *ordenacao_topologica_dfs(GrafoLista *g, int *tamanho);
int eh_dag(GrafoLista *g);

#endif