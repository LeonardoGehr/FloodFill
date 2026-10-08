#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include "image.h"
#ifdef _WIN32
#include <direct.h>
#define criarPasta(nome) _mkdir(nome)
#else
#include <sys/stat.h>
#define criarPasta(nome) mkdir(nome, 0777)
#endif

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

int coresIguais(Color a, Color b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

int salvarEtapa(const Image *imagem, const char *pasta, size_t etapa) {
    char caminho[128];
    snprintf(caminho, sizeof caminho, "%s/passo_%04zu.bmp", pasta, etapa);
    return salvarBMP(caminho, imagem);
}

void executarFloodFill(const Image *original, Position inicio, Color novaCor, int usarPilha) {
    if (inicio.linha < 0 || inicio.linha >= original->altura ||
        inicio.coluna < 0 || inicio.coluna >= original->largura) {
        puts("Coordenada fora da imagem.");
        return;
    }
    Color corOriginal = original->pixels[(size_t)inicio.linha * (size_t)original->largura + (size_t)inicio.coluna];
    if (coresIguais(corOriginal, novaCor)) {
        puts("A nova cor e igual a original; nenhum preenchimento necessario.");
        return;
    }
    size_t quantidade = (size_t)original->largura * (size_t)original->altura;
    Image trabalho = {original->largura, original->altura, malloc(quantidade * sizeof(Color))};
    if (trabalho.pixels == NULL) {
        puts("Memoria insuficiente.");
        return;
    }
    memcpy(trabalho.pixels, original->pixels, quantidade * sizeof(Color));
    char pasta[64];
    int pastaCriada = 0;
    for (unsigned int tentativa = 1; tentativa <= 999999; tentativa++) {
        snprintf(pasta, sizeof pasta, "%s_%04u", usarPilha ? "pilha" : "fila", tentativa);
        if (criarPasta(pasta) == 0) {
            pastaCriada = 1;
            break;
        }
        if (errno != EEXIST) {
            break;
        }
    }
    if (!pastaCriada) {
        puts("Nao foi possivel criar a pasta de resultados.");
        liberarImagem(&trabalho);
        return;
    }
    char caminho[128];
    snprintf(caminho, sizeof caminho, "%s/original.bmp", pasta);
    Stack pilha = {NULL};
    Queue fila = {NULL, NULL};
    size_t pintados = 0;
    size_t etapa = 0;
    /* Pequenas imagens: cada pixel. Grandes: cerca de 100 quadros. */
    size_t intervalo = quantidade > 100 ? quantidade / 100 : 1;
    int sucesso = salvarBMP(caminho, original);
    if (sucesso) {
        sucesso = usarPilha ? empilhar(&pilha, inicio) : enfileirar(&fila, inicio);
    }
    Position atual;
    while (sucesso && (usarPilha ? desempilhar(&pilha, &atual) : desenfileirar(&fila, &atual))) {
        if (atual.linha < 0 || atual.linha >= trabalho.altura ||
            atual.coluna < 0 || atual.coluna >= trabalho.largura) {
            continue;
        }
        size_t indice = (size_t)atual.linha * (size_t)trabalho.largura + (size_t)atual.coluna;
        if (!coresIguais(trabalho.pixels[indice], corOriginal)) {
            continue;
        }
        trabalho.pixels[indice] = novaCor;
        pintados++;
        if (pintados % intervalo == 0 || pintados == 1) {
            sucesso = salvarEtapa(&trabalho, pasta, ++etapa);
            if (!sucesso) {
                break;
            }
        }
        Position vizinhos[4] = {
            {atual.linha - 1, atual.coluna},
            {atual.linha + 1, atual.coluna},
            {atual.linha, atual.coluna - 1},
            {atual.linha, atual.coluna + 1}
        };
        for (int i = 0; i < 4; i++) {
            sucesso = usarPilha ? empilhar(&pilha, vizinhos[i]) : enfileirar(&fila, vizinhos[i]);
            if (!sucesso) {
                break;
            }
        }
    }
    if (sucesso) {
        snprintf(caminho, sizeof caminho, "%s/final.bmp", pasta);
        sucesso = salvarBMP(caminho, &trabalho);
    }
    liberarPilha(&pilha);
    liberarFila(&fila);
    liberarImagem(&trabalho);
    if (sucesso) {
        printf("Preenchidos %zu pixels. Imagens salvas em %s.\n", pintados, pasta);
    } else {
        printf("Execucao interrompida por falha de memoria ou gravacao. Resultados parciais em %s.\n", pasta);
    }
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

int pedirInteiro(const char *mensagem, int minimo, int maximo, int *resultado) {
    char entrada[128];
    printf("%s", mensagem);
    if (!lerLinha(entrada, sizeof entrada)) {
        return 0;
    }
    char *fim;
    errno = 0;
    long valor = strtol(entrada, &fim, 10);
    if (fim == entrada || errno == ERANGE || valor < minimo || valor > maximo) {
        printf("Informe um numero entre %d e %d.\n", minimo, maximo);
        return 0;
    }
    while (*fim == ' ' || *fim == '\t') {
        fim++;
    }
    if (*fim != '\0') {
        puts("Entrada numerica invalida.");
        return 0;
    }
    *resultado = (int)valor;
    return 1;
}
int main(void) {
    Image imagem = {0, 0, NULL};
    Position inicio = {0, 0};
    int coordenadaEscolhida = 0;
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
                coordenadaEscolhida = 0;
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
        } else if (opcao == 4) {
            if (imagem.pixels == NULL) {
                puts("Escolha uma imagem primeiro pela opcao 3.");
                continue;
            }
            int x, y;
            if (pedirInteiro("X (coluna, iniciando em 0): ", 0, imagem.largura - 1, &x) &&
                pedirInteiro("Y (linha, iniciando em 0): ", 0, imagem.altura - 1, &y)) {
                inicio.coluna = x;
                inicio.linha = y;
                coordenadaEscolhida = 1;
                printf("Coordenada escolhida: X=%d, Y=%d.\n", x, y);
            }
        } else if (opcao == 1 || opcao == 2) {
            if (imagem.pixels == NULL || !coordenadaEscolhida) {
                puts("Escolha a imagem (3) e a coordenada (4) primeiro.");
                continue;
            }
            int r, g, b;
            if (pedirInteiro("Vermelho (0-255): ", 0, 255, &r) &&
                pedirInteiro("Verde (0-255): ", 0, 255, &g) &&
                pedirInteiro("Azul (0-255): ", 0, 255, &b)) {
                Color cor = {(unsigned char)r, (unsigned char)g, (unsigned char)b};
                executarFloodFill(&imagem, inicio, cor, opcao == 1);
            }
        } else {
            puts("Opcao invalida.");
        }
    }
    liberarImagem(&imagem);
    return EXIT_SUCCESS;
}