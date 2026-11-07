#include "tps.h"

#include <stdlib.h>

typedef struct {
    int n;
    const int *pesos;
    int origem;
    int *atual;
    int *melhor;
    unsigned char *visitado;
    long long melhor_custo;
    int encontrou;
} EstadoTsp;

static int peso_valido(int peso)
{
    return peso >= 0 && peso != TSP_SEM_ARESTA;
}

static void copiar_circuito(int n, int *destino, const int *origem)
{
    int i;

    for (i = 0; i <= n; ++i) {
        destino[i] = origem[i];
    }
}

static void permutar_tsp(EstadoTsp *estado, int posicao, long long custo,
                         int circuito_valido)
{
    int vertice;

    if (posicao == estado->n) {
        int retorno = estado->pesos[estado->atual[estado->n - 1] * estado->n +
                                    estado->origem];
        long long custo_final;

        if (!circuito_valido || !peso_valido(retorno)) {
            return;
        }
        custo_final = custo + retorno;
        if (!estado->encontrou || custo_final < estado->melhor_custo) {
            estado->atual[estado->n] = estado->origem;
            copiar_circuito(estado->n, estado->melhor, estado->atual);
            estado->melhor_custo = custo_final;
            estado->encontrou = 1;
        }
        return;
    }

    for (vertice = 0; vertice < estado->n; ++vertice) {
        int peso;

        if (estado->visitado[vertice]) {
            continue;
        }
        peso = estado->pesos[estado->atual[posicao - 1] * estado->n +
                             vertice];
        estado->visitado[vertice] = 1;
        estado->atual[posicao] = vertice;
        permutar_tsp(estado, posicao + 1,
                     custo + (peso_valido(peso) ? peso : 0),
                     circuito_valido && peso_valido(peso));
        estado->visitado[vertice] = 0;
    }
}

int tsp_forca_bruta(int n, const int *pesos, int origem,
                    int *circuito, long long *custo_total)
{
    EstadoTsp estado;

    if (n <= 0 || !pesos || origem < 0 || origem >= n ||
        !circuito || !custo_total) {
        return -1;
    }
    if (n == 1) {
        circuito[0] = origem;
        circuito[1] = origem;
        *custo_total = 0;
        return 1;
    }
    estado.atual = malloc(((size_t)n + 1U) * sizeof(*estado.atual));
    estado.visitado = calloc((size_t)n, sizeof(*estado.visitado));
    if (!estado.atual || !estado.visitado) {
        free(estado.atual);
        free(estado.visitado);
        return -1;
    }
    estado.n = n;
    estado.pesos = pesos;
    estado.origem = origem;
    estado.melhor = circuito;
    estado.melhor_custo = 0;
    estado.encontrou = 0;
    estado.atual[0] = origem;
    estado.visitado[origem] = 1;
    permutar_tsp(&estado, 1, 0, 1);
    free(estado.atual);
    free(estado.visitado);
    if (!estado.encontrou) {
        return 0;
    }
    *custo_total = estado.melhor_custo;
    return 1;
}

int tsp_vizinho_mais_proximo(int n, const int *pesos, int origem,
                             int *circuito, long long *custo_total)
{
    unsigned char *visitado;
    long long custo = 0;
    int atual;
    int posicao;

    if (n <= 0 || !pesos || origem < 0 || origem >= n ||
        !circuito || !custo_total) {
        return -1;
    }
    visitado = calloc((size_t)n, sizeof(*visitado));
    if (!visitado) {
        return -1;
    }
    atual = origem;
    circuito[0] = origem;
    visitado[origem] = 1;
    for (posicao = 1; posicao < n; ++posicao) {
        int melhor = -1;
        int melhor_peso = TSP_SEM_ARESTA;
        int vertice;

        for (vertice = 0; vertice < n; ++vertice) {
            int peso = pesos[atual * n + vertice];

            if (!visitado[vertice] && peso_valido(peso) &&
                (melhor < 0 || peso < melhor_peso ||
                 (peso == melhor_peso && vertice < melhor))) {
                melhor = vertice;
                melhor_peso = peso;
            }
        }
        if (melhor < 0) {
            free(visitado);
            return 0;
        }
        circuito[posicao] = melhor;
        visitado[melhor] = 1;
        custo += melhor_peso;
        atual = melhor;
    }
    if (n > 1 && !peso_valido(pesos[atual * n + origem])) {
        free(visitado);
        return 0;
    }
    if (n > 1) {
        custo += pesos[atual * n + origem];
    }
    circuito[n] = origem;
    *custo_total = custo;
    free(visitado);
    return 1;
}

static int distancia_ao_circuito(int n, const int *pesos,
                                 const int *circuito, int tamanho,
                                 int vertice)
{
    int menor = TSP_SEM_ARESTA;
    int i;

    for (i = 0; i < tamanho; ++i) {
        int peso = pesos[circuito[i] * n + vertice];

        if (peso_valido(peso) && peso < menor) {
            menor = peso;
        }
    }
    return menor;
}

int tsp_insercao(int n, const int *pesos, int origem,
                 int *circuito, long long *custo_total)
{
    unsigned char *visitado;
    int tamanho;

    if (n <= 0 || !pesos || origem < 0 || origem >= n ||
        !circuito || !custo_total) {
        return -1;
    }
    if (n == 1) {
        circuito[0] = origem;
        circuito[1] = origem;
        *custo_total = 0;
        return 1;
    }
    visitado = calloc((size_t)n, sizeof(*visitado));
    if (!visitado) {
        return -1;
    }
    circuito[0] = origem;
    circuito[1] = origem;
    visitado[origem] = 1;
    tamanho = 1;

    while (tamanho < n) {
        int escolhido = -1;
        int menor_distancia = TSP_SEM_ARESTA;
        int melhor_posicao = -1;
        long long menor_aumento = 0;
        int vertice;
        int posicao;

        for (vertice = 0; vertice < n; ++vertice) {
            int distancia;

            if (visitado[vertice]) {
                continue;
            }
            distancia = distancia_ao_circuito(n, pesos, circuito, tamanho,
                                              vertice);
            if (distancia < menor_distancia ||
                (distancia == menor_distancia &&
                 distancia != TSP_SEM_ARESTA && vertice < escolhido)) {
                escolhido = vertice;
                menor_distancia = distancia;
            }
        }
        if (escolhido < 0) {
            free(visitado);
            return 0;
        }
        for (posicao = 0; posicao < tamanho; ++posicao) {
            int u = circuito[posicao];
            int v = circuito[posicao + 1];
            int ida = pesos[u * n + escolhido];
            int volta = pesos[escolhido * n + v];
            int removida = (tamanho == 1) ? 0 : pesos[u * n + v];
            long long aumento;

            if (!peso_valido(ida) || !peso_valido(volta) ||
                (tamanho > 1 && !peso_valido(removida))) {
                continue;
            }
            aumento = (long long)ida + volta - removida;
            if (melhor_posicao < 0 || aumento < menor_aumento) {
                melhor_posicao = posicao;
                menor_aumento = aumento;
            }
        }
        if (melhor_posicao < 0) {
            free(visitado);
            return 0;
        }
        for (posicao = tamanho + 1; posicao > melhor_posicao + 1;
             --posicao) {
            circuito[posicao] = circuito[posicao - 1];
        }
        circuito[melhor_posicao + 1] = escolhido;
        visitado[escolhido] = 1;
        ++tamanho;
    }

    *custo_total = 0;
    for (tamanho = 0; tamanho < n; ++tamanho) {
        *custo_total += pesos[circuito[tamanho] * n +
                              circuito[tamanho + 1]];
    }
    free(visitado);
    return 1;
}

int grafo_para_tsp(int n, const int *adjacencias, int *pesos_tsp)
{
    int u;
    int v;

    if (n <= 0 || !adjacencias || !pesos_tsp) {
        return -1;
    }
    for (u = 0; u < n; ++u) {
        for (v = 0; v < n; ++v) {
            pesos_tsp[u * n + v] = (u == v) ? 0 :
                                      (adjacencias[u * n + v] ? 1 : 2);
        }
    }
    return 1;
}

int tsp_circuito_valido(int n, const int *pesos, int origem,
                        const int *circuito, long long custo_total)
{
    unsigned char *visitado;
    long long custo = 0;
    int valido = 1;
    int i;

    if (n <= 0 || !pesos || origem < 0 || origem >= n || !circuito ||
        circuito[0] != origem || circuito[n] != origem) {
        return 0;
    }
    visitado = calloc((size_t)n, sizeof(*visitado));
    if (!visitado) {
        return 0;
    }
    for (i = 0; i < n; ++i) {
        int u = circuito[i];
        int v = circuito[i + 1];
        int peso;

        if (u < 0 || u >= n || visitado[u] || v < 0 || v >= n) {
            valido = 0;
            break;
        }
        visitado[u] = 1;
        if (n == 1) {
            continue;
        }
        peso = pesos[u * n + v];
        if (!peso_valido(peso)) {
            valido = 0;
            break;
        }
        custo += peso;
    }
    free(visitado);
    return valido && custo == custo_total;
}
