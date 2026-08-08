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
    float corR, corG, corB; /* cor base (usada na textura procedural)        */
    GLuint texturaID;       /* preenchido pelo modulo de Textura (Pessoa C)  */
    int temLua;             /* 1 = tem lua (por padrao, so a Terra)          */
} Planeta;

/* ---------------------------------------------------------------
   Estado global compartilhado. Este array + as assinaturas abaixo
   sao o UNICO contrato entre os tres modulos. Ninguem chama funcao
   "interna" de ninguem, so essas aqui.
   --------------------------------------------------------------- */
extern Planeta planetas[MAX_PLANETAS];
extern int numPlanetas;

/* ---- Pessoa A: Visualizacao 3D + Curvas Parametricas ---- */
void inicializaCorpos(void);      /* preenche planetas[] e numPlanetas       */
void atualizaOrbitas(void);       /* avanca angulos de orbita e rotacao      */
void desenhaSistemaSolar(void);   /* desenha hierarquia Sol -> Planeta -> Lua */

/* ---- Pessoa B: Iluminacao e Sombreamento ---- */
void inicializaIluminacao(void);        /* configura GL_LIGHT0 (o Sol) uma vez */
void atualizaPosicaoLuz(void);          /* re-fixa a luz na origem a cada frame */
void configuraMaterialSol(void);        /* Sol e emissivo, nao recebe luz       */
void configuraMaterialPlaneta(int indice); /* material difuso/especular do planeta[i] */
void alternaModeloSombreamento(void);   /* alterna GL_FLAT <-> GL_SMOOTH */

/* ---- Pessoa C: Textura + Algoritmos de Visibilidade ---- */
void carregaTexturasPlanetas(void);     /* gera/carrega e preenche texturaID de cada planeta */
void desenhaEsferaTexturizada(float raio, int fatias, int camadas, GLuint texID);
void desenhaCampoDeEstrelas(int quantidade);
void configuraVisibilidade(void);       /* liga z-buffer + back-face culling */
void alternaBackfaceCulling(void);

#endif