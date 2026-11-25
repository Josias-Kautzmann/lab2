#include "auxiliar.h"

#include <math.h>
#include <stdlib.h>

#include <stdint.h>

// O treinamento usa um gerador separado do rand() usado pela cobrinha.
static uint64_t estado = UINT64_C(0x1234abcd330e);

void inicia_sorteio(long semente)
{
    estado = ((uint64_t)(uint32_t)semente << 16) | 0x330e;
}

static uint64_t sorteia_48(void)
{
    estado = (UINT64_C(0x5deece66d) * estado + 11) & UINT64_C(0xffffffffffff);
    return estado;
}

// usa rand48 em vez de rand ou random, para ter valores aleatórios
//   independentes para o treinamento e para o jogo

float al01()
{
  return (sorteia_48() >> 24) / 16777216.0f;
}

float gauss(float média, float desvio)
{
  float pi = acosf(-1);
  float x, y, z;
  do { x = al01(); } while (x == 0);
  y = al01();
  z = sqrtf(-2 * logf(x)) * cosf(2 * pi * y);
  return média + desvio * z;
}

int aleatório(int limite)
{
  return (int)((sorteia_48() >> 17) % limite);
}

int aleatório_p(int limite)
{
  if (limite <= 1) return 0;
  float alp01;
  do {
    alp01 = fabsf(gauss(0, 1));
  } while (alp01 >= 1);
  return alp01 * limite;
}

int aleatório_entre(int min, int max)
{
  return min + aleatório(max - min);
}

int max(int a, int b)
{
  if (a > b) return a;
  return b;
}

bool probabilidade(int percentual)
{
  return aleatório(100) < percentual;
}
