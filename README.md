# Flood Fill

Trabalho TDE 2 de Estruturas de Dados, em C.

## Estado atual

Menu no terminal e estruturas proprias de pilha e fila encadeadas, armazenando coordenadas. A opcao 5 demonstra a ordem de retirada. A opcao 3 abre imagens BMP de 24 bits sem compressao; a opcao 6 salva uma copia. As opcoes 1 e 2 executam Flood Fill iterativo com pilha e fila proprias e salvam as etapas em BMP.

## Compilar e executar

Com GCC instalado, a partir desta pasta:

    gcc -std=c11 -Wall -Wextra -Wpedantic src/main.c src/image.c -o programa.exe
    .\programa.exe


## Estruturas

Position guarda linha e coluna. Node guarda uma posicao e o ponteiro para o proximo no.
Stack guarda o topo: insercao e retirada no topo (ultimo a entrar, primeiro a sair).
Queue guarda inicio e fim: insercao no fim e retirada no inicio (primeiro a entrar, primeiro a sair).
Cada insercao aloca um no; cada retirada libera esse no. Falha de alocacao e retirada de estrutura vazia retornam 0.
Ao retirar o ultimo elemento da fila, ambos os ponteiros ficam nulos. As funcoes de limpeza liberam todos os nos restantes.

## Teste manual

Escolher 5: a pilha deve retirar (2,2), (1,1), (0,0); a fila deve retirar (0,0), (1,1), (2,2). Repetir a opcao e verificar a reutilizacao. Escolher 0 para encerrar.

## Imagens

image.h define Color (RGB) e Image (largura, altura e pixels). image.c abre e salva BMP de 24 bits sem compressao, considerando a orientacao e o alinhamento das linhas. Outros formatos sao rejeitados.
A linha 0 fica no topo e a coluna 0 na esquerda. O indice de um pixel e linha * largura + coluna.
Escolher 3 e informar imagem_teste.bmp. Deve mostrar 9 x 9 pixels. Escolher 6 e salvar como copia_teste.bmp; abrir ambas e conferir se sao iguais. Ao falhar a abertura, a imagem anterior e preservada.
No MSYS2 UCRT64, executar ./programa.exe.

## Referencias e apoio

Formato BMP: https://learn.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapinfoheader

## Executar o preenchimento

1. Opcao 3: abrir imagem_teste.bmp.
2. Opcao 4: informar X=4 e Y=4. Coordenadas iniciam em 0, com origem no canto superior esquerdo.
3. Opcao 1: informar vermelho=0, verde=0, azul=255. Deve preencher 9 pixels internos.
4. Opcao 2: repetir a mesma cor. Deve preencher os mesmos 9 pixels, partindo da imagem original.
5. Abrir original.bmp, passo_0001.bmp e demais passos, e final.bmp nas pastas pilha_0001 e fila_0001. Os finais devem ser iguais e as etapas podem diferir.
6. Testar X=0,Y=0: preenche 56 pixels externos, sem atravessar o contorno vermelho.
7. Testar X=2,Y=2: preenche os 16 pixels do contorno vermelho.
8. No interior branco, nova cor 255,255,255 deve encerrar sem iniciar o preenchimento.
9. Coordenadas fora dos limites, cores fora de 0-255 e execucao sem imagem/coordenada devem ser rejeitadas.

O algoritmo retira uma posicao, verifica limites e cor, pinta e insere cima, baixo, esquerda e direita. A nova cor marca a visita; nao utiliza recursao nem lista de visitados. Cada execucao trabalha sobre uma copia e usa uma pasta nova, sem sobrescrever execucoes anteriores. A escolha de nova imagem exige selecionar novamente a coordenada.

Sao salvas a imagem original, etapas numeradas e a imagem final. Ate 100 pixels, cada pixel pintado gera um quadro; para imagens maiores, o intervalo e max(1, quantidade total de pixels / 100), salvando tambem o primeiro pixel. As pastas geradas sao excluidas do Git; a imagem usada nos testes e incluida. Se houver erro de memoria ou gravacao, a execucao e interrompida, liberando a memoria, e a pasta pode conter resultados parciais.