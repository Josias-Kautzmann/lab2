#include "lista.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct no {
    Str dado;
    struct no *anterior;
    struct no *proximo;
} No;

struct lista {
    No sentinela;
    int tamanho;
};

Lista l_cria(void)
{
    Lista l = calloc(1, sizeof(*l));
    if (!l) abort();
    l->sentinela.proximo = &l->sentinela;
    l->sentinela.anterior = &l->sentinela;
    return l;
}

int l_tam(Lista l)
{
    return l->tamanho;
}

bool l_vazia(Lista l)
{
    return l->tamanho == 0;
}

bool l_cheia(Lista l)
{
    return l->tamanho == INT_MAX;
}

// A posição logo após o último elemento corresponde ao sentinela.
static No *no_na_posicao(Lista l, int pos)
{
    No *no;
    if (pos < l->tamanho / 2) {
        no = l->sentinela.proximo;
        for (int i = 0; i < pos; i++) no = no->proximo;
    } else {
        no = &l->sentinela;
        for (int i = l->tamanho; i > pos; i--) no = no->anterior;
    }
    return no;
}

void l_insere_pos(Lista l, dado_t dado, int pos)
{
    if (pos < 0 || pos > l->tamanho || l_cheia(l)) return;
    No *depois = no_na_posicao(l, pos);
    No *novo = malloc(sizeof(*novo));
    if (!novo) abort();
    novo->dado = dado;
    novo->proximo = depois;
    novo->anterior = depois->anterior;
    depois->anterior->proximo = novo;
    depois->anterior = novo;
    l->tamanho++;
}

dado_t l_remove_pos(Lista l, int pos)
{
    if (pos < 0 || pos >= l->tamanho) return NULL;
    No *no = no_na_posicao(l, pos);
    Str dado = no->dado;
    no->anterior->proximo = no->proximo;
    no->proximo->anterior = no->anterior;
    free(no);
    l->tamanho--;
    return dado;
}

dado_t l_dado_pos(Lista l, int pos)
{
    if (pos < 0 || pos >= l->tamanho) return NULL;
    return no_na_posicao(l, pos)->dado;
}

void l_insere_inicio(Lista l, dado_t d)
{
    l_insere_pos(l, d, 0);
}

void l_insere_fim(Lista l, dado_t d)
{
    l_insere_pos(l, d, l->tamanho);
}

dado_t l_remove_inicio(Lista l)
{
    return l_remove_pos(l, 0);
}

dado_t l_remove_fim(Lista l)
{
    return l_remove_pos(l, l->tamanho - 1);
}

dado_t l_dado_inicio(Lista l)
{
    return l_dado_pos(l, 0);
}

dado_t l_dado_fim(Lista l)
{
    return l_dado_pos(l, l->tamanho - 1);
}

dado_t l_primeiro(Lista l)
{
    return l_dado_inicio(l);
}

void l_insere(Lista l, dado_t d)
{
    l_insere_fim(l, d);
}

dado_t l_remove(Lista l)
{
    return l_remove_inicio(l);
}

dado_t l_topo(Lista l)
{
    return l_dado_fim(l);
}

void l_empilha(Lista l, dado_t d)
{
    l_insere_fim(l, d);
}

dado_t l_desempilha(Lista l)
{
    return l_remove_fim(l);
}

void l_imprime(Lista l)
{
    for (No *no = l->sentinela.proximo; no != &l->sentinela; no = no->proximo) {
        if (no != l->sentinela.proximo) putchar(' ');
        s_imprime(no->dado);
    }
}

void l_destroi(Lista l)
{
    while (!l_vazia(l)) s_destroi(l_remove_inicio(l));
    free(l);
}

Lista l_cria_separando(Str s, Str sep)
{
    Lista partes = l_cria();
    int inicio = s_busca_nc(s, 0, sep);
    while (inicio >= 0) {
        int fim = s_busca_c(s, inicio, sep);
        if (fim < 0) fim = s_tam(s);
        l_insere_fim(partes, s_cria_substring(s, inicio, fim - inicio));
        inicio = s_busca_nc(s, fim, sep);
    }
    return partes;
}
