#include "fila.h"
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct fila {
    unsigned char *dados;
    size_t largura;
    size_t capacidade;
    size_t inicio;
    size_t quantidade;
    size_t proximo;
    size_t faltam;
    bool reverso;
};

Fila f_cria(int tam_do_dado)
{
    if (tam_do_dado <= 0) return NULL;
    Fila f = calloc(1, sizeof(*f));
    if (!f) abort();
    f->largura = tam_do_dado;
    return f;
}

void f_destrói(Fila f)
{
    free(f->dados);
    free(f);
}

bool f_tá_vazia(Fila f)
{
    return f->quantidade == 0;
}

static unsigned char *elemento(Fila f, size_t pos)
{
    size_t indice = (f->inicio + pos) % f->capacidade;
    return f->dados + indice * f->largura;
}

// Ao crescer, coloca o primeiro elemento na posição zero do novo vetor.
static void aumenta_fila(Fila f)
{
    size_t capacidade = f->capacidade ? 2 * f->capacidade : 8;
    if (capacidade <= f->capacidade || capacidade > SIZE_MAX / f->largura) abort();
    unsigned char *dados = malloc(capacidade * f->largura);
    if (!dados) abort();
    for (size_t i = 0; i < f->quantidade; i++) {
        memcpy(dados + i * f->largura, elemento(f, i), f->largura);
    }
    free(f->dados);
    f->dados = dados;
    f->capacidade = capacidade;
    f->inicio = 0;
}

void f_insere(Fila f, void *dado)
{
    if (f->quantidade == f->capacidade) aumenta_fila(f);
    memcpy(elemento(f, f->quantidade), dado, f->largura);
    f->quantidade++;
    f->faltam = 0;
}

void f_remove(Fila f, void *dado)
{
    if (f_tá_vazia(f)) return;
    if (dado) memcpy(dado, elemento(f, 0), f->largura);
    f->inicio = (f->inicio + 1) % f->capacidade;
    f->quantidade--;
    f->faltam = 0;
}

void f_inicia_percurso(Fila f, int pos)
{
    f->reverso = pos < 0;
    f->faltam = 0;
    if (f->reverso) {
        size_t distancia = (size_t)(-(long long)pos);
        if (distancia > f->quantidade) return;
        f->proximo = f->quantidade - distancia;
        f->faltam = f->proximo + 1;
    } else if ((size_t)pos < f->quantidade) {
        f->proximo = (size_t)pos;
        f->faltam = f->quantidade - f->proximo;
    }
}

bool f_próximo(Fila f, void *dado)
{
    if (!f->faltam) return false;
    if (dado) memcpy(dado, elemento(f, f->proximo), f->largura);
    f->faltam--;
    if (f->faltam) {
        if (f->reverso) f->proximo--;
        else f->proximo++;
    }
    return true;
}
