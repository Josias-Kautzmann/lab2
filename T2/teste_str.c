#include "str.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void confere(Str s, const char *esperado)
{
    Str outro = s_cria(esperado);
    assert(s_igual(s, outro));
    s_destroi(outro);
}

static void testa_edicao(void)
{
    Str s = s_cria("Rábica");
    Str trecho = s_cria_substring(s, 3, 2);
    confere(trecho, "ic");
    s_substring(trecho, s, 1, 2);
    confere(trecho, "áb");
    s_substitui(trecho, -2, 1, NULL);
    s_substitui(s, -7, 3, trecho);
    confere(s, "áica");
    s_copia(trecho, s);
    // Usa a mesma string como origem e destino para verificar se a edição preserva os bytes.
    s_anexa(s, s);
    confere(s, "áicaáica");
    s_substring(s, s, -10, 3);
    confere(s, "ái");
    s_insere(s, 99, trecho);
    confere(s, "áiáica");
    s_remove(s, 0, -1);
    s_anexa_c(s, 0x1f40d);
    assert(s_ch(s, -2) == 0x1f40d);
    assert(s_ch(s, -1) == UNI_INV && s_ch(s, -3) == UNI_INV);
    s_insere_c(s, 0, 0xd800);
    assert(s_tam(s) == 1);
    s_destroi(trecho);
    s_destroi(s);
}

static void testa_busca(void)
{
    Str s = s_cria(" .pé.pé. ");
    Str sobras = s_cria(" .");
    Str alvo = s_cria("pé");
    assert(s_busca_c(s, 2, sobras) == 4);
    assert(s_busca_nc(s, 0, sobras) == 2);
    // A busca reversa começa antes da posição 4, embora nessa posição também haja um ponto.
    assert(s_busca_rc(s, 4, sobras) == 1);
    assert(s_busca_rnc(s, -1, sobras) == 6);
    assert(s_busca_s(s, 3, alvo) == 5);
    s_apara(s, sobras);
    confere(s, "pé.pé");
    s_apara(s, s);
    assert(s_tam(s) == 0 && s_busca_s(alvo, -1, s) == 2);
    s_destroi(alvo);
    s_destroi(sobras);
    s_destroi(s);
}

static void testa_arquivo(char *nome)
{
    Str s = s_cria("a");
    s_anexa_c(s, 0);
    s_anexa_c(s, 0x20ac);
    s_grava_arquivo(s, nome);
    Str copia = s_cria_de_arquivo(nome);
    assert(s_tam(copia) == 3 && s_igual(s, copia));
    char *dados = s_strc(copia);
    // Compara os seis bytes, incluindo o zero interno e o terminador da cópia.
    assert(memcmp(dados, "a\0€", 6) == 0);
    free(dados);
    s_destroi(copia);
    FILE *arquivo = fopen(nome, "wb");
    assert(arquivo);
    // Um byte inicial isolado deixa o último caractere incompleto.
    fputc(0xf0, arquivo);
    fclose(arquivo);
    copia = s_cria_de_arquivo(nome);
    assert(s_tam(copia) == 0);
    remove(nome);
    s_destroi(copia);
    s_destroi(s);
}

int main(void)
{
    Str invalida = s_cria("\xc0\x80");
    assert(s_tam(invalida) == 0);
    s_destroi(invalida);
    testa_edicao();
    testa_busca();
    testa_arquivo("teste_str_temporario.bin");
    puts("Testes de string passaram.");
    return 0;
}
