#include "agm.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static void validar_dsu(void)
{
    DSU d = {NULL, NULL, 0};
    int raiz;
    int i;

    assert(inicializar_dsu(&d, 7));
    for (i = 0; i < d.n; ++i) {
        assert(d.pai[i] == i);
        assert(d.rank[i] == 0);
    }
    unir(&d, 0, 1);
    unir(&d, 2, 3);
    unir(&d, 0, 2);
    unir(&d, 4, 5);
    unir(&d, 0, 4);
    raiz = find(&d, 5);
    assert(raiz == find(&d, 0));
    assert(d.pai[5] == raiz);
    assert(d.rank[raiz] == 2);
    assert(find(&d, 6) == 6);
    assert(find(&d, -1) == -1 && find(&d, 7) == -1);
    liberar_dsu(&d);
    assert(d.pai == NULL && d.rank == NULL && d.n == 0);
    puts("DSU: compressao de caminho e uniao por rank validadas.");
}

static void imprimir_resultado(const char *algoritmo, const Aresta *resultado,
                               int quantidade, long long peso,
                               int componentes)
{
    int i;

    printf("  %s: ", algoritmo);
    if (componentes == 1) {
        puts("arvore geradora minima");
    } else {
        printf("nao existe uma unica AGM; floresta minima (%d componentes)\n",
               componentes);
    }
    for (i = 0; i < quantidade; ++i) {
        printf("    %d -- %d (peso %d)\n", resultado[i].u,
               resultado[i].v, resultado[i].peso);
    }
    printf("    Custo total: %lld\n", peso);
}

static void testar_grafo(const char *nome, int n, const Aresta *arestas,
                         int quantidade_arestas, long long custo_esperado,
                         int componentes_esperados)
{
    size_t capacidade = n > 1 ? (size_t)(n - 1) : 1U;
    Aresta *resultado_kruskal = malloc(capacidade * sizeof(*resultado_kruskal));
    Aresta *resultado_prim = malloc(capacidade * sizeof(*resultado_prim));
    int quantidade_kruskal;
    int quantidade_prim;
    int componentes_kruskal;
    int componentes_prim;
    long long custo_kruskal;
    long long custo_prim;

    assert(resultado_kruskal && resultado_prim);
    componentes_kruskal =
        kruskal(n, arestas, quantidade_arestas, resultado_kruskal,
                &quantidade_kruskal, &custo_kruskal);
    componentes_prim =
        prim(n, arestas, quantidade_arestas, resultado_prim,
             &quantidade_prim, &custo_prim);

    assert(componentes_kruskal == componentes_esperados);
    assert(componentes_prim == componentes_esperados);
    assert(quantidade_kruskal == n - componentes_esperados);
    assert(quantidade_prim == n - componentes_esperados);
    assert(custo_kruskal == custo_esperado);
    assert(custo_prim == custo_esperado);
    assert(custo_kruskal == custo_prim);

    printf("\n%s\n", nome);
    imprimir_resultado("Kruskal", resultado_kruskal, quantidade_kruskal,
                       custo_kruskal, componentes_kruskal);
    imprimir_resultado("Prim", resultado_prim, quantidade_prim, custo_prim,
                       componentes_prim);
    free(resultado_kruskal);
    free(resultado_prim);
}

int main(void)
{
    const Aresta pequeno[] = {
        {0, 1, 1}, {0, 2, 4}, {1, 2, 2}, {1, 3, 5}, {2, 3, 3}
    };
    const Aresta repetidos[] = {
        {0, 1, 1}, {1, 2, 1}, {2, 3, 1},
        {3, 0, 1}, {0, 2, 2}, {1, 3, 2}
    };
    const Aresta negativos[] = {
        {0, 1, -4}, {1, 2, -2}, {0, 2, 3}, {2, 3, 1}, {1, 3, 5}
    };
    const Aresta ciclos[] = {
        {0, 1, 2}, {1, 2, 3}, {2, 3, 1}, {3, 4, 4},
        {4, 0, 8}, {0, 2, 6}, {1, 3, 5}, {2, 4, 7}
    };
    const Aresta desconectado[] = {
        {0, 1, 1}, {1, 2, 2}, {0, 2, 9}, {3, 4, -1}
    };

    validar_dsu();
    testar_grafo("1. Grafo pequeno com AGM conhecida", 4, pequeno,
                 (int)(sizeof(pequeno) / sizeof(pequeno[0])), 6, 1);
    testar_grafo("2. Grafo com pesos repetidos", 4, repetidos,
                 (int)(sizeof(repetidos) / sizeof(repetidos[0])), 3, 1);
    testar_grafo("3. Grafo com pesos negativos", 4, negativos,
                 (int)(sizeof(negativos) / sizeof(negativos[0])), -5, 1);
    testar_grafo("4. Grafo que possui ciclos", 5, ciclos,
                 (int)(sizeof(ciclos) / sizeof(ciclos[0])), 10, 1);
    testar_grafo("5. Grafo desconectado", 6, desconectado,
                 (int)(sizeof(desconectado) / sizeof(desconectado[0])), 2, 3);
    puts("\nTodos os testes da pratica 08 passaram.");
    return 0;
}
