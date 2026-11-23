#include "terminal.h"

#include <stdio.h>
#include <stdlib.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
static DWORD modo_saida;
static bool console_ativo;
#endif

// salva ou recupera o conteúdo da tela
static void t_seleciona_tela_alternativa(bool alt)
{
  if (alt) {
    printf("\x1b[?1049h");
  } else {
    printf("\x1b[?1049l");
  }
}

void t_configura()
{
#ifdef _WIN32
  HANDLE saida = GetStdHandle(STD_OUTPUT_HANDLE);
  console_ativo = GetConsoleMode(saida, &modo_saida) != 0;
  if (console_ativo) SetConsoleMode(saida, modo_saida | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
#endif
  // salva o conteúdo da tela
  t_seleciona_tela_alternativa(true);
#ifndef _WIN32
  // coloca o terminal em modo cru
  if (system("stty raw opost -echo min 0 time 1") != 0) {
    perror("erro na execução de system(\"stty\")");
    fprintf(stderr, "você tem o programa stty instalado?\n");
    exit(1);
  }
  // configura leitura não bufferizada
  if (setvbuf(stdin, NULL, _IONBF, 0) != 0) {
    perror("erro na execução de setvbuf()");
    exit(1);
  }
#endif
  t_mostra_cursor(false);
  t_limpa();
}

void t_normaliza()
{
#ifndef _WIN32
  system("stty sane");
#endif
  // recupera o conteúdo da tela
  t_seleciona_tela_alternativa(false);
  t_mostra_cursor(true);
#ifdef _WIN32
  fflush(stdout);
  if (console_ativo) SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), modo_saida);
#endif
}

void t_mostra_cursor(bool mostra)
{
  if (mostra) {
    printf("\x1b[?25h");
  } else {
    printf("\x1b[?25l");
  }
}

void t_limpa()
{
  t_posiciona((posição){1, 1});
  printf("\x1b[2J");
}

void t_posiciona(posição pos)
{
  printf("\x1b[%d;%dH", pos.linha, pos.coluna);
}

void t_seleciona_cor_normal()
{
  printf("\x1b[m");
}

void t_seleciona_cor(cor fundo, cor letra)
{
  printf("\x1b[48;2;%d;%d;%dm", fundo.vermelho, fundo.verde, fundo.azul);
  printf("\x1b[38;2;%d;%d;%dm", letra.vermelho, letra.verde, letra.azul);
}

char t_lê_tecla()
{
  // força o envio de tudo que foi escrito na tela ao SO
  fflush(stdout);
#ifdef _WIN32
  Sleep(100);
  if (console_ativo && _kbhit()) return (char)_getch();
#else
  // lê um caractere e retorna
  char c;
  if (fread(&c, 1, 1, stdin) == 1) return c;
#endif
  // se a leitura não deu certo, retorna 0
  return 0;
}
