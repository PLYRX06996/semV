#include <GL/glut.h>
#include <cmath>
#include <algorithm>

void drawLineDDA(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;

    float steps = std::max(std::abs(dx), std::abs(dy));

    float xIncr = dx / steps;
    float yIncr = dy / steps;

    float x = x1, y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= (int)steps; i++) {
        glVertex2i((int)round(x), (int)round(y));
        x += xIncr;
        y += yIncr;
    }
    glEnd();
}

void drawSegment(float x1, float y1, float x2, float y2) {
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(5.0f);
    drawLineDDA(x1, y1, x2, y2);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    float p1x = 7.5f,  p1y = 9.2f;
    float p2x = 4.4f,  p2y = 5.4f;
    float p3x = 4.4f,  p3y = 0.4f;
    float p4x = 10.4f, p4y = 0.4f;
    float p5x = 10.4f, p5y = 5.4f;

    drawSegment(p3x, p3y, p4x, p4y);
    drawSegment(p4x, p4y, p5x, p5y);
    drawSegment(p5x, p5y, p1x, p1y);
    drawSegment(p1x, p1y, p2x, p2y);
    drawSegment(p2x, p2y, p3x, p3y);

    drawSegment(p2x, p2y, p4x, p4y);
    drawSegment(p3x, p3y, p5x, p5y);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Assignment 06: House with Diagonals (DDA)");
    gluOrtho2D(0.0, 12.0, 0.0, 12.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
