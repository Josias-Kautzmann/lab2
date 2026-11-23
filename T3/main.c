#include "calc.h"
#include <stdio.h>
#include <stdlib.h>

// Uma linha vazia também é uma expressão e recebe sua própria resposta.
static Lista separa_linhas(Str texto)
{
    Lista linhas = l_cria();
    Str quebra = s_cria("\n");
    int inicio = 0;
    int tamanho = s_tam(texto);
    while (inicio < tamanho) {
        int fim = s_busca_c(texto, inicio, quebra);
        if (fim < 0) fim = tamanho;
        l_insere_fim(linhas, s_cria_substring(texto, inicio, fim - inicio));
        if (fim == tamanho) break;
        inicio = fim + 1;
    }
    s_destroi(quebra);
    return linhas;
}

static bool grava_respostas(Lista respostas, char *nome)
{
    FILE *arquivo = fopen(nome, "wb");
    if (!arquivo) return false;
    bool gravou = true;
    while (!l_vazia(respostas) && gravou) {
        Str resposta = l_remove_inicio(respostas);
        char *texto = s_strc(resposta);
        gravou = fprintf(arquivo, "%s\n", texto) >= 0;
        free(texto);
        s_destroi(resposta);
    }
    return fclose(arquivo) == 0 && gravou;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Uso: %s entrada.txt saida.txt\n", argv[0]);
        return 1;
    }
    FILE *arquivo = fopen(argv[1], "rb");
    if (!arquivo) {
        perror(argv[1]);
        return 1;
    }
    bool vazio = fgetc(arquivo) == EOF && !ferror(arquivo);
    fclose(arquivo);
    Str texto = s_cria_de_arquivo(argv[1]);
    if (!vazio && s_tam(texto) == 0) {
        fprintf(stderr, "Falha na leitura do arquivo UTF-8.\n");
        s_destroi(texto);
        return 1;
    }
    Lista linhas = separa_linhas(texto);
    Lista respostas = l_cria();
    s_destroi(texto);
    while (!l_vazia(linhas)) {
        Str linha = l_remove_inicio(linhas);
        l_insere_fim(respostas, calculadora(linha));
        s_destroi(linha);
    }
    bool gravou = grava_respostas(respostas, argv[2]);
    l_destroi(linhas);
    l_destroi(respostas);
    if (!gravou) perror(argv[2]);
    return gravou ? 0 : 1;
}
