#include "grafo.h"
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

typedef struct aresta {
    int origem;
    int destino;
    void *valor;
    struct aresta *proxima_saida;
    struct aresta *proxima_entrada;
} Aresta;

typedef struct {
    void *valor;
    Aresta *saidas;
    Aresta *entradas;
} Vertice;

struct grafo {
    Vertice *vertices;
    int quantidade;
    size_t capacidade;
    size_t tam_no;
    size_t tam_aresta;
    Aresta *proxima;
    bool chegando;
};

static void *copia_dado(const void *dado, size_t tamanho)
{
    void *copia = malloc(tamanho);
    if (!copia) abort();
    memcpy(copia, dado, tamanho);
    return copia;
}

Grafo g_cria(int tam_no, int tam_aresta)
{
    if (tam_no <= 0 || tam_aresta <= 0) return NULL;
    Grafo g = calloc(1, sizeof(*g));
    if (!g) abort();
    g->tam_no = tam_no;
    g->tam_aresta = tam_aresta;
    return g;
}

int g_num_nós(Grafo g)
{
    return g->quantidade;
}

int g_insere_nó(Grafo g, void *dado)
{
    if (g->quantidade == INT_MAX) abort();
    if ((size_t)g->quantidade == g->capacidade) {
        size_t capacidade = g->capacidade ? 2 * g->capacidade : 8;
        if (capacidade <= g->capacidade || capacidade > SIZE_MAX / sizeof(Vertice)) abort();
        Vertice *vertices = realloc(g->vertices, capacidade * sizeof(Vertice));
        if (!vertices) abort();
        g->vertices = vertices;
        g->capacidade = capacidade;
    }
    int pos = g->quantidade++;
    g->vertices[pos] = (Vertice){.valor = copia_dado(dado, g->tam_no)};
    return pos;
}

static bool existe_no(Grafo g, int no)
{
    return no >= 0 && no < g->quantidade;
}

void *g_valor_do_nó(Grafo g, int no)
{
    if (!existe_no(g, no)) return NULL;
    return g->vertices[no].valor;
}

void g_altera_valor_do_nó(Grafo g, int no, void *dado)
{
    if (existe_no(g, no)) memmove(g->vertices[no].valor, dado, g->tam_no);
}

static Aresta *busca_aresta(Grafo g, int origem, int destino)
{
    if (!existe_no(g, origem) || !existe_no(g, destino)) return NULL;
    Aresta *a = g->vertices[origem].saidas;
    while (a && a->destino != destino) a = a->proxima_saida;
    return a;
}

void *g_valor_da_aresta(Grafo g, int origem, int destino)
{
    Aresta *a = busca_aresta(g, origem, destino);
    return a ? a->valor : NULL;
}

// Cada aresta aparece em duas listas, mas o valor é alocado uma única vez.
static void remove_aresta(Grafo g, Aresta *a)
{
    Aresta **saida = &g->vertices[a->origem].saidas;
    Aresta **entrada = &g->vertices[a->destino].entradas;
    while (*saida != a) saida = &(*saida)->proxima_saida;
    while (*entrada != a) entrada = &(*entrada)->proxima_entrada;
    *saida = a->proxima_saida;
    *entrada = a->proxima_entrada;
    g->proxima = NULL;
    free(a->valor);
    free(a);
}

void g_altera_valor_da_aresta(Grafo g, int origem, int destino, void *dado)
{
    if (!existe_no(g, origem) || !existe_no(g, destino)) return;
    Aresta *a = busca_aresta(g, origem, destino);
    if (!dado) {
        if (a) remove_aresta(g, a);
    } else if (a) {
        memmove(a->valor, dado, g->tam_aresta);
    } else {
        a = malloc(sizeof(*a));
        if (!a) abort();
        a->origem = origem;
        a->destino = destino;
        a->valor = copia_dado(dado, g->tam_aresta);
        a->proxima_saida = g->vertices[origem].saidas;
        a->proxima_entrada = g->vertices[destino].entradas;
        g->vertices[origem].saidas = a;
        g->vertices[destino].entradas = a;
    }
}

void g_remove_nó(Grafo g, int no)
{
    if (!existe_no(g, no)) return;
    Vertice *v = &g->vertices[no];
    while (v->saidas) remove_aresta(g, v->saidas);
    while (v->entradas) remove_aresta(g, v->entradas);
    free(v->valor);
    g->quantidade--;
    memmove(v, v + 1, (g->quantidade - no) * sizeof(*v));
    for (int i = no; i < g->quantidade; i++) {
        for (Aresta *a = g->vertices[i].saidas; a; a = a->proxima_saida) a->origem = i;
        for (Aresta *a = g->vertices[i].entradas; a; a = a->proxima_entrada) a->destino = i;
    }
    g->proxima = NULL;
}

void g_arestas_que_partem(Grafo g, int origem)
{
    g->chegando = false;
    g->proxima = existe_no(g, origem) ? g->vertices[origem].saidas : NULL;
}

void g_arestas_que_chegam(Grafo g, int destino)
{
    g->chegando = true;
    g->proxima = existe_no(g, destino) ? g->vertices[destino].entradas : NULL;
}

void *g_próxima_aresta(Grafo g, int *vizinho)
{
    Aresta *a = g->proxima;
    if (!a) return NULL;
    if (g->chegando) {
        if (vizinho) *vizinho = a->origem;
        g->proxima = a->proxima_entrada;
    } else {
        if (vizinho) *vizinho = a->destino;
        g->proxima = a->proxima_saida;
    }
    return a->valor;
}

void g_destrói(Grafo g)
{
    for (int i = 0; i < g->quantidade; i++) {
        Aresta *a = g->vertices[i].saidas;
        while (a) {
            Aresta *proxima = a->proxima_saida;
            free(a->valor);
            free(a);
            a = proxima;
        }
        free(g->vertices[i].valor);
    }
    free(g->vertices);
    free(g);
}

// Um vértice fica pronto quando todos os seus antecessores já saíram da fila.
static int percorre_sem_dependencias(Grafo g, Fila ordem)
{
    int visitados = 0;
    if (!g->quantidade) return 0;
    int *grau = calloc(g->quantidade, sizeof(*grau));
    if (!grau) abort();
    Fila prontos = f_cria(sizeof(int));
    for (int i = 0; i < g->quantidade; i++) {
        for (Aresta *a = g->vertices[i].entradas; a; a = a->proxima_entrada) grau[i]++;
        if (!grau[i]) f_insere(prontos, &i);
    }
    while (!f_tá_vazia(prontos)) {
        int no;
        f_remove(prontos, &no);
        if (ordem) f_insere(ordem, &no);
        visitados++;
        for (Aresta *a = g->vertices[no].saidas; a; a = a->proxima_saida) {
            grau[a->destino]--;
            if (!grau[a->destino]) f_insere(prontos, &a->destino);
        }
    }
    f_destrói(prontos);
    free(grau);
    return visitados;
}

bool g_tem_ciclo(Grafo g)
{
    return percorre_sem_dependencias(g, NULL) != g->quantidade;
}

Fila g_ordem_topológica(Grafo g)
{
    Fila ordem = f_cria(sizeof(int));
    if (percorre_sem_dependencias(g, ordem) != g->quantidade) {
        while (!f_tá_vazia(ordem)) f_remove(ordem, NULL);
    }
    return ordem;
}
