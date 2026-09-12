#include <stdio.h>
#include <stdlib.h>

#include "grafo_lista.h"
#include "busca_largura.h"
#include "busca_profundidade.h"

int main(void)
{
    GrafoLista *g;
    int *dist;
    int *pred;
    int *visitado;
    int *tempo_entrada;
    int *tempo_saida;
    int tempo = 0;
    int i;

    g = criar_grafo_lista(6);

    if (g == NULL) {
        return 1;
    }

    adicionar_aresta_lista(g, 0, 1);
    adicionar_aresta_lista(g, 0, 2);
    adicionar_aresta_lista(g, 1, 3);
    adicionar_aresta_lista(g, 2, 3);
    adicionar_aresta_lista(g, 4, 5);

    printf("Grafo:\n");
    imprimir_grafo_lista(g);

    dist = malloc((size_t)g->n * sizeof(*dist));
    pred = malloc((size_t)g->n * sizeof(*pred));

    if (dist == NULL || pred == NULL) {
        liberar_grafo_lista(g);
        free(dist);
        free(pred);
        return 1;
    }

    bfs(g, 0, dist, pred);

    printf("\nBFS a partir do vertice 0:\n");

    for (i = 0; i < g->n; i++) {
        printf("Vertice %d: distancia = %d, predecessor = %d\n",
               i, dist[i], pred[i]);
    }

    visitado = calloc((size_t)g->n, sizeof(*visitado));
    tempo_entrada = malloc((size_t)g->n * sizeof(*tempo_entrada));
    tempo_saida = malloc((size_t)g->n * sizeof(*tempo_saida));

    if (visitado == NULL || tempo_entrada == NULL || tempo_saida == NULL) {
        liberar_grafo_lista(g);
        free(dist);
        free(pred);
        free(visitado);
        free(tempo_entrada);
        free(tempo_saida);
        return 1;
    }

    dfs_recursiva(g, 0, visitado, tempo_entrada, tempo_saida, &tempo);

    printf("\nDFS a partir do vertice 0:\n");

    for (i = 0; i < g->n; i++) {
        if (visitado[i]) {
            printf("Vertice %d: entrada = %d, saida = %d\n",
                   i, tempo_entrada[i], tempo_saida[i]);
        }
    }

    printf("\nNumero de componentes conexas: %d\n",
           contar_componentes(g));

    printf("Possui ciclo? %s\n",
           tem_ciclo(g) ? "Sim" : "Nao");

    printf("Eh bipartido? %s\n",
           eh_bipartido(g) ? "Sim" : "Nao");

    free(dist);
    free(pred);
    free(visitado);
    free(tempo_entrada);
    free(tempo_saida);
    liberar_grafo_lista(g);

    return 0;
}