#include <GL/freeglut.h>
#include <bits/stdc++.h>
using namespace std;

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    float x0 = 1.0f, y0 = 2.0f;
    float x1 = 3.0f, y1 = 8.0f;
    float x2 = 6.0f, y2 = 8.0f;
    float x3 = 8.0f, y3 = 2.0f;

    glColor3f(0.6f, 0.6f, 0.6f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(x0, y0);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
        glVertex2f(x3, y3);
    glEnd();

    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 100; i++)
    {
        float t = i / 100.0f;
        float u = 1.0f - t;
        float x = u*u*u*x0
                + 3*u*u*t*x1
                + 3*u*t*t*x2
                + t*t*t*x3;
        float y = u*u*u*y0
                + 3*u*u*t*y1
                + 3*u*t*t*y2
                + t*t*t*y3;
        glVertex2f(x, y);
    }
    glEnd();
    glFlush();
}

void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 10, 0, 10, -1, 1);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Cubic Bezier Spline Demo");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
