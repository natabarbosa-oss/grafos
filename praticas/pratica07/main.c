#include "bellman_ford.h"

#include <assert.h>
#include <stdio.h>

static GrafoListaPonderado *criar_grafo_com_arestas(
    int n, const int arestas[][3], size_t quantidade)
{
    GrafoListaPonderado *g = criar_grafo_lista_ponderado(n);
    size_t i;

    assert(g);
    for (i = 0; i < quantidade; ++i) {
        assert(inserir_aresta_ponderada(g, arestas[i][0], arestas[i][1],
                                       arestas[i][2]));
    }
    return g;
}

static void verificar_distancias(const int *obtidas, const int *esperadas, int n)
{
    int i;

    for (i = 0; i < n; ++i) {
        assert(obtidas[i] == esperadas[i]);
    }
}

static void testar_pesos_positivos(void)
{
    const int arestas[][3] = {
        {0, 1, 4}, {0, 2, 5}, {1, 2, 1}, {1, 3, 2}, {2, 3, 3}
    };
    const int esperado[] = {0, 4, 5, 6};
    int dist[4];
    int pred[4];
    GrafoListaPonderado *g = criar_grafo_com_arestas(
        4, arestas, sizeof(arestas) / sizeof(arestas[0]));

    assert(bellman_ford(g, 0, dist, pred) == 1);
    verificar_distancias(dist, esperado, 4);
    assert(pred[0] == -1 && pred[1] == 0 && pred[2] == 0 && pred[3] == 1);
    puts("1. Pesos positivos: retorno 1 e distancias [0, 4, 5, 6].");
    liberar_grafo_lista_ponderado(g);
}

static void testar_pesos_negativos_sem_ciclo(void)
{
    const int arestas[][3] = {
        {0, 1, 4}, {0, 2, 5}, {1, 2, -2}, {2, 3, 3}, {1, 3, 4}
    };
    const int esperado[] = {0, 4, 2, 5};
    int dist[4];
    int pred[4];
    GrafoListaPonderado *g = criar_grafo_com_arestas(
        4, arestas, sizeof(arestas) / sizeof(arestas[0]));

    assert(bellman_ford(g, 0, dist, pred) == 1);
    verificar_distancias(dist, esperado, 4);
    assert(pred[0] == -1 && pred[1] == 0 && pred[2] == 1 && pred[3] == 2);
    puts("2. Pesos negativos sem ciclo: retorno 1 e distancias [0, 4, 2, 5].");
    liberar_grafo_lista_ponderado(g);
}

static void testar_ciclo_negativo_alcancavel(void)
{
    const int arestas[][3] = {
        {0, 1, 1}, {1, 2, -1}, {2, 3, -1}, {3, 1, -1}
    };
    int dist[4];
    int pred[4];
    GrafoListaPonderado *g = criar_grafo_com_arestas(
        4, arestas, sizeof(arestas) / sizeof(arestas[0]));

    puts("3. Ciclo negativo alcancavel (reconstrucao esperada abaixo):");
    assert(bellman_ford(g, 0, dist, pred) == 0);
    assert(pred[1] == 3 && pred[2] == 1 && pred[3] == 2);
    puts("   Retorno 0 confirmado.");
    liberar_grafo_lista_ponderado(g);
}

static void testar_ciclo_negativo_inalcancavel(void)
{
    const int arestas[][3] = {
        {0, 1, 2}, {1, 2, 3}, {3, 4, -2}, {4, 3, -2}
    };
    const int esperado[] = {0, 2, 5, INT_MAX, INT_MAX};
    int dist[5];
    int pred[5];
    GrafoListaPonderado *g = criar_grafo_com_arestas(
        5, arestas, sizeof(arestas) / sizeof(arestas[0]));

    assert(bellman_ford(g, 0, dist, pred) == 1);
    verificar_distancias(dist, esperado, 5);
    assert(pred[3] == -1 && pred[4] == -1);
    puts("4. Ciclo negativo inalcancavel: retorno 1; ciclo nao reportado.");
    liberar_grafo_lista_ponderado(g);
}

static void testar_vertices_desconectados(void)
{
    const int arestas[][3] = {{0, 1, 0}, {1, 2, 7}, {3, 4, 1}};
    const int esperado[] = {0, 0, 7, INT_MAX, INT_MAX, INT_MAX};
    int dist[6];
    int pred[6];
    GrafoListaPonderado *g = criar_grafo_com_arestas(
        6, arestas, sizeof(arestas) / sizeof(arestas[0]));

    assert(bellman_ford(g, 0, dist, pred) == 1);
    verificar_distancias(dist, esperado, 6);
    assert(pred[3] == -1 && pred[4] == -1 && pred[5] == -1);
    puts("5. Vertices desconectados: distancias [0, 0, 7, INF, INF, INF].");
    liberar_grafo_lista_ponderado(g);
}

int main(void)
{
    testar_pesos_positivos();
    testar_pesos_negativos_sem_ciclo();
    testar_ciclo_negativo_alcancavel();
    testar_ciclo_negativo_inalcancavel();
    testar_vertices_desconectados();
    puts("Todos os testes da pratica 07 passaram.");
    return 0;
}
