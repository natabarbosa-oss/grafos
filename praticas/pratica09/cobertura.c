#include "cobertura.h"

#include <stdlib.h>

static int entrada_valida(int n, const Aresta *arestas,
                          int quantidade_arestas)
{
    int i;

    if (n < 0 || quantidade_arestas < 0 ||
        (quantidade_arestas > 0 && (!arestas || n == 0))) {
        return 0;
    }
    for (i = 0; i < quantidade_arestas; ++i) {
        if (arestas[i].u < 0 || arestas[i].u >= n ||
            arestas[i].v < 0 || arestas[i].v >= n) {
            return 0;
        }
    }
    return 1;
}

int eh_cobertura(int n, const Aresta *arestas, int quantidade_arestas,
                 const int *cobertura, int tamanho_cobertura)
{
    unsigned char *selecionado;
    int i;
    int valido = 1;

    if (!entrada_valida(n, arestas, quantidade_arestas) ||
        tamanho_cobertura < 0 || tamanho_cobertura > n ||
        (tamanho_cobertura > 0 && !cobertura)) {
        return 0;
    }
    selecionado = calloc(n > 0 ? (size_t)n : 1U, sizeof(*selecionado));
    if (!selecionado) {
        return 0;
    }
    for (i = 0; i < tamanho_cobertura; ++i) {
        if (cobertura[i] < 0 || cobertura[i] >= n ||
            selecionado[cobertura[i]]) {
            valido = 0;
            break;
        }
        selecionado[cobertura[i]] = 1;
    }
    for (i = 0; valido && i < quantidade_arestas; ++i) {
        if (!selecionado[arestas[i].u] && !selecionado[arestas[i].v]) {
            valido = 0;
        }
    }
    free(selecionado);
    return valido;
}

typedef struct {
    int n;
    const Aresta *arestas;
    int quantidade_arestas;
    int *atual;
    int *melhor;
    int melhor_tamanho;
} EstadoBt;

static void copiar_solucao(EstadoBt *estado, int tamanho)
{
    int i;

    for (i = 0; i < tamanho; ++i) {
        estado->melhor[i] = estado->atual[i];
    }
    estado->melhor_tamanho = tamanho;
}

static void buscar_cobertura(EstadoBt *estado, int vertice, int tamanho)
{
    if (tamanho >= estado->melhor_tamanho) {
        return;
    }

    if (eh_cobertura(estado->n, estado->arestas,
                     estado->quantidade_arestas, estado->atual, tamanho)) {
        copiar_solucao(estado, tamanho);
        return;
    }

    if (vertice == estado->n) {
        return;
    }

    estado->atual[tamanho] = vertice;
    buscar_cobertura(estado, vertice + 1, tamanho + 1);
    buscar_cobertura(estado, vertice + 1, tamanho);
}

int cobertura_minima_bt(int n, const Aresta *arestas, int quantidade_arestas,
                        int *cobertura)
{
    EstadoBt estado;
    int *atual;

    if (!entrada_valida(n, arestas, quantidade_arestas) ||
        (n > 0 && !cobertura)) {
        return -1;
    }
    atual = malloc((n > 0 ? (size_t)n : 1U) * sizeof(*atual));
    if (!atual) {
        return -1;
    }
    estado.n = n;
    estado.arestas = arestas;
    estado.quantidade_arestas = quantidade_arestas;
    estado.atual = atual;
    estado.melhor = cobertura;
    estado.melhor_tamanho = n + 1;

    buscar_cobertura(&estado, 0, 0);
    free(atual);
    return estado.melhor_tamanho <= n ? estado.melhor_tamanho : -1;
}

int cobertura_gulosa(int n, const Aresta *arestas, int quantidade_arestas,
                     int *cobertura)
{
    unsigned char *coberta;
    int restantes;
    int tamanho = 0;

    if (!entrada_valida(n, arestas, quantidade_arestas) ||
        (n > 0 && !cobertura)) {
        return -1;
    }
    coberta = calloc(quantidade_arestas > 0 ? (size_t)quantidade_arestas : 1U,
                     sizeof(*coberta));
    if (!coberta) {
        return -1;
    }
    restantes = quantidade_arestas;

    while (restantes > 0) {
        int melhor_vertice = -1;
        int maior_grau = -1;
        int v;
        int i;

        for (v = 0; v < n; ++v) {
            int grau_restante = 0;

            for (i = 0; i < quantidade_arestas; ++i) {
                if (!coberta[i] &&
                    (arestas[i].u == v || arestas[i].v == v)) {
                    ++grau_restante;
                }
            }
            if (grau_restante > maior_grau) {
                maior_grau = grau_restante;
                melhor_vertice = v;
            }
        }

        if (melhor_vertice < 0 || maior_grau <= 0) {
            free(coberta);
            return -1;
        }
        cobertura[tamanho++] = melhor_vertice;
        for (i = 0; i < quantidade_arestas; ++i) {
            if (!coberta[i] &&
                (arestas[i].u == melhor_vertice ||
                 arestas[i].v == melhor_vertice)) {
                coberta[i] = 1;
                --restantes;
            }
        }
    }

    free(coberta);
    return tamanho;
}
