#include <GL/freeglut.h>
#include <cmath>

void plotCirclePoints(int xc, int yc, int x, int y) {
    glVertex2i(xc + x, yc + y); glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y); glVertex2i(xc - x, yc - y);
    glVertex2i(xc + y, yc + x); glVertex2i(xc - y, yc + x);
    glVertex2i(xc + y, yc - x); glVertex2i(xc - y, yc - x);
}

void midpointCircle(int xc, int yc, int r) {
    int x = 0, y = r, p = 1 - r;
    glBegin(GL_POINTS);
    while (x <= y) {
        plotCirclePoints(xc, yc, x, y);
        if (p < 0) p += 2 * x + 3; else { p += 2 * (x - y) + 5; y--; }
        x++;
    }
    glEnd();
}

void drawLineBH(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1), dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1, sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;
    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x1 += sx; }
        if (e2 < dx) { err += dx; y1 += sy; }
    }
    glEnd();
}
void drawGrid(int maxX, int maxY) {
    glColor3f(0.2f, 0.2f, 0.2f); // Dark gray color for subtle grid lines
    glLineWidth(1.0f);
    glBegin(GL_LINES);
    
    // Draw vertical grid lines
    for (int x = 0; x <= maxX; x++) {
        glVertex2i(x, 0);
        glVertex2i(x, maxY);
    }
    
    // Draw horizontal grid lines
    for (int y = 0; y <= maxY; y++) {
        glVertex2i(0, y);
        glVertex2i(maxX, y);
    }
    
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(2.0f);

    int r = 16;
    midpointCircle(300, 368, r);
    midpointCircle(176, 216, r);
    midpointCircle(176, 16, r);
    midpointCircle(416, 16, r);
    midpointCircle(416, 216, r);

    drawLineBH(192, 16, 400, 16);
    drawLineBH(416, 32, 416, 200);
    drawLineBH(192, 216, 400, 216);
    drawLineBH(176, 32, 176, 200);
    drawLineBH(186, 228, 290, 356);
    drawLineBH(407, 228, 309, 356);

    glFlush();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION); glLoadIdentity();
    glOrtho(0, 500, 0, 500, -1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 500);
    glutCreateWindow("Assignment 2 - House with Nodes");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}