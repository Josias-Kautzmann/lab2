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
