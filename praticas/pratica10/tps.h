#ifndef TPS_H
#define TPS_H

#include <limits.h>

#define TSP_SEM_ARESTA INT_MAX

/*
 * As matrizes possuem n * n inteiros em ordem de linhas. Peso negativo ou
 * TSP_SEM_ARESTA representa aresta inexistente. O circuito comporta n + 1
 * vertices e repete a origem na ultima posicao.
 * As funcoes retornam 1 quando encontram circuito, 0 quando ele e impossivel
 * e -1 para entrada invalida ou falha de alocacao.
 */
int tsp_forca_bruta(int n, const int *pesos, int origem,
                    int *circuito, long long *custo_total);
int tsp_vizinho_mais_proximo(int n, const int *pesos, int origem,
                             int *circuito, long long *custo_total);
int tsp_insercao(int n, const int *pesos, int origem,
                 int *circuito, long long *custo_total);

/* Constroi o grafo completo: aresta original recebe 1; inexistente recebe 2. */
int grafo_para_tsp(int n, const int *adjacencias, int *pesos_tsp);

int tsp_circuito_valido(int n, const int *pesos, int origem,
                        const int *circuito, long long custo_total);

#endif
