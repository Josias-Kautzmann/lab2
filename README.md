Repo para trabalhos lab2.
## Compilar e executar

Comece cada bloco na raiz do repositório. Com Clang, troque `gcc` por `clang`.

### T2 — strings UTF-8

```sh
cd T2
gcc -std=gnu11 -Wall -Wextra teste_str.c str.c utf8.c -o teste_str
./teste_str
```

### T3 — calculadora

```sh
cd T3
gcc -std=gnu11 -Wall -Wextra main.c calc.c lista.c str.c utf8.c dic_simples.c -lm -o calc
gcc -std=gnu11 -Wall -Wextra teste_calc.c calc.c lista.c str.c utf8.c dic_simples.c -lm -o teste_calc
./teste_calc
./calc entrada.txt saida.txt
```

Crie `entrada.txt` com uma expressão por linha. As linhas `x=2+3`, `x^2` e `1/0` produzem `5`, `25` e uma mensagem de erro. As variáveis duram até o fim da execução.

### T4 — cobrinha

```sh
cd entrega-t4
gcc -std=gnu11 testa_grafo.c grafo.c fila.c -o testa_grafo
gcc -std=gnu11 -O2 main.c jogo.c terminal.c fila.c grafo.c rede.c treina.c geracao.c especie.c individuo.c auxiliar.c -lm -o cobra
./testa_grafo
./cobra
```

Use `j` para virar à esquerda e `k` para virar à direita. Para jogar com as redes de exemplo, execute `./cobra rede1 42` ou `./cobra rede2 42`. O último argumento é a semente. Para treinar, execute `./cobra treina 42` em uma pasta de trabalho própria. As melhores redes ficam na subpasta `G`. A configuração original prevê 8.000 gerações de 500 indivíduos.

A análise do treinamento e a proposta para a API do grafo estão em [parte-iii.md](entrega-t4/parte-iii.md).
