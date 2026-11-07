#include "hamiltoniano.h"
#include "tps.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void imprimir_rota(const int *rota, int quantidade)
{
    int i;

    for (i = 0; i < quantidade; ++i) {
        printf("%s%d", i == 0 ? "" : " -> ", rota[i]);
    }
}

static void testar_hamiltoniano(void)
{
    const int ciclo_c5[5 * 5] = {
        0, 1, 0, 0, 1,
        1, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 1, 0, 1,
        1, 0, 0, 1, 0
    };
    const int caminho_p5[5 * 5] = {
        0, 1, 0, 0, 0,
        1, 0, 1, 0, 0,
        0, 1, 0, 1, 0,
        0, 0, 1, 0, 1,
        0, 0, 0, 1, 0
    };
    const int desconexo[4 * 4] = {
        0, 1, 0, 0,
        1, 0, 0, 0,
        0, 0, 0, 1,
        0, 0, 1, 0
    };
    int resultado[6];
    int retorno;

    puts("1. Grafo com caminho e ciclo Hamiltoniano (C5)");
    retorno = encontrar_caminho_hamiltoniano_bt(5, ciclo_c5, 0, resultado);
    assert(retorno == 1);
    assert(caminho_hamiltoniano_valido(5, ciclo_c5, resultado, 0));
    printf("   Caminho: ");
    imprimir_rota(resultado, 5);
    putchar('\n');
    retorno = encontrar_caminho_hamiltoniano_bt(5, ciclo_c5, 1, resultado);
    assert(retorno == 1);
    assert(caminho_hamiltoniano_valido(5, ciclo_c5, resultado, 1));
    printf("   Ciclo:   ");
    imprimir_rota(resultado, 6);
    putchar('\n');

    puts("2. Grafo com caminho, mas sem ciclo Hamiltoniano (P5)");
    retorno = encontrar_caminho_hamiltoniano_bt(5, caminho_p5, 0, resultado);
    assert(retorno == 1);
    assert(caminho_hamiltoniano_valido(5, caminho_p5, resultado, 0));
    printf("   Caminho: ");
    imprimir_rota(resultado, 5);
    putchar('\n');
    assert(encontrar_caminho_hamiltoniano_bt(5, caminho_p5, 1,
                                             resultado) == 0);
    puts("   Ciclo: inexistente");

    puts("3. Grafo sem caminho Hamiltoniano (duas componentes)");
    assert(encontrar_caminho_hamiltoniano_bt(4, desconexo, 0,
                                             resultado) == 0);
    assert(encontrar_caminho_hamiltoniano_bt(4, desconexo, 1,
                                             resultado) == 0);
    puts("   Caminho e ciclo: inexistentes");
}

static void testar_tsp_conhecido(void)
{
    const int pesos[4 * 4] = {
         0, 10, 15, 20,
        10,  0, 35, 25,
        15, 35,  0, 30,
        20, 25, 30,  0
    };
    int exata[5];
    int vizinho[5];
    int insercao[5];
    long long custo_exato;
    long long custo_vizinho;
    long long custo_insercao;

    assert(tsp_forca_bruta(4, pesos, 0, exata, &custo_exato) == 1);
    assert(tsp_vizinho_mais_proximo(4, pesos, 0, vizinho,
                                    &custo_vizinho) == 1);
    assert(tsp_insercao(4, pesos, 0, insercao, &custo_insercao) == 1);
    assert(custo_exato == 80);
    assert(tsp_circuito_valido(4, pesos, 0, exata, custo_exato));
    assert(tsp_circuito_valido(4, pesos, 0, vizinho, custo_vizinho));
    assert(tsp_circuito_valido(4, pesos, 0, insercao, custo_insercao));
    assert(custo_vizinho >= custo_exato && custo_insercao >= custo_exato);

    puts("4 e 5. TSP conhecido e comparacao das abordagens");
    printf("   Forca bruta:             ");
    imprimir_rota(exata, 5);
    printf(" | custo = %lld (otimo conhecido)\n", custo_exato);
    printf("   Vizinho mais proximo:    ");
    imprimir_rota(vizinho, 5);
    printf(" | custo = %lld\n", custo_vizinho);
    printf("   Insercao mais proxima:   ");
    imprimir_rota(insercao, 5);
    printf(" | custo = %lld\n", custo_insercao);
}

static void testar_rotas_impossiveis(void)
{
    const int x = TSP_SEM_ARESTA;
    const int pesos[3 * 3] = {
        0, 1, x,
        1, 0, x,
        x, x, 0
    };
    int circuito[4];
    long long custo;

    assert(tsp_forca_bruta(3, pesos, 0, circuito, &custo) == 0);
    assert(tsp_vizinho_mais_proximo(3, pesos, 0, circuito, &custo) == 0);
    assert(tsp_insercao(3, pesos, 0, circuito, &custo) == 0);
    puts("   Rotas impossiveis: corretamente rejeitadas pelos tres metodos");
}

static void testar_reducao(void)
{
    const int ciclo_c4[4 * 4] = {
        0, 1, 0, 1,
        1, 0, 1, 0,
        0, 1, 0, 1,
        1, 0, 1, 0
    };
    const int caminho_p4[4 * 4] = {
        0, 1, 0, 0,
        1, 0, 1, 0,
        0, 1, 0, 1,
        0, 0, 1, 0
    };
    int tsp_ciclo[4 * 4];
    int tsp_caminho[4 * 4];
    int circuito[5];
    long long custo;
    int u;
    int v;

    assert(grafo_para_tsp(4, ciclo_c4, tsp_ciclo) == 1);
    assert(grafo_para_tsp(4, caminho_p4, tsp_caminho) == 1);
    for (u = 0; u < 4; ++u) {
        for (v = 0; v < 4; ++v) {
            if (u != v) {
                assert(tsp_ciclo[u * 4 + v] ==
                       (ciclo_c4[u * 4 + v] ? 1 : 2));
                assert(tsp_caminho[u * 4 + v] ==
                       (caminho_p4[u * 4 + v] ? 1 : 2));
            }
        }
    }
    assert(tsp_forca_bruta(4, tsp_ciclo, 0, circuito, &custo) == 1);
    assert(custo == 4);
    assert(tsp_forca_bruta(4, tsp_caminho, 0, circuito, &custo) == 1);
    assert(custo > 4);
    puts("6. Reducao Grafo -> TSP com pesos 1 e 2");
    puts("   C4 tem ciclo Hamiltoniano: custo otimo TSP = n = 4");
    printf("   P4 nao tem ciclo Hamiltoniano: custo otimo TSP = %lld > n\n",
           custo);
}

static uint32_t proximo_aleatorio(uint32_t *estado)
{
    *estado = *estado * UINT32_C(1664525) + UINT32_C(1013904223);
    return *estado;
}

static int *gerar_tsp_completo(int n, uint32_t semente)
{
    int *pesos = malloc((size_t)n * (size_t)n * sizeof(*pesos));
    int u;
    int v;

    if (!pesos) {
        return NULL;
    }
    for (u = 0; u < n; ++u) {
        pesos[u * n + u] = 0;
        for (v = u + 1; v < n; ++v) {
            int peso = 1 + (int)(proximo_aleatorio(&semente) % 99U);

            pesos[u * n + v] = peso;
            pesos[v * n + u] = peso;
        }
    }
    return pesos;
}

static double agora(void)
{
    struct timespec instante;

    assert(timespec_get(&instante, TIME_UTC) == TIME_UTC);
    return (double)instante.tv_sec + (double)instante.tv_nsec / 1000000000.0;
}

static void executar_experimento(void)
{
    const int tamanhos[] = {5, 8, 10, 12};
    const uint32_t sementes[] = {
        UINT32_C(1005), UINT32_C(1008), UINT32_C(1010), UINT32_C(1012)
    };
    size_t caso;

    puts("\nExperimento reproduzivel");
    puts("Instancias completas, simetricas, pesos inteiros 1..99, LCG e sementes fixas.");
    puts("+----+------------+---------+---------------+---------+---------------+---------+---------------+");
    puts("| n  | otimo (FB) | FB (s)  | vizinho/custo | viz (s) | insercao/custo| ins (s) | gaps V/I      |");
    puts("+----+------------+---------+---------------+---------+---------------+---------+---------------+");

    for (caso = 0; caso < sizeof(tamanhos) / sizeof(tamanhos[0]); ++caso) {
        int n = tamanhos[caso];
        int *pesos = gerar_tsp_completo(n, sementes[caso]);
        int *exata = malloc(((size_t)n + 1U) * sizeof(*exata));
        int *vizinho = malloc(((size_t)n + 1U) * sizeof(*vizinho));
        int *insercao = malloc(((size_t)n + 1U) * sizeof(*insercao));
        long long custo_exato;
        long long custo_vizinho;
        long long custo_insercao;
        double inicio;
        double tempo_exato;
        double tempo_vizinho;
        double tempo_insercao;
        double gap_vizinho;
        double gap_insercao;

        assert(pesos && exata && vizinho && insercao);
        inicio = agora();
        assert(tsp_forca_bruta(n, pesos, 0, exata, &custo_exato) == 1);
        tempo_exato = agora() - inicio;
        inicio = agora();
        assert(tsp_vizinho_mais_proximo(n, pesos, 0, vizinho,
                                        &custo_vizinho) == 1);
        tempo_vizinho = agora() - inicio;
        inicio = agora();
        assert(tsp_insercao(n, pesos, 0, insercao, &custo_insercao) == 1);
        tempo_insercao = agora() - inicio;

        assert(tsp_circuito_valido(n, pesos, 0, exata, custo_exato));
        assert(tsp_circuito_valido(n, pesos, 0, vizinho, custo_vizinho));
        assert(tsp_circuito_valido(n, pesos, 0, insercao,
                                   custo_insercao));
        assert(custo_vizinho >= custo_exato && custo_insercao >= custo_exato);
        gap_vizinho = 100.0 * (double)(custo_vizinho - custo_exato) /
                       (double)custo_exato;
        gap_insercao = 100.0 * (double)(custo_insercao - custo_exato) /
                       (double)custo_exato;
        printf("| %2d | %10lld | %.6f | %13lld | %.6f | %13lld | %.6f | %5.1f%%/%5.1f%% |\n",
               n, custo_exato, tempo_exato, custo_vizinho, tempo_vizinho,
               custo_insercao, tempo_insercao, gap_vizinho, gap_insercao);

        free(pesos);
        free(exata);
        free(vizinho);
        free(insercao);
    }
    puts("+----+------------+---------+---------------+---------+---------------+---------+---------------+");
}

int main(void)
{
    puts("Testes de correcao da Pratica 10");
    testar_hamiltoniano();
    testar_tsp_conhecido();
    testar_rotas_impossiveis();
    testar_reducao();
    puts("Todos os testes de correcao passaram.");
    executar_experimento();
    return 0;
}
