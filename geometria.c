/* ============================================================
   PESSOA A - Visualizacao 3D + Curvas Parametricas
   Responsavel por: hierarquia de transformacoes (Sol -> Planeta
   -> Lua com glPushMatrix/glPopMatrix) e pelas orbitas, que sao
   desenhadas com a equacao parametrica da elipse:
        x(t) = a * cos(t)
        z(t) = b * sin(t),   b = a * sqrt(1 - e^2)
   Este arquivo SO usa funcoes declaradas em solarsystem.h.
   Nao conhece a implementacao de iluminacao nem de textura.
   ============================================================ */
#include <math.h>
#include "solarsystem.h"

#define DEG2RAD 0.0174533f
#define PI 3.14159265f

/* Estado global compartilhado (declarado extern no header) */
Planeta planetas[MAX_PLANETAS];
int numPlanetas = 0;

/* dados da unica lua do sistema (orbita a Terra) */
static float anguloOrbitalLua = 0.0f;
static const float velOrbitalLua  = 12.0f;
static const float raioLua        = 0.07f;
static const float distanciaLua   = 0.5f;

static void adicionaPlaneta(const char *nome, float raio, float dist, float ecc,
                             float velOrb, float velRot,
                             float r, float g, float b, int lua)
{
    Planeta p;
    p.nome = nome;
    p.raio = raio;
    p.distanciaSol = dist;
    p.excentricidade = ecc;
    p.velOrbital = velOrb;
    p.velRotacao = velRot;
    p.anguloOrbital = 0.0f;
    p.anguloRotacao = 0.0f;
    p.corR = r; p.corG = g; p.corB = b;
    p.texturaID = 0;
    p.temLua = lua;
    planetas[numPlanetas++] = p;
}

void inicializaCorpos(void)
{
    numPlanetas = 0;
    /*                nome        raio   dist  ecc    vOrb   vRot   r    g    b   lua */
    adicionaPlaneta("Mercurio", 0.15f, 2.0f, 0.20f, 4.10f, 1.0f, 0.60f,0.60f,0.60f, 0);
    adicionaPlaneta("Venus",    0.22f, 2.8f, 0.01f, 1.60f, 0.8f, 0.80f,0.60f,0.30f, 0);
    adicionaPlaneta("Terra",    0.25f, 3.6f, 0.02f, 1.00f, 3.0f, 0.20f,0.40f,0.90f, 1);
    adicionaPlaneta("Marte",    0.18f, 4.4f, 0.09f, 0.53f, 2.8f, 0.80f,0.30f,0.20f, 0);
    adicionaPlaneta("Jupiter",  0.55f, 5.8f, 0.05f, 0.08f, 6.0f, 0.80f,0.60f,0.40f, 0);
    adicionaPlaneta("Saturno",  0.48f, 7.2f, 0.06f, 0.03f, 5.5f, 0.90f,0.80f,0.50f, 0);
    adicionaPlaneta("Urano",    0.35f, 8.4f, 0.05f, 0.01f, 4.0f, 0.50f,0.80f,0.90f, 0);
    adicionaPlaneta("Netuno",   0.34f, 9.6f, 0.01f, 0.006f,4.2f, 0.30f,0.40f,0.90f, 0);
}

void atualizaOrbitas(void)
{
    int i;
    for (i = 0; i < numPlanetas; i++) {
        planetas[i].anguloOrbital += planetas[i].velOrbital;
        planetas[i].anguloRotacao += planetas[i].velRotacao;
        if (planetas[i].anguloOrbital > 360.0f) planetas[i].anguloOrbital -= 360.0f;
        if (planetas[i].anguloRotacao > 360.0f) planetas[i].anguloRotacao -= 360.0f;
    }
    anguloOrbitalLua += velOrbitalLua;
    if (anguloOrbitalLua > 360.0f) anguloOrbitalLua -= 360.0f;
}

/* Curva parametrica da orbita, desenhada como referencia visual */
static void desenhaOrbita(float a, float e)
{
    float b = a * sqrtf(1.0f - e * e);
    int i;
    glDisable(GL_LIGHTING);
    glColor3f(0.25f, 0.25f, 0.25f);
    glBegin(GL_LINE_LOOP);
    for (i = 0; i < 100; i++) {
        float t = (2.0f * PI * i) / 100.0f;
        float x = a * cosf(t);
        float z = b * sinf(t);
        glVertex3f(x, 0.0f, z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void desenhaSistemaSolar(void)
{
    int i;

    /* Sol: raiz da hierarquia, fica parado na origem */
    glPushMatrix();
        configuraMaterialSol();                       /* Pessoa B */
        desenhaEsferaTexturizada(1.0f, 30, 20, 0);     /* Pessoa C (0 = sem textura) */
    glPopMatrix();

    for (i = 0; i < numPlanetas; i++) {
        Planeta *p = &planetas[i];
        float a = p->distanciaSol;
        float b = a * sqrtf(1.0f - p->excentricidade * p->excentricidade);
        float t = p->anguloOrbital * DEG2RAD;
        float x = a * cosf(t);
        float z = b * sinf(t);

        desenhaOrbita(a, p->excentricidade);

        glPushMatrix();
            glTranslatef(x, 0.0f, z);
            glRotatef(p->anguloRotacao, 0.0f, 1.0f, 0.0f); /* rotacao propria */

            configuraMaterialPlaneta(i);                          /* Pessoa B */
            desenhaEsferaTexturizada(p->raio, 20, 16, p->texturaID); /* Pessoa C */

            if (p->temLua) {
                glPushMatrix();
                    glRotatef(anguloOrbitalLua, 0.0f, 1.0f, 0.0f);
                    glTranslatef(distanciaLua, 0.0f, 0.0f);
                    desenhaEsferaTexturizada(raioLua, 12, 10, 0);
                glPopMatrix();
            }
        glPopMatrix();
    }
}