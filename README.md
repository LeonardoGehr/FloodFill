# Flood Fill

Trabalho TDE 2 de Estruturas de Dados, em C.

## Estado atual

Menu no terminal e estruturas proprias de pilha e fila encadeadas, armazenando coordenadas. A opcao 5 demonstra a ordem de retirada. A opcao 3 abre imagens BMP de 24 bits sem compressao; a opcao 6 salva uma copia. O preenchimento ainda nao foi implementado.

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