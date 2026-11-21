#include "str.h"
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN_ALLOC 8

// O vetor guarda os bytes UTF-8. Um caractere pode ocupar de um a quatro bytes.
struct str {
    byte *dados;
    int bytes;
    int caracteres;
    size_t capacidade;
};

// Confere se os contadores e a capacidade do vetor são coerentes depois de uma edição.
static void s_ok(Str_c s)
{
    assert(s && s->caracteres >= 0 && s->caracteres <= s->bytes);
    assert((s->dados == NULL) == (s->bytes == 0));
    assert((s->capacidade == 0) == (s->bytes == 0));
    assert(s->capacidade >= (size_t)s->bytes);
    assert((s->capacidade & (s->capacidade - 1)) == 0);
    assert(!s->bytes || s->capacidade >= MIN_ALLOC);
    assert(s->capacidade <= MIN_ALLOC || s->capacidade <= 3ULL * s->bytes);
}

// Escolhe a menor potência de dois que comporta os bytes, respeitando a alocação mínima.
static size_t capacidade_para(int bytes)
{
    size_t capacidade = MIN_ALLOC;
    if (bytes == 0) return 0;
    while (capacidade < (size_t)bytes) capacidade *= 2;
    return capacidade;
}

// Só realoca ao mudar de faixa de capacidade. Uma string vazia libera o vetor.
static void reserva(Str s, int bytes)
{
    size_t capacidade = capacidade_para(bytes);
    if (capacidade == s->capacidade) return;
    if (capacidade == 0) {
        free(s->dados);
        s->dados = NULL;
    } else {
        byte *dados = realloc(s->dados, capacidade);
        if (!dados) abort();
        s->dados = dados;
    }
    s->capacidade = capacidade;
}

// Valida a sequência inteira antes de copiar, para não guardar um trecho de UTF-8 inválido.
static void carrega(Str s, byte *dados, int bytes)
{
    int caracteres = bytes ? u8_conta_unichar_nos_bytes(bytes, dados) : 0;
    if (caracteres < 0) return;
    reserva(s, bytes);
    if (bytes) memcpy(s->dados, dados, bytes);
    s->bytes = bytes;
    s->caracteres = caracteres;
}

Str s_cria(char const *strC)
{
    Str s = calloc(1, sizeof(*s));
    if (!s) abort();
    if (strC) {
        size_t bytes = strlen(strC);
        if (bytes > INT_MAX) abort();
        carrega(s, (byte *)strC, (int)bytes);
    }
    return s;
}

void s_destroi(Str s)
{
    s_ok(s);
    free(s->dados);
    free(s);
}

// Pela convenção do TAD, -1 indica o fim e -2 indica o último caractere.
static long long posicao(Str_c s, int pos)
{
    if (pos < 0) return (long long)s->caracteres + 1 + pos;
    return pos;
}

static int limita(Str_c s, long long pos)
{
    if (pos < 0) return 0;
    if (pos > s->caracteres) return s->caracteres;
    return (int)pos;
}

// Avança caracteres inteiros até encontrar o byte onde começa a posição pedida.
static int indice_byte(Str_c s, int pos)
{
    if (pos == 0) return 0;
    if (pos == s->caracteres) return s->bytes;
    return (int)(u8_avanca_unichar(s->dados, pos) - s->dados);
}

int s_tam(Str_c s)
{
    s_ok(s);
    return s->caracteres;
}

// Acrescenta o terminador em uma cópia. O vetor interno pode conter um zero no meio.
char *s_strc(Str_c s)
{
    char *copia = malloc((size_t)s->bytes + 1);
    if (!copia) abort();
    if (s->bytes) memcpy(copia, s->dados, s->bytes);
    copia[s->bytes] = '\0';
    return copia;
}

// Localiza o primeiro byte e decodifica o caractere, que pode ocupar várias posições no vetor.
unichar s_ch(Str_c s, int pos)
{
    long long p = posicao(s, pos);
    unichar c;
    if (p < 0 || p >= s->caracteres) return UNI_INV;
    int i = indice_byte(s, (int)p);
    if (u8_unichar_nos_bytes(s->bytes - i, s->dados + i, &c) < 0) return UNI_INV;
    return c;
}

bool s_igual(Str_c s, Str_c sb)
{
    return s->bytes == sb->bytes &&
           (s->bytes == 0 || memcmp(s->dados, sb->dados, s->bytes) == 0);
}

// Recorta as duas pontas separadamente, inclusive quando o início é negativo.
static void intervalo(Str_c s, int pos, int tam, int *inicio, int *fim)
{
    long long p = posicao(s, pos);
    *inicio = limita(s, p);
    *fim = tam < 0 ? s->caracteres : limita(s, p + tam);
}

void s_substring(Str s, Str_c sb, int pos, int tam)
{
    int inicio, fim;
    intervalo(sb, pos, tam, &inicio, &fim);
    int primeiro = indice_byte(sb, inicio);
    int bytes = indice_byte(sb, fim) - primeiro;
    // Ao recortar a própria string, move o trecho antes de encolher o vetor.
    if (s != sb) reserva(s, bytes);
    if (bytes) memmove(s->dados, sb->dados + primeiro, bytes);
    if (s == sb) reserva(s, bytes);
    s->bytes = bytes;
    s->caracteres = fim - inicio;
    s_ok(s);
}

Str s_cria_substring(Str_c s, int pos, int tam)
{
    Str nova = s_cria(NULL);
    s_substring(nova, s, pos, tam);
    return nova;
}

Str s_cria_cópia(Str_c s)
{
    return s_cria_substring(s, 0, -1);
}

void s_copia(Str s, Str_c sb)
{
    s_substring(s, sb, 0, -1);
}

void s_substitui(Str s, int pos, int tam, Str_c sb)
{
    Str copia = NULL;
    int inicio, fim;
    intervalo(s, pos, tam, &inicio, &fim);
    int primeiro = indice_byte(s, inicio);
    int ultimo = indice_byte(s, fim);
    // Guarda a origem antes de mover ou realocar o vetor. Isso permite anexar a string a si mesma.
    if (s == sb) {
        copia = s_cria_cópia(s);
        sb = copia;
    }
    int inseridos = sb ? sb->bytes : 0;
    int restantes = s->bytes - (ultimo - primeiro);
    if (inseridos > INT_MAX - restantes) abort();
    int bytes = restantes + inseridos;
    if (bytes > s->bytes) reserva(s, bytes);
    // Desloca o sufixo para fechar ou abrir espaço. Os trechos de memória podem se sobrepor.
    if (ultimo < s->bytes) {
        memmove(s->dados + primeiro + inseridos, s->dados + ultimo, s->bytes - ultimo);
    }
    if (inseridos) memcpy(s->dados + primeiro, sb->dados, inseridos);
    if (bytes < s->bytes) reserva(s, bytes);
    s->bytes = bytes;
    s->caracteres += (sb ? sb->caracteres : 0) - (fim - inicio);
    if (copia) s_destroi(copia);
    s_ok(s);
}

void s_insere(Str s, int pos, Str_c sb)
{
    s_substitui(s, pos, 0, sb);
}

void s_insere_c(Str s, int pos, unichar c)
{
    byte dados[4];
    int n = u8_converte_pra_utf8(c, dados);
    if (n < 0) return;
    // A inserção copia os bytes enquanto o vetor local ainda existe.
    struct str letra = {dados, n, 1, sizeof(dados)};
    s_insere(s, pos, &letra);
}

void s_anexa(Str s, Str_c sb)
{
    s_insere(s, -1, sb);
}

void s_anexa_c(Str s, unichar c)
{
    s_insere_c(s, -1, c);
}

void s_remove(Str s, int pos, int tam)
{
    s_substitui(s, pos, tam, NULL);
}
