#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "solarsystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TEX_SIZE 64

GLuint texturaSolID = 0;
GLuint texturaFundoID = 0;
GLuint texturaLuaID = 0;

/* Carrega uma imagem de arquivo (jpg/png/bmp/tga...) e devolve o ID
   da textura OpenGL ja pronta, com mipmaps. Retorna 0 em caso de
   falha (arquivo nao encontrado, formato invalido etc). */
static GLuint carregaTexturaDeArquivo(const char *caminho)
{
    int largura, altura, nrCanais;
    unsigned char *dados = stbi_load(caminho, &largura, &altura, &nrCanais, 0);
    if (!dados) {
        fprintf(stderr, "Aviso: nao foi possivel carregar textura '%s' (%s). Usando fallback procedural.\n",
                caminho, stbi_failure_reason());
        return 0;
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum formato = (nrCanais == 4) ? GL_RGBA : GL_RGB;
    gluBuild2DMipmaps(GL_TEXTURE_2D, formato, largura, altura, formato, GL_UNSIGNED_BYTE, dados);

    stbi_image_free(dados);
    return id;
}

static GLuint geraTexturaProcedural(float r, float g, float b)
{
    static GLubyte dados[TEX_SIZE][TEX_SIZE][3];
    int i, j;
    for (i = 0; i < TEX_SIZE; i++) {
        for (j = 0; j < TEX_SIZE; j++) {
            int faixa = (i / 8) % 2;              /* listras horizontais */
            float ruido = ((rand() % 20) - 10) / 100.0f; /* variacao sutil */
            float fator = (faixa ? 1.0f : 0.75f) + ruido;
            if (fator < 0.0f) fator = 0.0f;
            if (fator > 1.0f) fator = 1.0f;
            dados[i][j][0] = (GLubyte)(r * 255 * fator);
            dados[i][j][1] = (GLubyte)(g * 255 * fator);
            dados[i][j][2] = (GLubyte)(b * 255 * fator);
        }
    }

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TEX_SIZE, TEX_SIZE, 0,
                 GL_RGB, GL_UNSIGNED_BYTE, dados);
    return id;
}

void carregaTexturasPlanetas(void)
{
    int i;

    texturaSolID = carregaTexturaDeArquivo("texturas/sun.jpg");
    if (texturaSolID == 0) {
        texturaSolID = geraTexturaProcedural(1.0f, 0.9f, 0.4f);
    }

    texturaLuaID = carregaTexturaDeArquivo("texturas/moon.jpg");
    if (texturaLuaID == 0) {
        texturaLuaID = geraTexturaProcedural(0.75f, 0.75f, 0.75f); /* cinza, cor da lua */
    }


    texturaFundoID = carregaTexturaDeArquivo("texturas/fundo_espaco.jpg");

    for (i = 0; i < numPlanetas; i++) {
        GLuint id = 0;

        if (planetas[i].arquivoTextura != NULL) {
            id = carregaTexturaDeArquivo(planetas[i].arquivoTextura);
        }

        if (id == 0) { /* sem arquivo definido, ou falhou ao carregar */
            id = geraTexturaProcedural(planetas[i].corR, planetas[i].corG, planetas[i].corB);
        }

        planetas[i].texturaID = id;

        if (planetas[i].temAnel && planetas[i].arquivoTexturaAnel != NULL) {
            planetas[i].anelTexturaID = carregaTexturaDeArquivo(planetas[i].arquivoTexturaAnel);
            /* sem fallback procedural para o anel: se faltar o arquivo, o
               anel simplesmente nao desenha (ver desenhaAnelTexturizado) */
        }
    }
}

/* Desenha uma esfera com coordenadas de textura (gluSphere calcula
   isso automaticamente, diferente de glutSolidSphere). texID == 0
   desenha sem textura (usado para o Sol e para a Lua neste esqueleto). */
void desenhaEsferaTexturizada(float raio, int fatias, int camadas, GLuint texID)
{
    GLUquadric *quad = gluNewQuadric();
    gluQuadricNormals(quad, GLU_SMOOTH);

    if (texID != 0) {
        gluQuadricTexture(quad, GL_TRUE);
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, texID);
    }

    gluSphere(quad, raio, fatias, camadas);

    if (texID != 0) {
        glDisable(GL_TEXTURE_2D);
    }
    gluDeleteQuadric(quad);
}

/* Desenha um anel plano (annulus) entre raioInterno e raioExterno, no
   plano XZ. Texturas de anel costumam ser uma faixa 1D (as vezes com
   canal alfa para as falhas/gaps), entao o mapeamento de textura aqui
   e radial: s=0 na borda interna, s=1 na borda externa. */
void desenhaAnelTexturizado(float raioInterno, float raioExterno, int segmentos, GLuint texID)
{
    int i;
    GLfloat difusaBranca[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat zero[] = {0.0f, 0.0f, 0.0f, 1.0f};

    if (texID == 0) return; /* sem textura carregada, nao desenha nada */

    /* material neutro para a foto do anel aparecer com as cores reais,
       sem herdar a cor do ultimo planeta desenhado */
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, difusaBranca);
    glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, zero);

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);

    /* aneis costumam ter transparencia (png com alpha) nas falhas */
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glBegin(GL_TRIANGLE_STRIP);
    for (i = 0; i <= segmentos; i++) {
        float t = (2.0f * 3.14159265f * i) / segmentos;
        float cosT = cosf(t), sinT = sinf(t);
        float xi = raioInterno * cosT, zi = raioInterno * sinT;
        float xo = raioExterno * cosT, zo = raioExterno * sinT;

        glNormal3f(0.0f, 1.0f, 0.0f);
        glTexCoord2f(0.0f, (float)i / segmentos);
        glVertex3f(xi, 0.0f, zi);
        glTexCoord2f(1.0f, (float)i / segmentos);
        glVertex3f(xo, 0.0f, zo);
    }
    glEnd();

    glDisable(GL_BLEND);
    glDisable(GL_TEXTURE_2D);
}




/* Fundo esferico 3D: uma esfera enorme texturizada, vista de dentro,
   sempre recentralizada no olho da camera (camX,camY,camZ = mesma
   posicao passada pro gluLookAt naquele frame). Como e desenhada logo
   apos o gluLookAt e transladada de volta pra origem da camera, ela
   fica "grudada" no olho: acompanha a rotacao/orbita da camera mas
   nao a translacao - efeito de ceu no infinito. */
void desenhaSkybox(GLuint texID, float camX, float camY, float camZ)
{
    if (texID == 0) return;

    GLUquadric *quad = gluNewQuadric();
    gluQuadricTexture(quad, GL_TRUE);
    gluQuadricNormals(quad, GLU_SMOOTH);

    glPushMatrix();
    glTranslatef(camX, camY, camZ); /* recentraliza a esfera no olho da camera */

    glDisable(GL_LIGHTING);   /* fundo nao deve ser afetado pela luz do Sol */
    glDisable(GL_DEPTH_TEST); /* sempre desenhado "atras" de tudo */
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texID);
    glColor3f(1.0f, 1.0f, 1.0f);

    gluSphere(quad, 60.0, 40, 40); /* raio bem maior que a orbita mais externa */

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glPopMatrix();
    gluDeleteQuadric(quad);
}

void configuraVisibilidade(void)
{
    glEnable(GL_DEPTH_TEST);   /* algoritmo do z-buffer: remove superficies ocultas
                                   comparando a profundidade de cada fragmento     */
    glDepthFunc(GL_LESS);
}
