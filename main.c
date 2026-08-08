/* ============================================================
   INTEGRACAO - escrito por qualquer um dos tres, no final.
   So chama as funcoes que ja estao prontas nos outros arquivos,
   na ordem certa. Nao contem nenhuma logica de negocio.
   Compilar: gcc main.c pessoa_a_geometria.c pessoa_b_iluminacao.c
             pessoa_c_textura_visibilidade.c -o solar -lglut -lGLU -lGL -lm
   ============================================================ */
#include <stdlib.h>
#include <math.h>
#include "solarsystem.h"

static float camAngulo = 30.0f;
static float camAltura = 8.0f;
static float camDist   = 18.0f;

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    inicializaCorpos();          /* Pessoa A */
    inicializaIluminacao();      /* Pessoa B */
    carregaTexturasPlanetas();   /* Pessoa C */
    configuraVisibilidade();     /* Pessoa C */
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float rad = camAngulo * 0.0174533f;
    gluLookAt(camDist * sinf(rad), camAltura, camDist * cosf(rad),
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    atualizaPosicaoLuz();          /* Pessoa B - luz fixa no Sol, na origem */
    desenhaCampoDeEstrelas(400);   /* Pessoa C */
    desenhaSistemaSolar();         /* Pessoa A (usa B e C internamente) */

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    if (h == 0) h = 1;
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60.0, (GLfloat)w / (GLfloat)h, 0.5, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int v)
{
    atualizaOrbitas();
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    switch (key) {
        case 's': alternaModeloSombreamento(); break;
        case 'c': alternaBackfaceCulling();    break;
        case 'a': camAngulo -= 3.0f; break;
        case 'd': camAngulo += 3.0f; break;
        case 'w': camAltura += 0.5f; break;
        case 'x': camAltura -= 0.5f; break;
        case 27: exit(0);
    }
    glutPostRedisplay();
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(900, 900);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Sistema Solar - Computacao Grafica UFPB");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}