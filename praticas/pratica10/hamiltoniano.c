#include "hamiltoniano.h"

#include <stdlib.h>

typedef struct {
    int n;
    const int *adjacencias;
    int buscar_ciclo;
    int *caminho;
    unsigned char *visitado;
} EstadoHamiltoniano;

static int ha_aresta(const EstadoHamiltoniano *estado, int u, int v)
{
    return estado->adjacencias[u * estado->n + v] != 0;
}

static int buscar(EstadoHamiltoniano *estado, int posicao)
{
    int vertice;

    if (posicao == estado->n) {
        if (estado->buscar_ciclo &&
            !ha_aresta(estado, estado->caminho[estado->n - 1],
                       estado->caminho[0])) {
            return 0;
        }
        if (estado->buscar_ciclo) {
            estado->caminho[estado->n] = estado->caminho[0];
        }
        return 1;
    }

    for (vertice = 0; vertice < estado->n; ++vertice) {
        if (!estado->visitado[vertice] &&
            (posicao == 0 ||
             ha_aresta(estado, estado->caminho[posicao - 1], vertice))) {
            estado->caminho[posicao] = vertice;
            estado->visitado[vertice] = 1;
            if (buscar(estado, posicao + 1)) {
                return 1;
            }
            estado->visitado[vertice] = 0;
        }
    }
    return 0;
}

int encontrar_caminho_hamiltoniano_bt(int n, const int *adjacencias,
                                      int buscar_ciclo, int *resultado)
{
    EstadoHamiltoniano estado;
    int encontrado;

    if (n <= 0 || !adjacencias || !resultado) {
        return -1;
    }
    estado.visitado = calloc((size_t)n, sizeof(*estado.visitado));
    if (!estado.visitado) {
        return -1;
    }
    estado.n = n;
    estado.adjacencias = adjacencias;
    estado.buscar_ciclo = buscar_ciclo != 0;
    estado.caminho = resultado;
    encontrado = buscar(&estado, 0);
    free(estado.visitado);
    return encontrado;
}

int caminho_hamiltoniano_valido(int n, const int *adjacencias,
                                const int *caminho, int eh_ciclo)
{
    unsigned char *visitado;
    int valido = 1;
    int i;

    if (n <= 0 || !adjacencias || !caminho) {
        return 0;
    }
    visitado = calloc((size_t)n, sizeof(*visitado));
    if (!visitado) {
        return 0;
    }
    for (i = 0; i < n; ++i) {
        int vertice = caminho[i];

        if (vertice < 0 || vertice >= n || visitado[vertice]) {
            valido = 0;
            break;
        }
        visitado[vertice] = 1;
        if (i > 0 && !adjacencias[caminho[i - 1] * n + vertice]) {
            valido = 0;
            break;
        }
    }
    if (valido && eh_ciclo &&
        (caminho[n] != caminho[0] ||
         !adjacencias[caminho[n - 1] * n + caminho[0]])) {
        valido = 0;
    }
    free(visitado);
    return valido;
}
