#include "dijkstra.h"
#include <assert.h>
#include <stdio.h>

static void verificar_heap(const MinHeap *h)
{
    int presentes = 0;
    for (int v = 0; v < h->capacidade; ++v) {
        int p = h->posicao[v];
        if (p == -1) continue;
        assert(p >= 0 && p < h->tamanho);
        assert(h->dados[p].vertice == v);
        ++presentes;
    }
    assert(presentes == h->tamanho);
    for (int i = 0; i < h->tamanho; ++i) {
        assert(h->posicao[h->dados[i].vertice] == i);
        if (i) assert(h->dados[(i - 1) / 2].distancia <= h->dados[i].distancia);
    }
}

static void testar_heap(void)
{
    assert(!criar_min_heap(0));
    assert(!inserir_min_heap(NULL, 0, 1));
    assert(extrair_minimo(NULL).vertice == -1);
    MinHeap *h = criar_min_heap(64);
    assert(h);
    for (int i = 0; i < 64; ++i) {
        assert(inserir_min_heap(h, i, 1000 - i));
        verificar_heap(h);
    }
    assert(!inserir_min_heap(h, 0, 0));
    assert(!inserir_min_heap(h, -1, 0));
    assert(!inserir_min_heap(h, 64, 0));
    for (int i = 0; i < 64; i += 2) {
        diminuir_chave(h, i, i);
        verificar_heap(h);
    }
    diminuir_chave(h, 0, 2000);
    diminuir_chave(h, -1, 0);
    diminuir_chave(h, 64, 0);
    int anterior = -1;
    while (h->tamanho) {
        NoHeap no = extrair_minimo(h);
        assert(no.distancia >= anterior);
        assert(h->posicao[no.vertice] == -1);
        anterior = no.distancia;
        diminuir_chave(h, no.vertice, 0);
        verificar_heap(h);
    }
    assert(extrair_minimo(h).vertice == -1);
    assert(inserir_min_heap(h, 0, 0));
    assert(!inserir_min_heap(h, 0, 1));
    assert(extrair_minimo(h).distancia == 0);
    liberar_min_heap(h);
    liberar_min_heap(NULL);
    puts("Min-Heap: insercao, extracao, diminuicao e posicoes OK.");
}

static void testar_dijkstra(void)
{
    GrafoListaPonderado *g = criar_grafo_lista_ponderado(7);
    assert(g);
    const int arestas[][3] = {
        {0, 1, 10}, {0, 2, 3}, {2, 1, 1}, {1, 3, 2},
        {2, 3, 8}, {2, 4, 2}, {4, 3, 0}, {3, 5, 0},
        {4, 5, 9}, {5, 4, 0}, {0, 0, 0}, {0, 1, 12}
    };
    for (size_t i = 0; i < sizeof(arestas) / sizeof(arestas[0]); ++i)
        assert(inserir_aresta_ponderada(g, arestas[i][0], arestas[i][1], arestas[i][2]));
    int dist[7], pred[7];
    const int esperado[] = {0, 4, 3, 5, 5, 5, INT_MAX};
    const int predecessores[] = {-1, 2, 0, 4, 2, 3, -1};
    dijkstra(g, 0, dist, pred);
    for (int i = 0; i < 7; ++i) {
        assert(dist[i] == esperado[i]);
        assert(pred[i] == predecessores[i]);
        if (dist[i] == INT_MAX) printf("Vertice %d: inalcançavel\n", i);
        else printf("Vertice %d: distancia %d, predecessor %d\n", i, dist[i], pred[i]);
    }
    printf("Caminho minimo 0 a 5 (custo 5): ");
    reconstruir_caminho(0, 5, pred);
    printf("Origem para si mesma: ");
    reconstruir_caminho(0, 0, pred);
    printf("Vertice inalcançavel: ");
    reconstruir_caminho(0, 6, pred);
    assert(!inserir_aresta_ponderada(g, 0, 1, -1));
    assert(!inserir_aresta_ponderada(g, 7, 0, 1));
    assert(!inserir_aresta_ponderada(g, 0, -1, 1));
    dijkstra(g, 7, dist, pred);
    for (int i = 0; i < 7; ++i) assert(dist[i] == INT_MAX && pred[i] == -1);
    /* Valida tambem grafos corrompidos fora da funcao de insercao. */
    g->adj[0]->peso = -1;
    dijkstra(g, 0, dist, pred);
    for (int i = 0; i < 7; ++i) assert(dist[i] == INT_MAX && pred[i] == -1);
    g->adj[0]->peso = 12;
    g->adj[0]->destino = 7;
    dijkstra(g, 0, dist, pred);
    for (int i = 0; i < 7; ++i) assert(dist[i] == INT_MAX && pred[i] == -1);
    liberar_grafo_lista_ponderado(g);

    g = criar_grafo_lista_ponderado(3);
    assert(g);
    assert(inserir_aresta_ponderada(g, 0, 1, INT_MAX - 1));
    assert(inserir_aresta_ponderada(g, 1, 2, 10));
    dijkstra(g, 0, dist, pred);
    assert(dist[1] == INT_MAX - 1 && dist[2] == INT_MAX && pred[2] == -1);
    liberar_grafo_lista_ponderado(g);
    g = criar_grafo_lista_ponderado(3);
    assert(g);
    assert(inserir_aresta_ponderada(g, 0, 1, 0));
    assert(inserir_aresta_ponderada(g, 1, 2, 0));
    assert(inserir_aresta_ponderada(g, 2, 0, 0));
    dijkstra(g, 0, dist, pred);
    assert(dist[0] == 0 && dist[1] == 0 && dist[2] == 0);
    assert(pred[0] == -1 && pred[1] == 0 && pred[2] == 1);
    dijkstra(g, 2, dist, pred);
    assert(dist[0] == 0 && dist[1] == 0 && dist[2] == 0);
    assert(pred[2] == -1 && pred[0] == 2 && pred[1] == 0);
    liberar_grafo_lista_ponderado(g);
    g = criar_grafo_lista_ponderado(1);
    assert(g);
    dijkstra(g, 0, dist, pred);
    assert(dist[0] == 0 && pred[0] == -1);
    dijkstra(g, -1, dist, pred);
    assert(dist[0] == INT_MAX && pred[0] == -1);
    dijkstra(g, 0, NULL, pred);
    dijkstra(g, 0, dist, NULL);
    liberar_grafo_lista_ponderado(g);
    assert(!criar_grafo_lista_ponderado(0));
    dijkstra(NULL, 0, dist, pred);
    reconstruir_caminho(-1, 0, pred);
    reconstruir_caminho(0, 1, NULL);
    int ciclo[] = {-1, 2, 1};
    reconstruir_caminho(0, 2, ciclo);
    liberar_grafo_lista_ponderado(NULL);
    puts("Dijkstra: multiplos caminhos, inalcançavel, pesos zero e entradas invalidas OK.");
}

int main(void)
{
    testar_heap();
    testar_dijkstra();
    puts("Todos os testes passaram.");
    return 0;
}
