#ifndef SOLARSYSTEM_H
#define SOLARSYSTEM_H

#include <GL/glut.h>

#define MAX_PLANETAS 9

typedef struct {
    const char *nome;
    float raio;             /* raio do planeta (escala visual)               */
    float distanciaSol;     /* semi-eixo maior "a" da orbita eliptica        */
    float excentricidade;   /* "e" da elipse (0 = circulo perfeito)          */
    float velOrbital;       /* graus/frame ao redor do sol                   */
    float velRotacao;       /* graus/frame em torno do proprio eixo          */
    float anguloOrbital;    /* estado atual, atualizado a cada frame         */
    float anguloRotacao;    /* estado atual, atualizado a cada frame         */
    float corR, corG, corB; /* cor base (fallback se a textura nao carregar) */
    const char *arquivoTextura; /* ex: "texturas/earth.jpg", ou NULL         */
    GLuint texturaID;       
    int temLua;             /* 1 = tem lua (por padrao, so a Terra)          */

    /* Anel (so Saturno usa) */
    int temAnel;
    float anelRaioInterno;
    float anelRaioExterno;
    const char *arquivoTexturaAnel;
    GLuint anelTexturaID;  
} Planeta;


extern Planeta planetas[MAX_PLANETAS];
extern int numPlanetas;
extern GLuint texturaSolID;   
extern GLuint texturaFundoID;
extern GLuint texturaLuaID;  

/* ---- Visualizacao 3D + Curvas Parametricas ---- */
void inicializaCorpos(void);      /* preenche planetas[] e numPlanetas       */
void atualizaOrbitas(void);       /* avanca angulos de orbita e rotacao      */
void desenhaSistemaSolar(void);   /* desenha hierarquia Sol -> Planeta -> Lua */

void inicializaCometa(void);      /* configura estado inicial do cometa      */
void atualizaCometa(void);        /* atualiza o parametro 't' na curva de Bezier */
void desenhaCometa(void);         /* desenha a curva e o cometa em 't'       */

/* ---- Iluminacao e Sombreamento ---- */
void inicializaIluminacao(void);        /* configura GL_LIGHT0 (o Sol) uma vez */
void atualizaPosicaoLuz(void);          /* re-fixa a luz na origem a cada frame */
void configuraMaterialSol(void);        /* Sol e emissivo, nao recebe luz       */
void configuraMaterialPlaneta(int indice); /* material difuso/especular do planeta[i] */
void configuraMaterialLua(void);        /* material neutro (branco) para a Lua */
void alternaModeloSombreamento(void);   /* alterna GL_FLAT <-> GL_SMOOTH */

/* ---- Textura + Algoritmos de Visibilidade ---- */
void carregaTexturasPlanetas(void);     /* gera/carrega e preenche texturaID de cada planeta */
void desenhaEsferaTexturizada(float raio, int fatias, int camadas, GLuint texID);
void desenhaAnelTexturizado(float raioInterno, float raioExterno, int segmentos, GLuint texID);
void desenhaCampoDeEstrelas(int quantidade);
void desenhaFundoEspacial(GLuint texID); /* fundo com imagem, cobrindo a tela inteira (2D) */
void desenhaSkybox(GLuint texID, float camX, float camY, float camZ); /* fundo esferico 3D, centrado na camera */
void configuraVisibilidade(void);       /* liga o z-buffer (algoritmo de visibilidade) */
void alternaWireframe(void); /* liga/desliga modo aramado, pra visualizar a malha */

#endif