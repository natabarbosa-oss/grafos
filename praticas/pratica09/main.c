#include "cobertura.h"

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void imprimir_cobertura(const int *cobertura, int tamanho)
{
    int i;

    putchar('{');
    for (i = 0; i < tamanho; ++i) {
        printf("%s%d", i == 0 ? "" : ", ", cobertura[i]);
    }
    putchar('}');
}

static void testar_grafo(const char *nome, int n, const Aresta *arestas,
                         int quantidade_arestas, int otimo_esperado,
                         int guloso_esperado)
{
    int *exata = malloc((n > 0 ? (size_t)n : 1U) * sizeof(*exata));
    int *gulosa = malloc((n > 0 ? (size_t)n : 1U) * sizeof(*gulosa));
    int tamanho_exata;
    int tamanho_gulosa;

    assert(exata && gulosa);
    tamanho_exata = cobertura_minima_bt(n, arestas, quantidade_arestas, exata);
    tamanho_gulosa = cobertura_gulosa(n, arestas, quantidade_arestas, gulosa);

    assert(tamanho_exata == otimo_esperado);
    assert(tamanho_gulosa == guloso_esperado);
    assert(eh_cobertura(n, arestas, quantidade_arestas, exata,
                        tamanho_exata));
    assert(eh_cobertura(n, arestas, quantidade_arestas, gulosa,
                        tamanho_gulosa));

    printf("%-43s BT = ", nome);
    imprimir_cobertura(exata, tamanho_exata);
    printf(" (%d), gulosa = ", tamanho_exata);
    imprimir_cobertura(gulosa, tamanho_gulosa);
    printf(" (%d)\n", tamanho_gulosa);

    free(exata);
    free(gulosa);
}

static void executar_testes(void)
{
    const Aresta estrela[] = {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}};
    const Aresta completo[] = {
        {0, 1}, {0, 2}, {0, 3}, {1, 2}, {1, 3}, {2, 3}
    };
    const Aresta desconexo[] = {
        {0, 1}, {1, 2}, {0, 2}, {3, 4}, {4, 5}, {3, 5}
    };
    const Aresta contraexemplo[] = {{0, 1}, {0, 2}, {1, 4}, {2, 3}};

    puts("Testes de correcao");
    testar_grafo("1. Grafo sem arestas", 5, NULL, 0, 0, 0);
    testar_grafo("2. Grafo estrela", 6, estrela,
                 (int)(sizeof(estrela) / sizeof(estrela[0])), 1, 1);
    testar_grafo("3. Grafo completo pequeno (K4)", 4, completo,
                 (int)(sizeof(completo) / sizeof(completo[0])), 3, 3);
    testar_grafo("4. Duas componentes desconexas (dois K3)", 6, desconexo,
                 (int)(sizeof(desconexo) / sizeof(desconexo[0])), 4, 4);
    testar_grafo("5. Contraexemplo para a heuristica", 5, contraexemplo,
                 (int)(sizeof(contraexemplo) / sizeof(contraexemplo[0])),
                 2, 3);
    puts("Todos os testes de correcao passaram.\n");
}

static uint32_t proximo_aleatorio(uint32_t *estado)
{
    *estado = *estado * UINT32_C(1664525) + UINT32_C(1013904223);
    return *estado;
}

static Aresta *gerar_grafo(int n, uint32_t semente, int *quantidade)
{
    int capacidade = n * (n - 1) / 2;
    Aresta *arestas = malloc((capacidade > 0 ? (size_t)capacidade : 1U) *
                             sizeof(*arestas));
    int total = 0;
    int u;
    int v;

    if (!arestas) {
        return NULL;
    }
    for (u = 0; u < n; ++u) {
        for (v = u + 1; v < n; ++v) {
            if (proximo_aleatorio(&semente) % 100U < 35U) {
                arestas[total++] = (Aresta){u, v};
            }
        }
    }
    *quantidade = total;
    return arestas;
}

static double segundos_decorridos(clock_t inicio, clock_t fim, int repeticoes)
{
    return (double)(fim - inicio) /
           ((double)CLOCKS_PER_SEC * (double)repeticoes);
}

static void executar_experimento(void)
{
    const int tamanhos[] = {5, 8, 10, 12, 15};
    const uint32_t sementes[] = {
        UINT32_C(9005), UINT32_C(9008), UINT32_C(9010),
        UINT32_C(9012), UINT32_C(9015)
    };
    const int repeticoes = 100;
    size_t caso;

    printf("Experimento reproduzivel (LCG, sementes fixas, p = 35%%; "
           "media de %d execucoes)\n", repeticoes);
    puts("+----+---------+----------------+----------------+--------+--------+----------+----------+");
    puts("| n  | arestas | tempo BT (s)   | tempo gul. (s)| tam BT | tam gul| val. BT  | val. gul.|");
    puts("+----+---------+----------------+----------------+--------+--------+----------+----------+");

    for (caso = 0; caso < sizeof(tamanhos) / sizeof(tamanhos[0]); ++caso) {
        int n = tamanhos[caso];
        int quantidade_arestas;
        Aresta *arestas = gerar_grafo(n, sementes[caso],
                                      &quantidade_arestas);
        int *exata = malloc((size_t)n * sizeof(*exata));
        int *gulosa = malloc((size_t)n * sizeof(*gulosa));
        clock_t inicio;
        clock_t fim;
        double tempo_bt;
        double tempo_guloso;
        int tamanho_exata;
        int tamanho_gulosa;
        int valida_exata;
        int valida_gulosa;
        int repeticao;

        assert(arestas && exata && gulosa);
        inicio = clock();
        for (repeticao = 0; repeticao < repeticoes; ++repeticao) {
            tamanho_exata = cobertura_minima_bt(
                n, arestas, quantidade_arestas, exata);
        }
        fim = clock();
        assert(inicio != (clock_t)-1 && fim != (clock_t)-1);
        tempo_bt = segundos_decorridos(inicio, fim, repeticoes);

        inicio = clock();
        for (repeticao = 0; repeticao < repeticoes; ++repeticao) {
            tamanho_gulosa = cobertura_gulosa(
                n, arestas, quantidade_arestas, gulosa);
        }
        fim = clock();
        assert(inicio != (clock_t)-1 && fim != (clock_t)-1);
        tempo_guloso = segundos_decorridos(inicio, fim, repeticoes);

        assert(tamanho_exata >= 0 && tamanho_gulosa >= 0);
        valida_exata = eh_cobertura(n, arestas, quantidade_arestas, exata,
                                    tamanho_exata);
        valida_gulosa = eh_cobertura(n, arestas, quantidade_arestas, gulosa,
                                     tamanho_gulosa);
        assert(valida_exata && valida_gulosa);

        printf("| %2d | %7d | %14.9f | %14.9f | %6d | %6d | %-8s | %-8s |\n",
               n, quantidade_arestas, tempo_bt, tempo_guloso,
               tamanho_exata, tamanho_gulosa,
               valida_exata ? "sim" : "nao",
               valida_gulosa ? "sim" : "nao");

        free(arestas);
        free(exata);
        free(gulosa);
    }
    puts("+----+---------+----------------+----------------+--------+--------+----------+----------+");
}

int main(void)
{
    executar_testes();
    executar_experimento();
    return 0;
}
