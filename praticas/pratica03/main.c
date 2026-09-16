#include <stdio.h>
#include <stdlib.h>

#include "dag.h"

static void imprimir_ordem(const char *nome, int *ordem, int tamanho)
{
    int i;

    printf("%s: ", nome);

    if (ordem == NULL) {
        printf("impossivel (grafo possui ciclo)\n");
        return;
    }

    for (i = 0; i < tamanho; i++) {
        printf("%d", ordem[i]);

        if (i < tamanho - 1) {
            printf(" -> ");
        }
    }

    printf("\n");
}

int main(void)
{
    GrafoLista *g;
    int *ordem_kahn;
    int *ordem_dfs;
    int tamanho_kahn;
    int tamanho_dfs;

    g = criar_grafo_lista(6);

    if (g == NULL) {
        return 1;
    }

    inserir_aresta_lista(g, 5, 2);
    inserir_aresta_lista(g, 5, 0);
    inserir_aresta_lista(g, 4, 0);
    inserir_aresta_lista(g, 4, 1);
    inserir_aresta_lista(g, 2, 3);
    inserir_aresta_lista(g, 3, 1);

    printf("Eh DAG? %s\n", eh_dag(g) ? "Sim" : "Nao");

    ordem_kahn = ordenacao_topologica_kahn(g, &tamanho_kahn);
    ordem_dfs = ordenacao_topologica_dfs(g, &tamanho_dfs);

    imprimir_ordem("Ordenacao topologica - Kahn", ordem_kahn, tamanho_kahn);
    imprimir_ordem("Ordenacao topologica - DFS", ordem_dfs, tamanho_dfs);

    free(ordem_kahn);
    free(ordem_dfs);

    printf("\nAdicionando aresta 1 -> 5 para criar um ciclo...\n");

    inserir_aresta_lista(g, 1, 5);

    printf("Eh DAG? %s\n", eh_dag(g) ? "Sim" : "Nao");

    ordem_kahn = ordenacao_topologica_kahn(g, &tamanho_kahn);
    ordem_dfs = ordenacao_topologica_dfs(g, &tamanho_dfs);

    imprimir_ordem("Ordenacao topologica - Kahn", ordem_kahn, tamanho_kahn);
    imprimir_ordem("Ordenacao topologica - DFS", ordem_dfs, tamanho_dfs);

    free(ordem_kahn);
    free(ordem_dfs);

    liberar_grafo_lista(g);

    return 0;
}