# Parte III — estruturas de dados no treinamento da cobrinha

## Histórico de inovações

Em `individuo.c`, a função `id_neurônio_entre` recebe os IDs de dois neurônios e procura o ID que já foi usado para dividir a sinapse entre eles. Isso permite reconhecer a mesma inovação em indivíduos diferentes. A busca percorre um vetor com até 10 mil registros. Quando ele enche, a função continua gerando IDs, mas perde o registro das novas divisões. Uma segunda consulta ao mesmo par pode então produzir outro ID.

Um dicionário com o par ordenado (origem, destino) como chave guardaria o ID intermediário. A direção faz diferença, por isso as duas posições da chave precisam ser preservadas. Com uma tabela hash que cresce e redistribui os pares, a consulta tem custo médio constante, e uma nova divisão continua registrada mesmo depois da décima milésima entrada.

## Identificação dos neurônios

`indivíduo_sinapse_com_ids` percorre os neurônios de uma rede para converter os dois IDs em posições. A função `compatibilidade` repete essa busca para cada sinapse que examina na outra rede. Mesmo em uma rede com poucas ligações por neurônio, cada consulta pode voltar a percorrer todos os neurônios.

Um dicionário por rede, de ID para posição, permitiria localizar os dois extremos em tempo médio constante, antes de procurar a aresta na lista de saída. A inserção acrescentaria uma entrada ao dicionário. Ao excluir um nó, seria necessário remover seu ID e atualizar as posições dos nós deslocados. Assim, o custo de acertar as posições ficaria na remoção e as sucessivas comparações entre indivíduos usariam o índice pronto.

## Indivíduos por espécie

Em `especie.c`, cada espécie reserva `MAX_IND_ESP` ponteiros, mesmo quando contém apenas seu fundador. Em `config.h`, esse limite é igual ao tamanho de toda a população. Com 500 indivíduos distribuídos em 100 espécies, são 50 mil posições reservadas para guardar 500 referências.

Um vetor dinâmico por espécie poderia começar pequeno e dobrar de capacidade quando enchesse. O acesso por índice continuaria direto para o sorteio e o mesmo `qsort` poderia ordenar a parte ocupada. A espécie liberaria o vetor ao ser destruída. A geração continuaria responsável por liberar os indivíduos.

## Uma operação para percorrer todas as arestas

Em `indivíduo_mistura_sinapses`, o cruzamento percorre todos os neurônios e inicia uma consulta de saídas para cada um. O que ele precisa, a cada passo, é da origem, do destino e do valor de uma sinapse. Na API atual, o estado do percurso fica dentro do grafo. Uma segunda consulta ao mesmo grafo substitui a primeira.

Uma operação como `g_para_cada_aresta(g, visita, contexto)` poderia percorrer as listas de saída e passar à função de visita a origem, o destino, o ponteiro para o valor e o contexto do cruzamento. O custo continuaria proporcional ao número de vértices mais o número de arestas. A vantagem seria percorrer cada aresta uma vez com um único laço no treinador, permitindo consultas independentes durante a visita. A topologia do grafo percorrido teria de permanecer fixa até o término desse percurso.
