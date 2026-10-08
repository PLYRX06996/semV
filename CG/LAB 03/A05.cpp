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
void drawHouseSegment(float x1, float y1, float x2, float y2) {
 
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

    float p1x = 7.5f,  p1y = 9.2f;
    float p2x = 4.4f,  p2y = 5.4f;
    float p3x = 4.4f,  p3y = 0.4f;
    float p4x = 10.4f, p4y = 0.4f;
    float p5x = 10.4f, p5y = 5.4f;

    drawHouseSegment(p3x, p3y, p4x, p4y); // Floor
    drawHouseSegment(p4x, p4y, p5x, p5y); // Right wall
    drawHouseSegment(p5x, p5y, p1x, p1y); // Right roof
    drawHouseSegment(p1x, p1y, p2x, p2y); // Left roof
    drawHouseSegment(p2x, p2y, p3x, p3y); // Left wall
    // drawLineDDA(p2x, p2y, p5x, p5y);
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