#include "calc.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void testa(const char *expressao, const char *esperado)
{
    Str entrada = s_cria(expressao);
    Str resultado = calculadora(entrada);
    char *texto = s_strc(resultado);
    if (esperado) assert(strcmp(texto, esperado) == 0);
    else assert(strncmp(texto, "#ERRO ", 6) == 0);
    free(texto);
    s_destroi(resultado);
    s_destroi(entrada);
}

static void testa_lista(void)
{
    Str texto = s_cria("a,ba,ca, te");
    Str separadores = s_cria(", ");
    Str virgula = s_cria(",");
    Lista partes = l_cria_separando(texto, separadores);
    assert(l_tam(partes) == 4);
    l_insere_pos(partes, s_cria("x"), 2);
    Str retirado = l_remove_pos(partes, 3);
    assert(s_ch(retirado, 0) == 'c');
    Str unida = s_cria_unindo(partes, virgula);
    Str esperado = s_cria("a,ba,x,te");
    assert(s_igual(unida, esperado));
    s_destroi(esperado);
    s_destroi(unida);
    s_destroi(retirado);
    l_destroi(partes);
    s_destroi(virgula);
    s_destroi(separadores);
    s_destroi(texto);
}

int main(void)
{
    testa_lista();
    testa("2+3*4", "14");
    testa("(2+3)*4", "20");
    testa("2^3^2", "512");
    testa("3-6-2", "-5");
    testa("a=b=7", "7");
    testa("b=(0-7)/2", "-3.5");
    testa("a+b", "3.5");
    testa("$x_1=.5", "0.5");
    testa("a=3", "3");
    testa("b*$x_1", "-1.75");
    testa("desconhecida", NULL);
    testa("1..3", NULL);
    testa("3 4", NULL);
    testa("()", NULL);
    testa("1/(3-3)", NULL);
    testa("(1+2", NULL);
    testa("1+2)", NULL);
    testa("1=2", NULL);
    testa("1+", NULL);
    testa("", NULL);
    puts("Testes de lista e calculadora passaram.");
    return 0;
}
