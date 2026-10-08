#include "image.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

static uint32_t ler32(const unsigned char *p) {
    return (uint32_t)p[0] | (uint32_t)p[1] << 8 |
           (uint32_t)p[2] << 16 | (uint32_t)p[3] << 24;
}

static void escrever32(unsigned char *p, uint32_t valor) {
    for (int i = 0; i < 4; i++) {
        p[i] = (unsigned char)(valor >> (i * 8));
    }
}

void liberarImagem(Image *imagem) {
    free(imagem->pixels);
    imagem->pixels = NULL;
    imagem->largura = imagem->altura = 0;
}

int abrirBMP(const char *caminho, Image *imagem) {
    FILE *arquivo = fopen(caminho, "rb");
    if (arquivo == NULL) {
        puts("Nao foi possivel abrir a imagem.");
        return 0;
    }
    unsigned char cabecalho[54];
    Image nova = {0, 0, NULL};
    unsigned char *linha = NULL;
    int sucesso = 0;
    if (fread(cabecalho, 1, 54, arquivo) != 54 ||
        cabecalho[0] != 'B' || cabecalho[1] != 'M') {
        goto fim;
    }
    uint32_t dib = ler32(cabecalho + 14);
    uint32_t offset = ler32(cabecalho + 10);
    uint32_t largura = ler32(cabecalho + 18);
    uint32_t alturaBruta = ler32(cabecalho + 22);
    int invertida = (alturaBruta & UINT32_C(0x80000000)) != 0;
    uint32_t altura = invertida ? (~alturaBruta + 1u) : alturaBruta;
    if (dib < 40 || (uint64_t)offset < (uint64_t)14 + dib ||
        largura == 0 || largura > INT_MAX || altura == 0 || altura > INT_MAX ||
        cabecalho[26] != 1 || cabecalho[27] != 0 ||
        cabecalho[28] != 24 || cabecalho[29] != 0 ||
        ler32(cabecalho + 30) != 0) {
        goto fim;
    }
    uint64_t stride64 = ((uint64_t)largura * 3 + 3) & ~UINT64_C(3);
    uint64_t quantidade = (uint64_t)largura * altura;
    uint64_t fimPixels = offset + stride64 * altura;
    if (stride64 > SIZE_MAX || quantidade > SIZE_MAX / sizeof(Color) ||
        fimPixels > LONG_MAX || fseek(arquivo, 0, SEEK_END) != 0) {
        goto fim;
    }
    long tamanho = ftell(arquivo);
    if (tamanho < 0 || (uint64_t)tamanho < fimPixels ||
        fseek(arquivo, (long)offset, SEEK_SET) != 0) {
        goto fim;
    }
    nova.largura = (int)largura;
    nova.altura = (int)altura;
    nova.pixels = malloc((size_t)quantidade * sizeof(Color));
    linha = malloc((size_t)stride64);
    if (nova.pixels == NULL || linha == NULL) {
        goto fim;
    }
    for (int y = 0; y < nova.altura; y++) {
        if (fread(linha, 1, (size_t)stride64, arquivo) != (size_t)stride64) {
            goto fim;
        }
        int destino = invertida ? y : nova.altura - 1 - y;
        for (int x = 0; x < nova.largura; x++) {
            size_t i = (size_t)x * 3;
            Color cor = {linha[i + 2], linha[i + 1], linha[i]};
            nova.pixels[(size_t)destino * largura + (size_t)x] = cor;
        }
    }
    liberarImagem(imagem);
    *imagem = nova;
    nova.pixels = NULL;
    sucesso = 1;
fim:
    free(linha);
    liberarImagem(&nova);
    fclose(arquivo);
    if (!sucesso) {
        puts("Imagem invalida, incompleta ou sem memoria. Use BMP de 24 bits sem compressao.");
    }
    return sucesso;
}

int salvarBMP(const char *caminho, const Image *imagem) {
    if (imagem->pixels == NULL || imagem->largura <= 0 || imagem->altura <= 0) {
        return 0;
    }
    uint64_t stride64 = ((uint64_t)imagem->largura * 3 + 3) & ~UINT64_C(3);
    uint64_t tamanho = stride64 * (uint64_t)imagem->altura;
    if (stride64 > SIZE_MAX || tamanho > UINT32_MAX - 54u) {
        return 0;
    }
    unsigned char *linha = calloc((size_t)stride64, 1);
    if (linha == NULL) {
        return 0;
    }
    FILE *arquivo = fopen(caminho, "wb");
    if (arquivo == NULL) {
        free(linha);
        return 0;
    }
    unsigned char cabecalho[54] = {0};
    cabecalho[0] = 'B'; cabecalho[1] = 'M';
    escrever32(cabecalho + 2, (uint32_t)tamanho + 54);
    escrever32(cabecalho + 10, 54);
    escrever32(cabecalho + 14, 40);
    escrever32(cabecalho + 18, (uint32_t)imagem->largura);
    escrever32(cabecalho + 22, (uint32_t)imagem->altura);
    cabecalho[26] = 1; cabecalho[28] = 24;
    escrever32(cabecalho + 34, (uint32_t)tamanho);
    int sucesso = fwrite(cabecalho, 1, 54, arquivo) == 54;
    for (int y = imagem->altura - 1; sucesso && y >= 0; y--) {
        for (int x = 0; x < imagem->largura; x++) {
            Color cor = imagem->pixels[(size_t)y * (size_t)imagem->largura + (size_t)x];
            size_t i = (size_t)x * 3;
            linha[i] = cor.b; linha[i + 1] = cor.g; linha[i + 2] = cor.r;
        }
        sucesso = fwrite(linha, 1, (size_t)stride64, arquivo) == (size_t)stride64;
    }
    if (fclose(arquivo) != 0) {
        sucesso = 0;
    }
    free(linha);
    return sucesso;
}
