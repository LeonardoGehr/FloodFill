#ifndef IMAGE_H
#define IMAGE_H

typedef struct {
    unsigned char r, g, b;
} Color;

typedef struct {
    int largura, altura;
    Color *pixels;
} Image;

int abrirBMP(const char *caminho, Image *imagem);
int salvarBMP(const char *caminho, const Image *imagem);
void liberarImagem(Image *imagem);

#endif
