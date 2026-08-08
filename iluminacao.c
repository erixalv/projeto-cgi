/* ============================================================
   PESSOA B - Iluminacao e Sombreamento
   Responsavel por: GL_LIGHT0 posicionada no Sol (luz posicional
   com atenuacao, para os planetas mais distantes ficarem mais
   escuros), materiais de cada planeta e alternancia entre
   GL_FLAT e GL_SMOOTH.
   Este arquivo pode ser testado sozinho: basta desenhar 2-3
   esferas soltas com glutSolidSphere e chamar estas funcoes -
   nao precisa da orbita nem da textura da Pessoa A/C prontas.
   ============================================================ */
#include "solarsystem.h"

static GLenum modoSombreamento = GL_SMOOTH;

void inicializaIluminacao(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(modoSombreamento);

    GLfloat luzAmbiente[]  = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat luzDifusa[]    = {1.0f, 1.0f, 0.95f, 1.0f};
    GLfloat luzEspecular[] = {1.0f, 1.0f, 1.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, luzAmbiente);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, luzDifusa);
    glLightfv(GL_LIGHT0, GL_SPECULAR, luzEspecular);

    /* atenuacao: planetas mais distantes do Sol recebem menos luz */
    glLightf(GL_LIGHT0, GL_CONSTANT_ATTENUATION, 1.0f);
    glLightf(GL_LIGHT0, GL_LINEAR_ATTENUATION, 0.02f);
    glLightf(GL_LIGHT0, GL_QUADRATIC_ATTENUATION, 0.0f);

    glEnable(GL_NORMALIZE); /* corrige normais deformadas por glScalef */
}

/* Precisa ser chamada a cada display(), DEPOIS do gluLookAt, para a
   posicao da luz ficar fixa no mundo (na origem, onde esta o Sol)
   independente da camera. */
void atualizaPosicaoLuz(void)
{
    GLfloat posicaoLuz[] = {0.0f, 0.0f, 0.0f, 1.0f}; /* w=1 -> luz posicional */
    glLightfv(GL_LIGHT0, GL_POSITION, posicaoLuz);
}

void configuraMaterialSol(void)
{
    GLfloat emissiva[] = {1.0f, 0.9f, 0.4f, 1.0f};
    GLfloat zero[]      = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, emissiva); /* o Sol "brilha", nao reflete luz */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, zero);
    glMaterialfv(GL_FRONT, GL_SPECULAR, zero);
}

void configuraMaterialPlaneta(int indice)
{
    if (indice < 0 || indice >= numPlanetas) return;
    Planeta *p = &planetas[indice];

    GLfloat difusa[]    = {p->corR, p->corG, p->corB, 1.0f};
    GLfloat especular[] = {0.3f, 0.3f, 0.3f, 1.0f};
    GLfloat zero[]      = {0.0f, 0.0f, 0.0f, 1.0f};

    glMaterialfv(GL_FRONT, GL_EMISSION, zero); /* reseta emissao herdada do Sol */
    glMaterialfv(GL_FRONT, GL_DIFFUSE, difusa);
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT, GL_SHININESS, 12.0f);
}

void alternaModeloSombreamento(void)
{
    modoSombreamento = (modoSombreamento == GL_SMOOTH) ? GL_FLAT : GL_SMOOTH;
    glShadeModel(modoSombreamento);
}