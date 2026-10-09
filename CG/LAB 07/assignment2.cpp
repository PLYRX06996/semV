#include <GL/freeglut.h>
#include <bits/stdc++.h>
using namespace std;

struct Point3D {
    float x, y, z;
};

vector<Point3D> controlPoints = {
    {3.0f, 1.5f, 0.0f},
    {4.5f, 1.0f, 0.0f},
    {6.5f, 1.2f, 0.0f},
    {8.0f, 2.5f, 0.0f},
    {8.8f, 4.0f, 0.0f},
    {8.2f, 5.0f, 0.0f},
    {7.0f, 4.5f, 0.0f},
    {5.5f, 3.5f, 0.0f},
    {4.0f, 2.5f, 0.0f},
    {3.0f, 2.0f, 0.0f}
};

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.6f, 0.6f, 0.6f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (const auto& p : controlPoints) {
        glVertex3f(p.x, p.y, p.z);
    }
    glEnd();

    glColor3f(1.0f, 0.0f, 0.0f);
    glPointSize(10.0f);
    glBegin(GL_POINTS);
    for (const auto& p : controlPoints) {
        glVertex3f(p.x, p.y, p.z);
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
    glutCreateWindow("Assignment 2 - Localized Control Points");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
