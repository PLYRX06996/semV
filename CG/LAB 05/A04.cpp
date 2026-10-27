#include <GL/freeglut.h>
#include <cmath>
#include <cstdlib>

void drawLineBH(int x1, int y1, int x2, int y2) {
    int dx = std::abs(x2 - x1);
    int dy = std::abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    glBegin(GL_POINTS);
    while (true) {
        glVertex2i(x1, y1);
        if (x1 == x2 && y1 == y2) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
    glEnd();
}

void plotCirclePoints(int xc, int yc, int x, int y) {
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y);
    glVertex2i(xc - x, yc - y);
    glVertex2i(xc + y, yc + x);
    glVertex2i(xc - y, yc + x);
    glVertex2i(xc + y, yc - x);
    glVertex2i(xc - y, yc - x);
}

void drawCircleMP(int xc, int yc, int r) {
    int x = 0;
    int y = r;
    int p = 1 - r;

    glBegin(GL_POINTS);
    while (x <= y) {
        plotCirclePoints(xc, yc, x, y);
        if (p < 0) {
            p += 2 * x + 3;
        } else {
            p += 2 * (x - y) + 5;
            y--;
        }
        x++;
    }
    glEnd();
}

void drawCircleArcMP(int xc, int yc, int r, int startAngle, int endAngle) {
    glBegin(GL_POINTS);
    for (int deg = startAngle; deg <= endAngle; deg++) {
        float rad = deg * 3.14159f / 180.0f;
        int x = xc + (int)round(r * cos(rad));
        int y = yc + (int)round(r * sin(rad));
        glVertex2i(x, y);
    }
    glEnd();
}

void plotEllipsePoints(int xc, int yc, int x, int y) {
    glVertex2i(xc + x, yc + y);
    glVertex2i(xc - x, yc + y);
    glVertex2i(xc + x, yc - y);
    glVertex2i(xc - x, yc - y);
}

void drawEllipseMP(int xc, int yc, int rx, int ry) {
    float dx, dy, d1, d2;
    float x = 0;
    float y = ry;

    d1 = (ry * ry) - (rx * rx * ry) + (0.25f * rx * rx);
    dx = 2 * ry * ry * x;
    dy = 2 * rx * rx * y;

    glBegin(GL_POINTS);
    while (dx < dy) {
        plotEllipsePoints(xc, yc, (int)round(x), (int)round(y));
        if (d1 < 0) {
            x++;
            dx = dx + (2 * ry * ry);
            d1 = d1 + dx + (ry * ry);
        } else {
            x++;
            y--;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d1 = d1 + dx - dy + (ry * ry);
        }
    }

    d2 = ((ry * ry) * ((x + 0.5f) * (x + 0.5f))) +
         ((rx * rx) * ((y - 1) * (y - 1))) -
         (rx * rx * ry * ry);

    while (y >= 0) {
        plotEllipsePoints(xc, yc, (int)round(x), (int)round(y));
        if (d2 > 0) {
            y--;
            dy = dy - (2 * rx * rx);
            d2 = d2 + (rx * rx) - dy;
        } else {
            y--;
            x++;
            dx = dx + (2 * ry * ry);
            dy = dy - (2 * rx * rx);
            d2 = d2 + dx - dy + (rx * rx);
        }
    }
    glEnd();
}

void drawEllipseArc(int xc, int yc, int rx, int ry, int startAngle, int endAngle) {
    glBegin(GL_POINTS);
    for (int deg = startAngle; deg <= endAngle; deg++) {
        float rad = deg * 3.14159f / 180.0f;
        int x = xc + (int)round(rx * cos(rad));
        int y = yc + (int)round(ry * sin(rad));
        glVertex2i(x, y);
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.1f, 0.1f, 0.15f);
    glPointSize(2.0f);

    drawLineBH(50, 60, 450, 60);

    drawEllipseArc(280, 220, 110, 95, 275, 480);
    drawEllipseArc(280, 220, 110, 95, 115, 185);

    drawCircleArcMP(170, 260, 75, 70, 230);

    drawCircleArcMP(105, 270, 48, 140, 310);
    drawCircleArcMP(80, 320, 28, 90, 270);
    drawCircleMP(78, 348, 6);
    drawCircleArcMP(96, 320, 22, 100, 280);
    drawCircleArcMP(120, 270, 35, 150, 310);

    drawCircleArcMP(150, 215, 18, 200, 340);

    drawCircleArcMP(145, 275, 12, 30, 150);
    drawCircleMP(145, 275, 4);

    drawEllipseArc(235, 270, 42, 65, 45, 340);
    drawEllipseArc(235, 270, 25, 42, 60, 310);

    drawLineBH(135, 175, 135, 60);
    drawLineBH(165, 160, 165, 60);
    drawCircleArcMP(150, 60, 15, 0, 180);
    drawLineBH(185, 150, 185, 60);
    drawLineBH(210, 150, 210, 60);
    drawCircleArcMP(197, 60, 12, 0, 180);

    drawLineBH(310, 140, 310, 60);
    drawLineBH(340, 140, 340, 60);
    drawCircleArcMP(325, 60, 15, 0, 180);
    drawLineBH(360, 140, 360, 60);
    drawLineBH(385, 140, 385, 60);
    drawCircleArcMP(372, 60, 12, 0, 180);

    drawEllipseArc(260, 155, 55, 15, 200, 340);

    drawCircleArcMP(142, 60, 4, 0, 180);
    drawCircleArcMP(150, 60, 4, 0, 180);
    drawCircleArcMP(158, 60, 4, 0, 180);

    drawCircleArcMP(318, 60, 4, 0, 180);
    drawCircleArcMP(326, 60, 4, 0, 180);
    drawCircleArcMP(334, 60, 4, 0, 180);

    drawCircleArcMP(415, 200, 30, 210, 350);
    drawEllipseMP(445, 185, 10, 6);

    glFlush();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 500, 0, 500, -1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Assignment 04 - Elephant Approximation (Mid-Point Circle/Ellipse & Bresenham)");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
