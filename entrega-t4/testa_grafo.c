/*
 * programa simples de teste do grafo.
 * cria um grafo com um exemplo visto em aula, calcula a ordem topológica.
 * imprime os nós do grafo, as arestas que partem de cada nó, as arestas que chegam
 *   em cada nó, a ordem topológica dos nós.
 * abaixo está um exemplo de saída do programa (a saída pode ser diferente, porque as
 *   arestas podem ser entregues em outra ordem, e existem várias ordens topológicas
 *   válidas para esse grafo).
7 vértices:
0: 'sapato'
1: 'meia'
2: 'calça'
3: 'cinto'
4: 'casaco'
5: 'cueca'
6: 'camisa'
arestas (partindo):
origem 0('sapato')
origem 1('meia')
  destino 0(sapato): 'meia->sapato'
origem 2('calça')
  destino 0(sapato): 'calça->sapato'
  destino 3(cinto): 'calça->cinto'
origem 3('cinto')
origem 4('casaco')
origem 5('cueca')
  destino 2(calça): 'cueca->calça'
origem 6('camisa')
  destino 3(cinto): 'camisa->cinto'
  destino 4(casaco): 'camisa->casaco'
arestas (chegando):
destino 0('sapato')
  origem 1(meia): 'meia->sapato'
  origem 2(calça): 'calça->sapato'
destino 1('meia')
destino 2('calça')
  origem 5(cueca): 'cueca->calça'
destino 3('cinto')
  origem 2(calça): 'calça->cinto'
  origem 6(camisa): 'camisa->cinto'
destino 4('casaco')
  origem 6(camisa): 'camisa->casaco'
destino 5('cueca')
destino 6('camisa')
Ordem topológica:
1º nó 1('meia')
2º nó 5('cueca')
3º nó 6('camisa')
4º nó 2('calça')
5º nó 4('casaco')
6º nó 0('sapato')
7º nó 3('cinto')
*/

#include "grafo.h"
#include "fila.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

Grafo cria_grafo()
{
  Grafo g = g_cria(7, 15);
  char s[20] = {0};
  strcpy(s, "sapato");
  g_insere_nó(g, s);
  strcpy(s, "meia");
  g_insere_nó(g, s);
  strcpy(s, "calça");
  g_insere_nó(g, s);
  g_insere_nó(g, (char[7]){"cinto"});
  g_insere_nó(g, (char[7]){"casaco"});
  g_insere_nó(g, (char[7]){"cueca"});
  g_insere_nó(g, (char[7]){"camisa"});
  strcpy(s, "meia->sapato");
  g_altera_valor_da_aresta(g, 1, 0, s);
  strcpy(s, "calça->sapato");
  g_altera_valor_da_aresta(g, 2, 0, s);
  g_altera_valor_da_aresta(g, 5, 2, (char[15]){"cueca->calça"});
  g_altera_valor_da_aresta(g, 2, 3, (char[15]){"calça->cinto"});
  g_altera_valor_da_aresta(g, 6, 4, (char[15]){"camisa->casaco"});
  g_altera_valor_da_aresta(g, 6, 3, (char[15]){"camisa->cinto"});
  return g;
}

void imprime_grafo(Grafo g)
{
  printf("%d vértices:\n", g_num_nós(g));
  for (int n = 0; n < g_num_nós(g); n++) {
    char *val_nó = g_valor_do_nó(g, n);
    printf("%d: '%s'\n", n, val_nó);
  }
  printf("arestas (partindo):\n");
  for (int n = 0; n < g_num_nós(g); n++) {
    char *val_nó;
    int n2;
    char *val_nó2;
    char *val_aresta;
    val_nó = g_valor_do_nó(g, n);
    printf("origem %d('%s')\n", n, val_nó);
    g_arestas_que_partem(g, n);
    while ((val_aresta = g_próxima_aresta(g, &n2)) != NULL) {
      val_nó2 = g_valor_do_nó(g, n2);
      printf("  destino %d(%s): '%s'\n", n2, val_nó2, val_aresta);
    }
  }
  printf("arestas (chegando):\n");
  for (int n = 0; n < g_num_nós(g); n++) {
    char *val_nó;
    int n2;
    char *val_nó2;
    char *val_aresta;
    val_nó = g_valor_do_nó(g, n);
    printf("destino %d('%s')\n", n, val_nó);
    g_arestas_que_chegam(g, n);
    while ((val_aresta = g_próxima_aresta(g, &n2)) != NULL) {
      val_nó2 = g_valor_do_nó(g, n2);
      printf("  origem %d(%s): '%s'\n", n2, val_nó2, val_aresta);
    }
  }
}

void imprime_fila(Fila f, Grafo g)
{
  int nó;
  int ordem = 1;
  printf("Ordem topológica:\n");
  f_inicia_percurso(f, 0);
  while (f_próximo(f, &nó)) {
    char *val_nó;
    val_nó = g_valor_do_nó(g, nó);
    printf("%dº nó %d('%s')\n", ordem, nó, val_nó);
    ordem++;
  }
}

static void testa_fila(void)
{
  Fila f = f_cria(sizeof(int));
  int n;
  for (int i = 0; i < 8; i++) f_insere(f, &i);
  for (int i = 0; i < 4; i++) {
    f_remove(f, &n);
    assert(n == i);
  }
  for (int i = 8; i < 20; i++) f_insere(f, &i);
  f_inicia_percurso(f, -2);
  for (int i = 18; i >= 4; i--) {
    assert(f_próximo(f, &n) && n == i);
  }
  assert(!f_próximo(f, NULL));
  f_inicia_percurso(f, 1);
  assert(f_próximo(f, NULL));
  assert(f_próximo(f, &n) && n == 6);
  while (!f_tá_vazia(f)) f_remove(f, NULL);
  f_destrói(f);
}

static void testa_ciclos_e_remocao(void)
{
  Grafo g = g_cria(sizeof(int), sizeof(int));
  for (int i = 0; i < 4; i++) g_insere_nó(g, &i);
  int peso = 8;
  g_altera_valor_da_aresta(g, 3, 1, &peso);
  g_altera_valor_da_aresta(g, 1, 2, &peso);
  g_altera_valor_da_aresta(g, 2, 3, &peso);
  assert(g_tem_ciclo(g));
  Fila ordem = g_ordem_topológica(g);
  assert(f_tá_vazia(ordem));
  f_destrói(ordem);
  g_remove_nó(g, 2);
  assert(g_num_nós(g) == 3 && !g_tem_ciclo(g));
  assert(*(int *)g_valor_do_nó(g, 2) == 3);
  assert(*(int *)g_valor_da_aresta(g, 2, 1) == 8);
  int vizinho = -1;
  g_arestas_que_chegam(g, 1);
  assert(g_próxima_aresta(g, &vizinho) && vizinho == 2);
  assert(!g_próxima_aresta(g, &vizinho));
  g_altera_valor_da_aresta(g, 1, 1, &peso);
  assert(g_tem_ciclo(g));
  g_altera_valor_da_aresta(g, 1, 1, NULL);
  ordem = g_ordem_topológica(g);
  int pos[3], n;
  for (int i = 0; i < 3; i++) {
    assert(!f_tá_vazia(ordem));
    f_remove(ordem, &n);
    pos[n] = i;
  }
  assert(pos[2] < pos[1] && f_tá_vazia(ordem));
  f_destrói(ordem);
  g_destrói(g);
}

int main()
{
  testa_fila();
  testa_ciclos_e_remocao();
  Grafo g = cria_grafo();
  imprime_grafo(g);
  Fila f = g_ordem_topológica(g);
  imprime_fila(f, g);
  f_destrói(f);
  g_destrói(g);
  return 0;
}
