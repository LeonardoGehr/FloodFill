#include <stdio.h>

int main(void) {
    int opcao;
    char entrada[64];
    for (;;) {
        printf("\nFlood Fill\n1 - Executar com pilha\n2 - Executar com fila\n3 - Escolher imagem\n4 - Escolher coordenada\n0 - Encerrar\nOpcao: ");
        if (fgets(entrada, sizeof entrada, stdin) == NULL) {
            break;
        }
        char extra;
        if (sscanf(entrada, "%d %c", &opcao, &extra) != 1) {
            puts("Digite uma opcao numerica valida.");
            continue;
        }
        if (opcao == 0) {
            break;
        }
        if (opcao >= 1 && opcao <= 4) {
            puts("Funcionalidade ainda em desenvolvimento.");
        } else {
            puts("Opcao invalida.");
        }
    }
    return 0;
}