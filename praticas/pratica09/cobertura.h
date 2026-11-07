#ifndef COBERTURA_H
#define COBERTURA_H

typedef struct {
    int u;
    int v;
} Aresta;

/*
 * Os vetores cobertura devem possuir espaco para n inteiros.
 * As funcoes de construcao retornam a quantidade de vertices escolhidos ou
 * -1 quando a entrada for invalida ou ocorrer falta de memoria.
 */
int cobertura_minima_bt(int n, const Aresta *arestas, int quantidade_arestas,
                        int *cobertura);
int cobertura_gulosa(int n, const Aresta *arestas, int quantidade_arestas,
                     int *cobertura);
int eh_cobertura(int n, const Aresta *arestas, int quantidade_arestas,
                 const int *cobertura, int tamanho_cobertura);

#endif
