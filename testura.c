/* ============================================================
   PESSOA C - Textura + Algoritmos de Visibilidade
   Responsavel por: gerar/aplicar textura em cada esfera (via
   GLUquadric, que ja calcula coordenadas de textura), pelo
   campo de estrelas de fundo, e pelos algoritmos de remocao de
   superficies ocultas: z-buffer (profundidade) e back-face
   culling.

   OBS: aqui a textura e gerada proceduralmente (nao depende de
   nenhuma lib externa de imagem, so GL puro). Para usar fotos
   reais dos planetas, troque geraTexturaProcedural() por uma
   funcao que carregue um arquivo com stb_image.h e mantenha a
   mesma assinatura de carregaTexturasPlanetas().
   ============================================================ */
#include <stdlib.h>
#include "solarsystem.h"

#define TEX_SIZE 64

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
    for (i = 0; i < numPlanetas; i++) {
        planetas[i].texturaID = geraTexturaProcedural(
            planetas[i].corR, planetas[i].corG, planetas[i].corB);
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

static GLfloat estrelas[500][3];
static int estrelasGeradas = 0;

void desenhaCampoDeEstrelas(int quantidade)
{
    int i;
    if (quantidade > 500) quantidade = 500;

    if (!estrelasGeradas) {
        for (i = 0; i < quantidade; i++) {
            estrelas[i][0] = (rand() % 2000 - 1000) / 20.0f;
            estrelas[i][1] = (rand() % 2000 - 1000) / 20.0f;
            estrelas[i][2] = (rand() % 2000 - 1000) / 20.0f;
        }
        estrelasGeradas = 1;
    }

    glDisable(GL_LIGHTING);
    glPointSize(1.5f);
    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POINTS);
    for (i = 0; i < quantidade; i++) {
        glVertex3fv(estrelas[i]);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

static int cullingAtivo = 1;

void configuraVisibilidade(void)
{
    glEnable(GL_DEPTH_TEST);   /* algoritmo do z-buffer: remove superficies ocultas
                                   comparando a profundidade de cada fragmento     */
    glDepthFunc(GL_LESS);

    glEnable(GL_CULL_FACE);    /* back-face culling: descarta faces cuja normal
                                   aponta para longe da camera, antes mesmo de
                                   rasterizar - reduz trabalho e evita artefatos
                                   em esferas fechadas                             */
    glCullFace(GL_BACK);
}

void alternaBackfaceCulling(void)
{
    cullingAtivo = !cullingAtivo;
    if (cullingAtivo) glEnable(GL_CULL_FACE);
    else               glDisable(GL_CULL_FACE);
}