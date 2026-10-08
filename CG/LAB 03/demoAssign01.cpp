#include <GL/glut.h>
#include <cmath>
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float x1 = 2.8f, y1 = 1.1f;
    float x2 = 9.2f, y2 = 4.9f;
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();
    float m = (y2 - y1) / (x2 - x1);
    float c = y1 - m * x1;
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(8.0f);
    glBegin(GL_POINTS);
    for (int x = (int)round(x1); x <= (int)round(x2); x++) {
        float y = m * x + c;
        glVertex2i(x, (int)round(y));
    }

    glEnd();
    glFlush();
}
int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Line Drawing with Pixels and Line");
    gluOrtho2D(0.0, 12.0, 0.0, 12.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}