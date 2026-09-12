#include "busca_profundidade.h"

#include "busca_largura.h"
#include <stdlib.h>

void dfs_recursiva(GrafoLista *g, int u, int *visitado,
                   int *tempo_entrada, int *tempo_saida, int *tempo)
{
    No *atual;

    if (g == NULL || visitado == NULL || tempo_entrada == NULL ||
        tempo_saida == NULL || tempo == NULL || u < 0 || u >= g->n ||
        visitado[u]) {
        return;
    }

    visitado[u] = 1;
    tempo_entrada[u] = ++(*tempo);

    atual = g->adj[u];

    while (atual != NULL) {
        if (!visitado[atual->destino]) {
            dfs_recursiva(g, atual->destino, visitado,
                          tempo_entrada, tempo_saida, tempo);
        }

        atual = atual->prox;
    }

    tempo_saida[u] = ++(*tempo);
}

static void marcar_componente(GrafoLista *g, int u, int *visitado)
{
    No *atual;

    visitado[u] = 1;
    atual = g->adj[u];

    while (atual != NULL) {
        if (!visitado[atual->destino]) {
            marcar_componente(g, atual->destino, visitado);
        }

        atual = atual->prox;
    }
}

int contar_componentes(GrafoLista *g)
{
    int *visitado;
    int componentes = 0;
    int i;

    if (g == NULL || g->n <= 0) {
        return 0;
    }

    visitado = calloc((size_t)g->n, sizeof(*visitado));

    if (visitado == NULL) {
        return -1;
    }

    for (i = 0; i < g->n; i++) {
        if (!visitado[i]) {
            marcar_componente(g, i, visitado);
            componentes++;
        }
    }

    free(visitado);

    return componentes;
}

static int ciclo_dfs(GrafoLista *g, int u, int pai, int *visitado)
{
    No *atual;

    visitado[u] = 1;
    atual = g->adj[u];

    while (atual != NULL) {
        int v = atual->destino;

        if (!visitado[v]) {
            if (ciclo_dfs(g, v, u, visitado)) {
                return 1;
            }

        } else if (v != pai) {
            return 1;
        }

        atual = atual->prox;
    }

    return 0;
}

int tem_ciclo(GrafoLista *g)
{
    int *visitado;
    int i;

    if (g == NULL || g->n <= 0) {
        return 0;
    }

    visitado = calloc((size_t)g->n, sizeof(*visitado));

    if (visitado == NULL) {
        return 0;
    }

    for (i = 0; i < g->n; i++) {
        if (!visitado[i] && ciclo_dfs(g, i, -1, visitado)) {
            free(visitado);
            return 1;
        }
    }

    free(visitado);

    return 0;
}

static int colorir_componente(GrafoLista *g, int origem, int *cor)
{
    Fila fila;

    fila.dados = malloc((size_t)g->n * sizeof(*fila.dados));

    if (fila.dados == NULL) {
        return -1;
    }

    fila.capacidade = g->n;
    fila.inicio = 0;
    fila.fim = 0;
    fila.tamanho = 0;

    cor[origem] = 0;

    fila.dados[fila.fim] = origem;
    fila.fim = (fila.fim + 1) % fila.capacidade;
    fila.tamanho++;

    while (fila.tamanho > 0) {
        int u = fila.dados[fila.inicio];
        No *atual;

        fila.inicio = (fila.inicio + 1) % fila.capacidade;
        fila.tamanho--;

        atual = g->adj[u];

        while (atual != NULL) {
            int v = atual->destino;

            if (cor[v] == -1) {
                cor[v] = 1 - cor[u];

                fila.dados[fila.fim] = v;
                fila.fim = (fila.fim + 1) % fila.capacidade;
                fila.tamanho++;

            } else if (cor[v] == cor[u]) {
                free(fila.dados);
                return 0;
            }

            atual = atual->prox;
        }
    }

    free(fila.dados);

    return 1;
}

int eh_bipartido(GrafoLista *g)
{
    int *cor;
    int i;

    if (g == NULL || g->n <= 0) {
        return 0;
    }

    cor = malloc((size_t)g->n * sizeof(*cor));

    if (cor == NULL) {
        return 0;
    }

    for (i = 0; i < g->n; i++) {
        cor[i] = -1;
    }

    for (i = 0; i < g->n; i++) {
        if (cor[i] == -1) {
            int resultado = colorir_componente(g, i, cor);

            if (resultado != 1) {
                free(cor);
                return 0;
            }
        }
    }

    free(cor);

    return 1;
}