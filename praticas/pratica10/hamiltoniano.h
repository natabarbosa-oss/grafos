#ifndef HAMILTONIANO_H
#define HAMILTONIANO_H

/*
 * A matriz de adjacencias deve ter n * n inteiros, em ordem de linhas.
 * O vetor resultado deve comportar n vertices para caminho e n + 1 para
 * ciclo. No ciclo, o primeiro vertice e repetido na ultima posicao.
 *
 * buscar_ciclo == 0 procura um caminho; qualquer outro valor procura ciclo.
 * Retorna 1 quando encontra, 0 quando nao existe e -1 para entrada invalida
 * ou falha de alocacao.
 */
int encontrar_caminho_hamiltoniano_bt(int n, const int *adjacencias,
                                      int buscar_ciclo, int *resultado);

/* Confere vertices distintos, adjacencias consecutivas e o retorno no ciclo. */
int caminho_hamiltoniano_valido(int n, const int *adjacencias,
                                const int *caminho, int eh_ciclo);

#endif
