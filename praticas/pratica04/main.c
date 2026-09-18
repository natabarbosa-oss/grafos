#include "conectividade.h"
#include "planaridade.h"

#include <stdio.h>
#include <stdlib.h>

static GrafoLista *criar_k33(void)
{
    GrafoLista *g = criar_grafo_lista(6);
    int u;
    int v;

    if (g == NULL) {
        return NULL;
    }

    for (u = 0; u < 3; u++) {
        for (v = 3; v < 6; v++) {
            if (!inserir_aresta_lista(g, u, v)) {
                liberar_grafo_lista(g);
                return NULL;
            }
        }
    }

    return g;
}

int main(void)
{
    GrafoLista *g = criar_grafo_lista(5);
    GrafoLista *k33;
    int *articulacoes;
    Aresta *pontes;
    int quantidade_pontes;
    int i;

    if (g == NULL || !inserir_aresta_lista(g, 0, 1) ||
        !inserir_aresta_lista(g, 1, 2) ||
        !inserir_aresta_lista(g, 2, 3) ||
        !inserir_aresta_lista(g, 3, 0) ||
        !inserir_aresta_lista(g, 2, 4)) {
        liberar_grafo_lista(g);
        return EXIT_FAILURE;
    }

    articulacoes = detectar_articulacoes(g);
    pontes = detectar_pontes(g, &quantidade_pontes);

    if (articulacoes == NULL ||
        (quantidade_pontes > 0 && pontes == NULL)) {
        free(articulacoes);
        free(pontes);
        liberar_grafo_lista(g);
        return EXIT_FAILURE;
    }

    printf("Ciclo com ramo - articulacoes:");

    for (i = 0; i < g->n; i++) {
        if (articulacoes[i]) {
            printf(" %d", i);
        }
    }

    printf("\nPontes:");

    for (i = 0; i < quantidade_pontes; i++) {
        printf(" %d--%d", pontes[i].u, pontes[i].v);
    }

    printf("\nEuler: %s; heuristica: %s\n",
           eh_planar_euler(g) ? "satisfaz" : "rejeita",
           eh_planar_heuristica(g) ? "sem obstrucao" : "nao planar");

    free(articulacoes);
    free(pontes);
    liberar_grafo_lista(g);

    k33 = criar_k33();

    if (k33 == NULL) {
        return EXIT_FAILURE;
    }

    printf("K3,3 - Euler: %s; heuristica: %s\n",
           eh_planar_euler(k33) ? "satisfaz" : "rejeita",
           eh_planar_heuristica(k33) ? "sem obstrucao" : "nao planar");

    liberar_grafo_lista(k33);

    return EXIT_SUCCESS;
}