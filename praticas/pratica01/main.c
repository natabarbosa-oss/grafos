#include <stdio.h>
#include "grafo_matriz.h"
#include "grafo_lista.h"

int main(void) {
    GrafoMatriz *matriz = criar_grafo_matriz(5);
    GrafoLista *lista = criar_grafo_lista(5);

    if (matriz == NULL || lista == NULL) {
        liberar_grafo_matriz(matriz);
        liberar_grafo_lista(lista);
        return 1;
    }

    inserir_aresta_matriz(matriz, 0, 1);
    inserir_aresta_matriz(matriz, 0, 2);
    inserir_aresta_matriz(matriz, 1, 3);
    inserir_aresta_matriz(matriz, 3, 4);

    inserir_aresta_lista(lista, 0, 1);
    inserir_aresta_lista(lista, 0, 2);
    inserir_aresta_lista(lista, 1, 3);
    inserir_aresta_lista(lista, 3, 4);

    printf("Matriz: grau do vertice 0 = %d\n",
           grau_matriz(matriz, 0));

    printf("Matriz: 0 e 2 adjacentes = %d\n",
           sao_adjacentes_matriz(matriz, 0, 2));

    printf("Lista: grau do vertice 0 = %d\n",
           grau_lista(lista, 0));

    printf("Lista: 0 e 2 adjacentes = %d\n",
           sao_adjacentes_lista(lista, 0, 2));

    remover_aresta_matriz(matriz, 0, 2);
    remover_aresta_lista(lista, 0, 2);

    printf("Matriz: 0 e 2 apos remocao = %d\n",
           sao_adjacentes_matriz(matriz, 0, 2));

    printf("Lista: 0 e 2 apos remocao = %d\n",
           sao_adjacentes_lista(lista, 0, 2));

    liberar_grafo_matriz(matriz);
    liberar_grafo_lista(lista);

    return 0;
}