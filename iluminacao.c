#include "solarsystem.h"

static GLenum modoSombreamento = GL_SMOOTH;

void inicializaIluminacao(void)
{
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glShadeModel(modoSombreamento);

    GLfloat luzAmbiente[]  = {0.1f, 0.1f, 0.1f, 1.0f};
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
    /* emissao branca: a textura real do Sol ja tem a cor/brilho corretos.
       Se usar emissao colorida aqui, o GL_MODULATE da textura multiplica
       e distorce as cores da foto. */
    GLfloat emissiva[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat zero[]      = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_EMISSION, emissiva);
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

void configuraMaterialLua(void)
{
    /* material neutro (branco) para a foto real da Lua aparecer sem
       herdar a cor do planeta que foi desenhado antes dela */
    GLfloat difusaBranca[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat especular[]    = {0.15f, 0.15f, 0.15f, 1.0f};
    GLfloat zero[]         = {0.0f, 0.0f, 0.0f, 1.0f};

    glMaterialfv(GL_FRONT, GL_EMISSION, zero);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, difusaBranca);
    glMaterialfv(GL_FRONT, GL_SPECULAR, especular);
    glMaterialf(GL_FRONT, GL_SHININESS, 5.0f);
}

void alternaModeloSombreamento(void)
{
    modoSombreamento = (modoSombreamento == GL_SMOOTH) ? GL_FLAT : GL_SMOOTH;
    glShadeModel(modoSombreamento);
}