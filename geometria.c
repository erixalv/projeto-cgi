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

/* rotacao propria do Sol em torno do proprio eixo */
static float anguloRotacaoSol = 0.0f;
static const float velRotacaoSol = 0.4f; /* mais lento que os planetas, sol e gigante */

/* Dados do cometa (curva de bézier) */
typedef struct { float x, y, z; } Ponto3D;
static Ponto3D p0 = { 12.0f, 0.0f,  12.0f };
static Ponto3D p1 = {-15.0f, 0.0f,   5.0f };
static Ponto3D p2 = { -5.0f, 0.0f, -15.0f };
static Ponto3D p3 = { 12.0f, 0.0f, -12.0f };
static float cometaT = 0.0f;
static float cometaVel = 0.002f;

void inicializaCometa(void) {
    cometaT = 0.0f;
}

void atualizaCometa(void) {
    cometaT += cometaVel;
    if (cometaT >= 1.0f) {
        cometaT = 0.0f; /* volta para o comeco */
    }
}

static Ponto3D calculaBezier(float t) {
    float u = 1.0f - t;
    float tt = t * t;
    float uu = u * u;
    float uuu = uu * u;
    float ttt = tt * t;
    Ponto3D p;
    p.x = uuu * p0.x + 3 * uu * t * p1.x + 3 * u * tt * p2.x + ttt * p3.x;
    p.y = uuu * p0.y + 3 * uu * t * p1.y + 3 * u * tt * p2.y + ttt * p3.y;
    p.z = uuu * p0.z + 3 * uu * t * p1.z + 3 * u * tt * p2.z + ttt * p3.z;
    return p;
}

static void desenhaOrbitaCometa(void) {
    int i;
    glDisable(GL_LIGHTING);
    glColor3f(0.4f, 0.8f, 1.0f);
    glBegin(GL_LINE_STRIP);
    for (i = 0; i <= 100; i++) {
        float t = i / 100.0f;
        Ponto3D p = calculaBezier(t);
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
}

void desenhaCometa(void) {
    Ponto3D pos;
    desenhaOrbitaCometa();
    pos = calculaBezier(cometaT);
    glPushMatrix();
        glTranslatef(pos.x, pos.y, pos.z);
        GLfloat difusa[] = {0.6f, 0.9f, 1.0f, 1.0f};
        GLfloat zero[] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, zero);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, difusa);
        glMaterialf(GL_FRONT, GL_SHININESS, 50.0f);
        desenhaEsferaTexturizada(0.12f, 12, 12, 0);
    glPopMatrix();
}

static void adicionaPlaneta(const char *nome, float raio, float dist, float ecc,
                             float velOrb, float velRot,
                             float r, float g, float b, int lua,
                             const char *arquivoTextura)
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
    p.arquivoTextura = arquivoTextura;
    p.texturaID = 0;
    p.temLua = lua;
    p.temAnel = 0;
    p.anelRaioInterno = 0.0f;
    p.anelRaioExterno = 0.0f;
    p.arquivoTexturaAnel = NULL;
    p.anelTexturaID = 0;
    planetas[numPlanetas++] = p;
}

void inicializaCorpos(void)
{
    numPlanetas = 0;
    /*                nome        raio   dist  ecc    vOrb   vRot   r    g    b    lua  arquivo de textura           */
    adicionaPlaneta("Mercurio", 0.15f, 2.0f, 0.20f, 4.10f, 1.0f, 0.60f,0.60f,0.60f, 0, "texturas/mercury.jpg");
    adicionaPlaneta("Venus",    0.22f, 2.8f, 0.01f, 1.60f, 0.8f, 0.80f,0.60f,0.30f, 0, "texturas/venus.jpg");
    adicionaPlaneta("Terra",    0.25f, 3.6f, 0.02f, 1.00f, 3.0f, 0.20f,0.40f,0.90f, 1, "texturas/earth.jpg");
    adicionaPlaneta("Marte",    0.18f, 4.4f, 0.09f, 0.53f, 2.8f, 0.80f,0.30f,0.20f, 0, "texturas/mars.jpg");
    adicionaPlaneta("Jupiter",  0.55f, 5.8f, 0.05f, 0.08f, 6.0f, 0.80f,0.60f,0.40f, 0, "texturas/jupiter.jpg");
    adicionaPlaneta("Saturno",  0.48f, 7.2f, 0.06f, 0.03f, 5.5f, 0.90f,0.80f,0.50f, 0, "texturas/saturn.jpg");
    adicionaPlaneta("Urano",    0.35f, 8.4f, 0.05f, 0.01f, 4.0f, 0.50f,0.80f,0.90f, 0, "texturas/uranus.jpg");
    adicionaPlaneta("Netuno",   0.34f, 9.6f, 0.01f, 0.006f,4.2f, 0.30f,0.40f,0.90f, 0, "texturas/neptune.jpg");

    /* Saturno (indice 5) e o unico com anel */
    planetas[5].temAnel = 1;
    planetas[5].anelRaioInterno = planetas[5].raio * 1.4f;
    planetas[5].anelRaioExterno = planetas[5].raio * 2.3f;
    planetas[5].arquivoTexturaAnel = "texturas/saturn_ring.png";

    inicializaCometa();
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

    anguloRotacaoSol += velRotacaoSol;
    if (anguloRotacaoSol > 360.0f) anguloRotacaoSol -= 360.0f;

    atualizaCometa();
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

    /* Sol: raiz da hierarquia, fica parado na origem (so gira em torno do eixo) */
    glPushMatrix();
        glRotatef(anguloRotacaoSol, 0.0f, 1.0f, 0.0f);
        configuraMaterialSol();                         
        desenhaEsferaTexturizada(1.0f, 30, 20, texturaSolID); 
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

            if (p->temAnel) {
                glPushMatrix();
                    glRotatef(20.0f, 1.0f, 0.0f, 0.0f); /* inclinacao do anel, tipo Saturno de verdade */
                    desenhaAnelTexturizado(p->anelRaioInterno, p->anelRaioExterno, 80, p->anelTexturaID); 
                glPopMatrix();
            }

            glRotatef(p->anguloRotacao, 0.0f, 1.0f, 0.0f); /* rotacao propria */

            configuraMaterialPlaneta(i);                          
            desenhaEsferaTexturizada(p->raio, 20, 16, p->texturaID); 

            if (p->temLua) {
                glPushMatrix();
                    glRotatef(anguloOrbitalLua, 0.0f, 1.0f, 0.0f);
                    glTranslatef(distanciaLua, 0.0f, 0.0f);
                    configuraMaterialLua(); 
                    desenhaEsferaTexturizada(raioLua, 12, 10, texturaLuaID); 
                glPopMatrix();
            }
        glPopMatrix();
    }

    desenhaCometa();
}