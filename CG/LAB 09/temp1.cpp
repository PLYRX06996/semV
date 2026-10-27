#include <GL/freeglut.h>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, 5,
              0, 0, 0,
              0, 1, 0);

    GLfloat lightPos[] = { -3, 4, 5, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    glColor3f(0.1, 0.3, 1.0);

    glutSolidSphere(1.5, 40, 40);

    glutSwapBuffers();
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat ambient[] = { 0.25, 0.25, 0.25, 1 };
    GLfloat diffuse[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45, 1, 1, 100);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Assignment 01 - 3D Shaded Sphere");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
