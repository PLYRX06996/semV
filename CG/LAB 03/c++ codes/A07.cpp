#include <GL/glut.h>
#include <cmath>
#include <algorithm>

void drawLineDDA(float x1, float y1, float x2, float y2) {
    float dx = x2 - x1;
    float dy = y2 - y1;
    float steps = std::max(std::abs(dx), std::abs(dy));
    if (steps == 0) { glBegin(GL_POINTS); glVertex2i((int)round(x1), (int)round(y1)); glEnd(); return; }

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
    int dx = std::abs(xe - x), dy = std::abs(ye - y);
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

void drawSegment(float x1, float y1, float x2, float y2) {
    drawLineDDA(x1, y1, x2, y2);
}

void drawRect(float x1, float y1, float x2, float y2) {
    drawSegment(x1, y1, x2, y1);
    drawSegment(x2, y1, x2, y2);
    drawSegment(x2, y2, x1, y2);
    drawSegment(x1, y2, x1, y1);
}

void drawTempleIsometric(float ox, float oy) {
    float bx1 = ox + 1.0f,  by1 = oy + 0.0f;
    float bx2 = ox + 11.0f, by2 = oy + 0.0f;
    float bx3 = ox + 13.0f, by3 = oy + 1.0f;
    float bx4 = ox + 3.0f,  by4 = oy + 1.0f;
    drawSegment(bx1, by1, bx2, by2);
    drawSegment(bx2, by2, bx3, by3);
    drawSegment(bx3, by3, bx4, by4);
    drawSegment(bx4, by4, bx1, by1);

    float mx1 = ox + 1.5f,  my1 = oy + 1.0f;
    float mx2 = ox + 10.5f, my2 = oy + 1.0f;
    float mx3 = ox + 12.5f, my3 = oy + 2.0f;
    float mx4 = ox + 3.5f,  my4 = oy + 2.0f;
    drawSegment(mx1, my1, mx2, my2);
    drawSegment(mx2, my2, mx3, my3);
    drawSegment(mx3, my3, mx4, my4);
    drawSegment(mx4, my4, mx1, my1);

    float tx1 = ox + 2.0f,  ty1 = oy + 2.0f;
    float tx2 = ox + 10.0f, ty2 = oy + 2.0f;
    float tx3 = ox + 12.0f, ty3 = oy + 3.0f;
    float tx4 = ox + 4.0f,  ty4 = oy + 3.0f;
    drawSegment(tx1, ty1, tx2, ty2);
    drawSegment(tx2, ty2, tx3, ty3);
    drawSegment(tx3, ty3, tx4, ty4);
    drawSegment(tx4, ty4, tx1, ty1);

    float pillarBase = oy + 3.0f;
    float pillarTop  = oy + 8.0f;
    int numPillars = 8;
    for (int i = 0; i < numPillars; i++) {
        float px = ox + 3.0f + i * 1.0f;
        drawSegment(px, pillarBase, px, pillarTop);
    }

    float rbx1 = ox + 2.5f,  rby1 = oy + 8.0f;
    float rbx2 = ox + 11.0f, rby2 = oy + 8.0f;
    float rbx3 = ox + 13.0f, rby3 = oy + 9.0f;
    float rbx4 = ox + 4.5f,  rby4 = oy + 9.0f;
    drawSegment(rbx1, rby1, rbx2, rby2);
    drawSegment(rbx2, rby2, rbx3, rby3);
    drawSegment(rbx3, rby3, rbx4, rby4);
    drawSegment(rbx4, rby4, rbx1, rby1);

    float rfLx = ox + 2.5f,  rfLy = oy + 9.0f;
    float rfRx = ox + 11.0f, rfRy = oy + 9.0f;
    float rfTx = ox + 6.75f, rfTy = oy + 11.5f;
    drawSegment(rfLx, rfLy, rfRx, rfRy);
    drawSegment(rfLx, rfLy, rfTx, rfTy);
    drawSegment(rfRx, rfRy, rfTx, rfTy);

    float rrTx = ox + 8.75f, rrTy = oy + 12.5f;
    drawSegment(rfTx, rfTy, rrTx, rrTy);

    float brRx = ox + 13.0f, brRy = oy + 9.0f;
    drawSegment(rfRx, rfRy, brRx, brRy);
    drawSegment(brRx, brRy, rrTx, rrTy);

    float brLx = ox + 4.5f, brLy = oy + 9.0f;
    drawSegment(rfLx, rfLy, brLx, brLy);
    drawSegment(brLx, brLy, rrTx, rrTy);
}

void drawTempleFront(float ox, float oy) {
    drawRect(ox + 0.5f, oy + 0.0f, ox + 13.5f, oy + 0.8f);
    drawRect(ox + 1.5f, oy + 0.8f, ox + 12.5f, oy + 1.6f);
    drawRect(ox + 2.5f, oy + 1.6f, ox + 11.5f, oy + 2.2f);

    float pillarBase = oy + 2.2f;
    float pillarTop  = oy + 8.0f;
    float pillarPositions[] = {4.0f, 5.0f, 6.0f, 8.0f, 9.0f, 10.0f};
    for (int i = 0; i < 6; i++) {
        float px = ox + pillarPositions[i];
        drawSegment(px, pillarBase, px, pillarTop);
    }

    drawRect(ox + 2.5f, oy + 8.0f, ox + 11.5f, oy + 8.8f);

    float roofLx = ox + 2.5f,  roofLy = oy + 8.8f;
    float roofRx = ox + 11.5f, roofRy = oy + 8.8f;
    float roofTx = ox + 7.0f,  roofTy = oy + 11.5f;
    drawSegment(roofLx, roofLy, roofTx, roofTy);
    drawSegment(roofTx, roofTy, roofRx, roofRy);
    drawSegment(roofLx, roofLy, roofRx, roofRy);
}

void drawWindow(float ox, float oy) {
    drawRect(ox + 0.5f, oy + 0.5f, ox + 12.5f, oy + 14.5f);

    drawRect(ox + 1.5f, oy + 1.5f, ox + 11.5f, oy + 13.5f);

    drawRect(ox + 3.0f, oy + 2.5f, ox + 6.25f, oy + 12.5f);

    drawRect(ox + 6.75f, oy + 2.5f, ox + 10.0f, oy + 12.5f);
}

void drawTempleFlatRoof(float ox, float oy) {
    drawRect(ox + 0.5f, oy + 0.0f, ox + 13.5f, oy + 0.8f);
    drawRect(ox + 1.5f, oy + 0.8f, ox + 12.5f, oy + 1.6f);
    drawRect(ox + 2.5f, oy + 1.6f, ox + 11.5f, oy + 2.2f);

    float pillarBase = oy + 2.2f;
    float pillarTop  = oy + 8.5f;
    int numPillars = 8;
    for (int i = 0; i < numPillars; i++) {
        float px = ox + 3.5f + i * 0.85f;
        drawSegment(px, pillarBase, px, pillarTop);
    }

    drawSegment(ox + 2.5f, oy + 8.5f, ox + 11.5f, oy + 8.5f);

    drawRect(ox + 2.0f, oy + 8.5f, ox + 12.0f, oy + 10.5f);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glColor3f(0.0f, 0.8f, 0.2f);
    glPointSize(2.0f);
    glPushMatrix();
    glTranslatef(1.0f, 17.0f, 0.0f);
    glScalef(1.0f, 1.0f, 1.0f);
    drawTempleIsometric(0.0f, 0.0f);
    glPopMatrix();

    glColor3f(0.2f, 0.4f, 1.0f);
    glPushMatrix();
    glTranslatef(17.0f, 17.0f, 0.0f);
    drawTempleFront(0.0f, 0.0f);
    glPopMatrix();

    glColor3f(1.0f, 0.5f, 0.0f);
    glPushMatrix();
    glTranslatef(2.0f, 1.0f, 0.0f);
    drawWindow(0.0f, 0.0f);
    glPopMatrix();

    glColor3f(0.8f, 0.0f, 0.5f);
    glPushMatrix();
    glTranslatef(17.0f, 2.0f, 0.0f);
    drawTempleFlatRoof(0.0f, 0.0f);
    glPopMatrix();

    glFlush();
}

void init() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Assignment 07: Four Graphics (DDA & Bresenham)");
    init();
    gluOrtho2D(0.0, 32.0, 0.0, 32.0);
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
