#ifndef AGM_H
#define AGM_H

typedef struct {
    int *pai, *rank;
    int n;
} DSU;

typedef struct {
    int u, v, peso;
} Aresta;

int inicializar_dsu(DSU *d, int n);
void liberar_dsu(DSU *d);
int find(DSU *d, int x);
void unir(DSU *d, int x, int y);

/*
 * resultado deve possuir espaco para, no maximo, n - 1 arestas.
 * As funcoes retornam o numero de componentes da floresta minima ou -1 em
 * caso de entrada invalida/falta de memoria. Em um grafo conexo, retornam 1.
 */
int kruskal(int n, const Aresta *arestas, int quantidade_arestas,
            Aresta *resultado, int *quantidade_resultado,
            long long *peso_total);
int prim(int n, const Aresta *arestas, int quantidade_arestas,
         Aresta *resultado, int *quantidade_resultado,
         long long *peso_total);

#endif
