#include <GL/glut.h>
#include <cmath>
#include<bits/stdc++.h>

void drawLineDDA(float x1, float y1, float x2, float y2){

    float dx = x2 - x1;
    float dy = y2 - y1;

    float S = std::max(abs(dx), abs(dy));

    float Xincr = dx / S;
    float Yincr = dy / S;

    float x = x1;
    float y = y1;

    glBegin(GL_POINTS);
    for (int i = 0; i <= S; i++){
        glVertex2i(round(x), round(y));
        x += Xincr;
        y += Yincr;
    }
    glEnd();
}

void drawLineBH(float x1, float y1, float x2, float y2){
    int x = round(x1);
    int y = round(y1);
    int x_ = round(x2);
    int y_ = round(y2);

    int dx = abs(x_ - x);
    int dy = abs(y_ - y);
    int sx = (x < x_) ? 1 : -1;
    int sy = (y < y_) ? 1 : -1;
    int E = dx - dy;

    glBegin(GL_POINTS);
    while (true)
    {
        glVertex2i(x, y);
        if(x == x_ && y == y_) break;
        int e = 2 * E;

        if(e > -dy){
            E -= dy;
            x += sx;
        }
        if(e < dx){
            E += dx;
            y += sy;
        }
    }
    glEnd();
}
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    float x1 = 2.8f, y1 = 1.1f;
    float x2 = 9.2f, y2 = 4.9f;
    glColor3f(1.0f, 0.0f, 0.0f);
    glLineWidth(2.0f);
    glColor3f(0.0f, 1.0f, 0.0f);
    glPointSize(8.0f);

    drawLineDDA(x1, y1, x2, y2);
    drawLineBH(x1, y1, x2, y2);
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