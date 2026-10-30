#include <GL/freeglut.h>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.8f, 0.5f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(2.0f, 2.0f);
        glVertex2f(6.0f, 2.0f);
        glVertex2f(6.0f, 5.0f);
        glVertex2f(2.0f, 5.0f);
    glEnd();

    glColor3f(0.8f, 0.1f, 0.1f);
    glBegin(GL_TRIANGLES);
        glVertex2f(1.5f, 5.0f);
        glVertex2f(6.5f, 5.0f);
        glVertex2f(4.0f, 8.0f);
    glEnd();

    glColor3f(0.3f, 0.15f, 0.05f);
    glBegin(GL_QUADS);
        glVertex2f(3.5f, 2.0f);
        glVertex2f(4.5f, 2.0f);
        glVertex2f(4.5f, 4.0f);
        glVertex2f(3.5f, 4.0f);
    glEnd();

    glColor3f(0.4f, 0.2f, 0.05f);
    glBegin(GL_QUADS);
        glVertex2f(7.5f, 2.0f);
        glVertex2f(8.2f, 2.0f);
        glVertex2f(8.2f, 4.5f);
        glVertex2f(7.5f, 4.5f);
    glEnd();

    glColor3f(0.0f, 0.6f, 0.1f);
    glBegin(GL_TRIANGLES);
        glVertex2f(7.0f,  4.0f);
        glVertex2f(8.7f,  4.0f);
        glVertex2f(7.85f, 6.0f);
    glEnd();

    glPushMatrix();
    glTranslatef(-1.0f, 0.0f, 0.0f);
    glScalef(0.5f, 0.5f, 1.0f);
    glRotatef(30.0f, 0.0f, 0.0f, 1.0f);

    glColor3f(0.7f, 0.0f, 0.7f);
    glBegin(GL_QUADS);
        glVertex2f(7.0f, 6.0f);
        glVertex2f(8.0f, 6.0f);
        glVertex2f(8.0f, 7.0f);
        glVertex2f(7.0f, 7.0f);
    glEnd();
    glPopMatrix();

    glFlush();
}

void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, 10.0, 0.0, 10.0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Demo Assignment 01 - 2D Scene");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
