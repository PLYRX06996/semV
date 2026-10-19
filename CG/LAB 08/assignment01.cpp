#include <GL/freeglut.h>

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    // House
    glColor3f(0.8, 0.5, 0.2);
    glBegin(GL_QUADS);
        glVertex2f(2, 2);
        glVertex2f(6, 2);
        glVertex2f(6, 5);
        glVertex2f(2, 5);
    glEnd();

    // Roof
    glColor3f(0.8, 0.1, 0.1);
    glBegin(GL_TRIANGLES);
        glVertex2f(1.5, 5);
        glVertex2f(6.5, 5);
        glVertex2f(4, 8);
    glEnd();

    // Door
    glColor3f(0.3, 0.15, 0.05);
    glBegin(GL_QUADS);
        // Remaining vertices are cut off in the image.
    // ...

    // Tree
    glColor3f(0.4, 0.2, 0.05);
    glBegin(GL_QUADS);
        glVertex2f(7.5, 2);
        glVertex2f(8, 2.2);
        glVertex2f(8, 2.4);
        glVertex2f(7.5, 4.5);
    glEnd();

    glColor3f(0, 0.6, 0.1);
    glBegin(GL_TRIANGLES);
        // First vertex is partially cut off.
        glVertex2f(8.7, 4);
        glVertex2f(7.85, 6);
    glEnd();

    // Transformations
    glPushMatrix();
    glTranslatef(-1, 0, 0);
    glScalef(0.5, 0.5, 1);
    glRotatef(30, 0, 0, 1);

    glColor3f(0.7, 0, 0.7);
    glBegin(GL_QUADS);
        glVertex2f(7, 6);
        glVertex2f(8, 6);
        // Remaining vertices are cut off in the image.
    // ...
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, 10, 0, 10);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 600);
    glutCreateWindow("2D Scene");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}