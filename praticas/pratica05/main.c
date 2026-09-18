#include "coloracao.h"

#include <stdio.h>
#include <stdlib.h>

static void mostrar_coloracao(const char *nome, const int cores[],
                              int n, int num_cores)
{
    int i;

    printf("%s (%d cores):", nome, num_cores);

    for (i = 0; i < n; i++) {
        printf(" %d:%d", i, cores[i]);
    }

    printf("\n");
}

int main(void)
{
    GrafoLista *g;
    int *cores_gulosa;
    int *cores_welsh;
    int num_cores_gulosa;
    int num_cores_welsh;

    g = criar_grafo_lista(5);

    if (g == NULL) {
        return EXIT_FAILURE;
    }

    inserir_aresta_lista(g, 0, 1);
    inserir_aresta_lista(g, 0, 2);
    inserir_aresta_lista(g, 1, 2);
    inserir_aresta_lista(g, 1, 3);
    inserir_aresta_lista(g, 2, 3);
    inserir_aresta_lista(g, 3, 4);

    cores_gulosa = coloracao_gulosa(g, &num_cores_gulosa);
    cores_welsh = coloracao_welsh_powell(g, &num_cores_welsh);

    if (cores_gulosa == NULL || cores_welsh == NULL) {
        free(cores_gulosa);
        free(cores_welsh);
        liberar_grafo_lista(g);
        return EXIT_FAILURE;
    }

    mostrar_coloracao("Gulosa", cores_gulosa,
                      g->n, num_cores_gulosa);

    mostrar_coloracao("Welsh-Powell", cores_welsh,
                      g->n, num_cores_welsh);

    printf("Bipartido: %s\n",
           eh_bipartido(g) ? "sim" : "nao");

    free(cores_gulosa);
    free(cores_welsh);
    liberar_grafo_lista(g);

    return EXIT_SUCCESS;
}