#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "image.h"

typedef struct {
    int linha;
    int coluna;
} Position;

typedef struct Node {
    Position posicao;
    struct Node *proximo;
} Node;

typedef struct {
    Node *topo;
} Stack;

typedef struct {
    Node *inicio;
    Node *fim;
} Queue;

Node *criarNo(Position posicao) {
    Node *no = malloc(sizeof *no);
    if (no != NULL) {
        no->posicao = posicao;
        no->proximo = NULL;
    }
    return no;
}

int empilhar(Stack *pilha, Position posicao) {
    Node *no = criarNo(posicao);
    if (no == NULL) {
        return 0;
    }
    no->proximo = pilha->topo;
    pilha->topo = no;
    return 1;
}

int desempilhar(Stack *pilha, Position *posicao) {
    if (pilha->topo == NULL) {
        return 0;
    }
    Node *no = pilha->topo;
    *posicao = no->posicao;
    pilha->topo = no->proximo;
    free(no);
    return 1;
}

int enfileirar(Queue *fila, Position posicao) {
    Node *no = criarNo(posicao);
    if (no == NULL) {
        return 0;
    }
    if (fila->fim == NULL) {
        fila->inicio = no;
    } else {
        fila->fim->proximo = no;
    }
    fila->fim = no;
    return 1;
}

int desenfileirar(Queue *fila, Position *posicao) {
    if (fila->inicio == NULL) {
        return 0;
    }
    Node *no = fila->inicio;
    *posicao = no->posicao;
    fila->inicio = no->proximo;
    if (fila->inicio == NULL) {
        fila->fim = NULL;
    }
    free(no);
    return 1;
}

void liberarPilha(Stack *pilha) {
    Position posicao;
    while (desempilhar(pilha, &posicao)) {
    }
}

void liberarFila(Queue *fila) {
    Position posicao;
    while (desenfileirar(fila, &posicao)) {
    }
}

void demonstrarEstruturas(void) {
    Stack pilha = {NULL};
    Queue fila = {NULL, NULL};
    for (int i = 0; i < 3; i++) {
        Position posicao = {i, i};
        if (!empilhar(&pilha, posicao) || !enfileirar(&fila, posicao)) {
            puts("Memoria insuficiente.");
            liberarPilha(&pilha);
            liberarFila(&fila);
            return;
        }
    }
    puts("Inseridas as posicoes (0,0), (1,1), (2,2).");
    Position posicao;
    printf("Retirada da pilha: ");
    while (desempilhar(&pilha, &posicao)) {
        printf("(%d,%d) ", posicao.linha, posicao.coluna);
    }
    printf("\nRetirada da fila: ");
    while (desenfileirar(&fila, &posicao)) {
        printf("(%d,%d) ", posicao.linha, posicao.coluna);
    }
    puts("\nEstruturas vazias: retirada retorna 0.");
    /* Reutilizar depois de esvaziar verifica o reinicio dos ponteiros. */
    Position nova = {3, 3};
    if (!empilhar(&pilha, nova) || !enfileirar(&fila, nova)) {
        puts("Memoria insuficiente.");
    } else {
        puts("Estruturas reutilizadas com a posicao (3,3).");
    }
    liberarPilha(&pilha);
    liberarFila(&fila);
}

int lerLinha(char *entrada, int capacidade) {
    if (fgets(entrada, capacidade, stdin) == NULL) {
        return 0;
    }
    char *fim = strchr(entrada, '\n');
    if (fim != NULL) {
        *fim = '\0';
    } else {
        int c = getchar();
        if (c != '\n' && c != EOF) {
            while (c != '\n' && c != EOF) {
                c = getchar();
            }
            entrada[0] = '\0';
            puts("Entrada muito longa.");
        }
    }
    return 1;
}

int main(void) {
    Image imagem = {0, 0, NULL};
    char entrada[1024];
    for (;;) {
        printf("\nFlood Fill\n1 - Executar com pilha\n2 - Executar com fila\n3 - Escolher imagem\n4 - Escolher coordenada\n5 - Demonstrar pilha e fila\n6 - Salvar copia da imagem\n0 - Encerrar\nOpcao: ");
        if (!lerLinha(entrada, sizeof entrada)) {
            break;
        }
        char *fim;
        errno = 0;
        long valor = strtol(entrada, &fim, 10);
        if (fim == entrada || errno == ERANGE || valor < INT_MIN || valor > INT_MAX) {
            puts("Digite uma opcao numerica valida.");
            continue;
        }
        while (*fim == ' ' || *fim == '\t') {
            fim++;
        }
        if (*fim != '\0') {
            puts("Digite uma opcao numerica valida.");
            continue;
        }
        int opcao = (int)valor;
        if (opcao == 0) {
            break;
        }
        if (opcao == 5) {
            demonstrarEstruturas();
        } else if (opcao == 3) {
            printf("Caminho da imagem BMP (sem aspas): ");
            if (!lerLinha(entrada, sizeof entrada)) {
                break;
            }
            if (abrirBMP(entrada, &imagem)) {
                printf("Imagem carregada: %d x %d pixels.\n", imagem.largura, imagem.altura);
            }
        } else if (opcao == 6) {
            if (imagem.pixels == NULL) {
                puts("Escolha uma imagem primeiro pela opcao 3.");
                continue;
            }
            printf("Caminho da copia BMP (use outro nome): ");
            if (!lerLinha(entrada, sizeof entrada)) {
                break;
            }
            puts(salvarBMP(entrada, &imagem) ? "Copia salva." : "Nao foi possivel salvar a copia.");
        } else if (opcao >= 1 && opcao <= 4) {
            puts("Funcionalidade ainda em desenvolvimento.");
        } else {
            puts("Opcao invalida.");
        }
    }
    liberarImagem(&imagem);
    return EXIT_SUCCESS;
}