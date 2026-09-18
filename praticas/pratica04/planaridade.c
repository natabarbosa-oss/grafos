#include "planaridade.h"

#include <stdint.h>
#include <stdlib.h>

typedef struct {
    const GrafoLista *g;
    int quantidade_pares;
    int pares[10][2];
    unsigned int mascara_ramos;
} BuscaSubdivisao;

static int contar_arestas(const GrafoLista *g)
{
    int soma = 0;
    int u;

    for (u = 0; u < g->n; u++) {
        const No *atual;

        for (atual = g->adj[u]; atual != NULL; atual = atual->prox) {
            soma++;
        }
    }

    return soma / 2;
}

int eh_planar_euler(GrafoLista *g)
{
    if (g == NULL) {
        return 0;
    }

    if (g->n < 3) {
        return 1;
    }

    return contar_arestas(g) <= 3 * g->n - 6;
}

static int buscar_pares(const BuscaSubdivisao *busca, int indice,
                        unsigned int internos_usados,
                        uint64_t arestas_usadas);

static int enumerar_caminhos(const BuscaSubdivisao *busca, int indice,
                             int atual, int destino,
                             unsigned int visitados,
                             unsigned int internos_usados,
                             uint64_t arestas_usadas)
{
    const No *no;

    for (no = busca->g->adj[atual]; no != NULL; no = no->prox) {
        int v = no->destino;
        int a = atual < v ? atual : v;
        int b = atual < v ? v : atual;
        int codigo = a * (2 * busca->g->n - a - 1) / 2 +
                     (b - a - 1);
        uint64_t bit_aresta = UINT64_C(1) << codigo;

        if ((arestas_usadas & bit_aresta) != 0 ||
            (visitados & (1U << v)) != 0) {
            continue;
        }

        if (v == destino) {
            if (buscar_pares(busca, indice + 1, internos_usados,
                             arestas_usadas | bit_aresta)) {
                return 1;
            }
        } else if ((busca->mascara_ramos & (1U << v)) == 0 &&
                   (internos_usados & (1U << v)) == 0 &&
                   enumerar_caminhos(busca, indice, v, destino,
                                     visitados | (1U << v),
                                     internos_usados | (1U << v),
                                     arestas_usadas | bit_aresta)) {
            return 1;
        }
    }

    return 0;
}

static int buscar_pares(const BuscaSubdivisao *busca, int indice,
                        unsigned int internos_usados,
                        uint64_t arestas_usadas)
{
    int origem;
    int destino;

    if (indice == busca->quantidade_pares) {
        return 1;
    }

    origem = busca->pares[indice][0];
    destino = busca->pares[indice][1];

    return enumerar_caminhos(busca, indice, origem, destino,
                             1U << origem, internos_usados,
                             arestas_usadas);
}

static int testar_k5(const GrafoLista *g, unsigned int mascara)
{
    BuscaSubdivisao busca;
    int vertices[5];
    int quantidade = 0;
    int i;
    int j;

    busca.g = g;
    busca.quantidade_pares = 0;
    busca.mascara_ramos = mascara;

    for (i = 0; i < g->n; i++) {
        if ((mascara & (1U << i)) != 0) {
            vertices[quantidade++] = i;
        }
    }

    for (i = 0; i < 5; i++) {
        for (j = i + 1; j < 5; j++) {
            busca.pares[busca.quantidade_pares][0] = vertices[i];
            busca.pares[busca.quantidade_pares][1] = vertices[j];
            busca.quantidade_pares++;
        }
    }

    return buscar_pares(&busca, 0, 0U, UINT64_C(0));
}

static int testar_k33(const GrafoLista *g, unsigned int esquerda,
                      unsigned int direita)
{
    BuscaSubdivisao busca;
    int u;
    int v;

    busca.g = g;
    busca.quantidade_pares = 0;
    busca.mascara_ramos = esquerda | direita;

    for (u = 0; u < g->n; u++) {
        if ((esquerda & (1U << u)) == 0) {
            continue;
        }

        for (v = 0; v < g->n; v++) {
            if ((direita & (1U << v)) != 0) {
                busca.pares[busca.quantidade_pares][0] = u;
                busca.pares[busca.quantidade_pares][1] = v;
                busca.quantidade_pares++;
            }
        }
    }

    return buscar_pares(&busca, 0, 0U, UINT64_C(0));
}

static int quantidade_bits(unsigned int valor)
{
    int quantidade = 0;

    while (valor != 0U) {
        quantidade += (int)(valor & 1U);
        valor >>= 1;
    }

    return quantidade;
}

static int tem_subdivisao_kuratowski(const GrafoLista *g)
{
    unsigned int limite = 1U << g->n;
    unsigned int conjunto;

    if (g->n >= 5) {
        for (conjunto = 0U; conjunto < limite; conjunto++) {
            if (quantidade_bits(conjunto) == 5 &&
                testar_k5(g, conjunto)) {
                return 1;
            }
        }
    }

    if (g->n >= 6) {
        unsigned int esquerda;

        for (esquerda = 0U; esquerda < limite; esquerda++) {
            unsigned int disponiveis;
            unsigned int direita;

            if (quantidade_bits(esquerda) != 3) {
                continue;
            }

            disponiveis = (limite - 1U) & ~esquerda;

            for (direita = disponiveis; direita != 0U;
                 direita = (direita - 1U) & disponiveis) {
                if (quantidade_bits(direita) == 3 &&
                    esquerda < direita &&
                    testar_k33(g, esquerda, direita)) {
                    return 1;
                }
            }
        }
    }

    return 0;
}

int eh_planar_heuristica(GrafoLista *g)
{
    if (!eh_planar_euler(g)) {
        return 0;
    }

    /* A busca combinatoria solicitada fica restrita aos grafos pequenos. */
    if (g->n <= 10 && tem_subdivisao_kuratowski(g)) {
        return 0;
    }

    return 1;
}