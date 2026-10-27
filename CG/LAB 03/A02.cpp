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

void drawLineBH(float x1, float y1, float x2, float y2) {
    int x = (int)round(x1), y = (int)round(y1);
    int xe = (int)round(x2), ye = (int)round(y2);

    int dx = std::abs(xe - x);
    int dy = std::abs(ye - y);
    int sx = (x < xe) ? 1 : -1;
    int sy = (y < ye) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x, y);
        if (x == xe && y == ye) break;

        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 <  dx) { err += dx; y += sy; }
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    float x1 = 8.8f, y1 = 0.8f;
    float x2 = 1.0f, y2 = 3.4f;

    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glVertex2f(x1, y1);
        glVertex2f(x2, y2);
    glEnd();

    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(8.0f);
    drawLineDDA(x1, y1, x2, y2);

    glFlush();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Assignment 02: DDA & Bresenham (8.8,0.8) to (1.0,3.4)");
    gluOrtho2D(0.0, 12.0, 0.0, 12.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
