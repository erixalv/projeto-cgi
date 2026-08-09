#include <stdlib.h>
#include <math.h>
#include "solarsystem.h"

static float camAngulo = 30.0f;
static float camAltura = 8.0f;
static float camDist   = 18.0f;

#define CAM_DIST_MIN 2.5f
#define CAM_DIST_MAX 40.0f

static void aplicaZoom(float delta)
{
    camDist += delta;
    if (camDist < CAM_DIST_MIN) camDist = CAM_DIST_MIN;
    if (camDist > CAM_DIST_MAX) camDist = CAM_DIST_MAX;
}

void init(void)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    inicializaCorpos();          
    inicializaIluminacao();   
    carregaTexturasPlanetas();   
    configuraVisibilidade();     
}

void display(void)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    float rad = camAngulo * 0.0174533f;
    float olhoX = camDist * sinf(rad);
    float olhoY = camAltura;
    float olhoZ = camDist * cosf(rad);

    gluLookAt(olhoX, olhoY, olhoZ,
              0.0, 0.0, 0.0,
              0.0, 1.0, 0.0);

    desenhaSkybox(texturaFundoID, olhoX, olhoY, olhoZ);     

    atualizaPosicaoLuz();     
    desenhaSistemaSolar();     

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
        case 'v': alternaWireframe();          break;
        case 'a': camAngulo -= 3.0f; break;
        case 'd': camAngulo += 3.0f; break;
        case 'w': camAltura += 0.5f; break;
        case 'x': camAltura -= 0.5f; break;
        case '+': aplicaZoom(-1.0f); break; /* aproxima */
        case '-': aplicaZoom( 1.0f); break; /* afasta   */
        case 27: exit(0);
    }
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (state != GLUT_DOWN) return; /* evento da roda dispara so no "down" */

    if (button == 3)      aplicaZoom(-1.0f); /* roda pra cima -> aproxima */
    else if (button == 4) aplicaZoom( 1.0f); /* roda pra baixo -> afasta  */

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
    glutMouseFunc(mouse);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}