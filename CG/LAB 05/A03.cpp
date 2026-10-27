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

void drawRect(int x1, int y1, int x2, int y2) {
    drawLineBH(x1, y1, x2, y1);
    drawLineBH(x2, y1, x2, y2);
    drawLineBH(x2, y2, x1, y2);
    drawLineBH(x1, y2, x1, y1);
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

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.0f, 0.0f, 0.0f);
    glPointSize(2.0f);

    drawRect(30, 40, 470, 55);

    drawLineBH(50, 55, 50, 250);
    drawLineBH(230, 55, 230, 250);

    drawLineBH(230, 55, 450, 55);
    drawLineBH(450, 55, 450, 230);
    drawLineBH(230, 230, 450, 230);

    drawLineBH(35, 240, 140, 360);
    drawLineBH(140, 360, 245, 240);
    drawLineBH(45, 230, 140, 345);
    drawLineBH(140, 345, 235, 230);
    drawLineBH(35, 240, 45, 230);
    drawLineBH(245, 240, 235, 230);

    drawLineBH(140, 360, 470, 240);
    drawLineBH(235, 230, 470, 240);
    drawLineBH(470, 240, 450, 230);

    drawRect(370, 275, 400, 330);
    drawRect(365, 330, 405, 340);

    drawRect(120, 270, 160, 310);
    drawLineBH(140, 270, 140, 310);
    drawLineBH(120, 290, 160, 290);

    drawRect(70, 100, 150, 160);
    drawLineBH(97, 100, 97, 160);
    drawLineBH(123, 100, 123, 160);
    drawRect(65, 85, 155, 100);

    drawRect(250, 55, 300, 160);
    drawRect(245, 45, 305, 55);
    drawCircleMP(260, 110, 4);

    drawRect(340, 100, 420, 160);
    drawLineBH(367, 100, 367, 160);
    drawLineBH(393, 100, 393, 160);
    drawRect(335, 85, 425, 100);

    drawEllipseMP(85, 105, 12, 6);
    drawEllipseMP(110, 107, 12, 7);
    drawEllipseMP(135, 105, 12, 6);

    drawEllipseMP(355, 105, 12, 6);
    drawEllipseMP(380, 107, 12, 7);
    drawEllipseMP(405, 105, 12, 6);

    drawCircleMP(70, 430, 28);
    for (int angle = 0; angle < 360; angle += 45) {
        float rad = angle * 3.14159f / 180.0f;
        int x1 = 70 + (int)round(34 * cos(rad));
        int y1 = 430 + (int)round(34 * sin(rad));
        int x2 = 70 + (int)round(46 * cos(rad));
        int y2 = 430 + (int)round(46 * sin(rad));
        drawLineBH(x1, y1, x2, y2);
    }

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
    glutCreateWindow("Assignment 03 - Complete House (Mid-Point Circle/Ellipse & Bresenham)");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
