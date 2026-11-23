#include "calc.h"
#include "dicionario.h"
#include <math.h>
#include <stdlib.h>
#include <string.h>

static bool digito(unichar c)
{
    return c >= '0' && c <= '9';
}

static bool letra(unichar c)
{
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

static bool parte_nome(unichar c)
{
    return letra(c) || digito(c) || c == '_' || c == '$';
}

Lista tokeniza(Str texto)
{
    Lista tokens = l_cria();
    int pos = 0;
    while (pos < s_tam(texto)) {
        int inicio = pos;
        unichar c = s_ch(texto, pos++);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') continue;
        if (digito(c) || c == '.') {
            while (digito(s_ch(texto, pos)) || s_ch(texto, pos) == '.') pos++;
        } else if (parte_nome(c)) {
            while (parte_nome(s_ch(texto, pos))) pos++;
        }
        l_insere_fim(tokens, s_cria_substring(texto, inicio, pos - inicio));
    }
    return tokens;
}

static bool numero_valido(Str s)
{
    int pontos = 0;
    int digitos = 0;
    for (int i = 0; i < s_tam(s); i++) {
        unichar c = s_ch(s, i);
        if (digito(c)) digitos++;
        else if (c == '.') pontos++;
        else return false;
    }
    return digitos > 0 && pontos <= 1;
}

static bool nome_valido(Str s)
{
    unichar c = s_ch(s, 0);
    if (!letra(c) && c != '$') return false;
    for (int i = 1; i < s_tam(s); i++) {
        c = s_ch(s, i);
        if (!letra(c) && !digito(c) && c != '_') return false;
    }
    return true;
}

static Dicionário variaveis;

static bool chaves_iguais(chave_t a, chave_t b)
{
    return s_igual(a, b);
}

static bool chave_menor(chave_t a, chave_t b)
{
    char *primeira = s_strc(a);
    char *segunda = s_strc(b);
    bool menor = strcmp(primeira, segunda) < 0;
    free(primeira);
    free(segunda);
    return menor;
}

static void libera_par(chave_t chave, valor_t valor)
{
    s_destroi(chave);
    s_destroi(valor);
}

static void encerra_variaveis(void)
{
    dic_para_todos(variaveis, libera_par);
    dic_destrói(variaveis);
}

static void guarda_valor(Str nome, Str valor)
{
    Str antigo = dic_busca(variaveis, nome);
    if (antigo) {
        s_copia(antigo, valor);
    } else {
        dic_insere(variaveis, s_cria_cópia(nome), s_cria_cópia(valor));
    }
}

static bool le_valor(Str dado, double *valor)
{
    if (nome_valido(dado)) {
        dado = dic_busca(variaveis, dado);
        if (!dado) return false;
    }
    *valor = s_número(dado);
    return true;
}

static const char *opera(Lista valores, char op)
{
    if (l_tam(valores) < 2) return "#ERRO faltou operando";
    Str direita = l_desempilha(valores);
    Str esquerda = l_desempilha(valores);
    double a, b, resultado = 0;
    const char *erro = NULL;
    if (!le_valor(direita, &b)) {
        erro = "#ERRO variavel sem valor";
    } else if (op == '=') {
        if (!nome_valido(esquerda)) erro = "#ERRO atribuicao exige um nome";
        resultado = b;
    } else if (!le_valor(esquerda, &a)) {
        erro = "#ERRO variavel sem valor";
    } else {
        switch (op) {
            case '+': resultado = a + b; break;
            case '-': resultado = a - b; break;
            case '*': resultado = a * b; break;
            case '/':
                if (b == 0) erro = "#ERRO divisao por zero";
                else resultado = a / b;
                break;
            case '^': resultado = pow(a, b); break;
        }
    }
    if (!erro && !isfinite(resultado)) erro = "#ERRO resultado fora do intervalo";
    if (!erro) {
        Str novo = s_cria_número(resultado);
        if (op == '=') guarda_valor(esquerda, novo);
        l_empilha(valores, novo);
    }
    s_destroi(esquerda);
    s_destroi(direita);
    return erro;
}

enum grupo { FIM, SOMA, PRODUTO, POTENCIA, ABRE, FECHA, ATRIBUI, DADO };

static enum grupo grupo_de(unichar c)
{
    switch (c) {
        case '+': case '-': return SOMA;
        case '*': case '/': return PRODUTO;
        case '^': return POTENCIA;
        case '(': return ABRE;
        case ')': return FECHA;
        case '=': return ATRIBUI;
        default: return DADO;
    }
}

// Linha = topo da pilha. Coluna = próximo operador da entrada.
static const char acoes[][8] = {
    "TEEEEXE",  // pilha vazia
    "OOEEEOE",  // + -
    "OOOEEOE",  // * /
    "OOOEEOE",  // ^
    "XEEEEDE",  // (
    "",         // ) nunca fica na pilha
    "OEEEEOE"   // =
};

static const char *processa_operador(Lista ops, Lista valores, enum grupo entrada, Str token)
{
    for (;;) {
        Str topo = l_topo(ops);
        enum grupo linha = topo ? grupo_de(s_ch(topo, 0)) : FIM;
        char acao = acoes[linha][entrada];
        if (acao == 'X') return topo ? "#ERRO faltou )" : "#ERRO faltou (";
        if (acao == 'T') return NULL;
        if (acao == 'E') {
            l_empilha(ops, s_cria_cópia(token));
            return NULL;
        }
        Str retirado = l_desempilha(ops);
        char op = (char)s_ch(retirado, 0);
        s_destroi(retirado);
        if (acao == 'D') return NULL;
        const char *erro = opera(valores, op);
        if (erro) return erro;
    }
}

Str calculadora(Str expressao)
{
    if (!variaveis) {
        variaveis = dic_cria(chave_menor, chaves_iguais);
        atexit(encerra_variaveis);
    }
    Lista tokens = tokeniza(expressao);
    Lista ops = l_cria();
    Lista valores = l_cria();
    bool espera_valor = true;
    const char *erro = NULL;
    while (!l_vazia(tokens) && !erro) {
        Str token = l_remove_inicio(tokens);
        enum grupo grupo = grupo_de(s_ch(token, 0));
        if (grupo == DADO) {
            if (!espera_valor) erro = "#ERRO faltou operador";
            else if (!numero_valido(token) && !nome_valido(token)) erro = "#ERRO operando invalido";
            else l_empilha(valores, s_cria_cópia(token));
            espera_valor = false;
        } else if (espera_valor && grupo != ABRE) {
            erro = "#ERRO faltou operando";
        } else if (!espera_valor && grupo == ABRE) {
            erro = "#ERRO faltou operador";
        } else {
            erro = processa_operador(ops, valores, grupo, token);
            espera_valor = grupo != FECHA;
        }
        s_destroi(token);
    }
    if (!erro && espera_valor) erro = "#ERRO faltou operando";
    if (!erro) erro = processa_operador(ops, valores, FIM, NULL);
    double resultado = 0;
    if (!erro && l_tam(valores) != 1) erro = "#ERRO expressao incompleta";
    if (!erro && !le_valor(l_topo(valores), &resultado)) erro = "#ERRO variavel sem valor";
    if (!erro && !isfinite(resultado)) erro = "#ERRO resultado fora do intervalo";
    l_destroi(valores);
    l_destroi(ops);
    l_destroi(tokens);
    return erro ? s_cria(erro) : s_cria_número(resultado);
}
