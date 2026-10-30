#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265f
#define WIN_W 1200
#define WIN_H 400
#define WORLD_W 30.0f
#define WORLD_H 10.0f
#define PX_PER_UNIT (WIN_W / WORLD_W)

#define MAX_PTS 32

typedef struct { float x, y; } Point2;

const Point2 SQ_C = {5, 5};    const float SQ_SIDE = 6;
const Point2 CI_C = {15, 5};   const float CI_R = 3;
const Point2 TR_A = {22, 2}, TR_B = {28, 2}, TR_T = {25, 2 + 5.196f};

Point2 sqPts[MAX_PTS], ciPts[MAX_PTS], trPts[MAX_PTS];
int sqN = 0, ciN = 0, trN = 0;

void drawLabel(float x, float y, const char* text, void* font, float lineH)
{
    char line[128];
    int i, n = 0, row = 0;
    for (i = 0;; i++)
    {
        if (text[i] == '\n' || text[i] == '\0')
        {
            line[n] = '\0';
            float w = glutBitmapLength(font, (const unsigned char*)line) / PX_PER_UNIT;
            glRasterPos2f(x - w / 2, y - row * lineH);
            for (int k = 0; line[k]; k++) glutBitmapCharacter(font, line[k]);
            row++; n = 0;
            if (text[i] == '\0') break;
        }
        else if (n < 127) line[n++] = text[i];
    }
}

void addEdgePoints(Point2* arr, int* n, Point2 a, Point2 b, int div)
{
    for (int j = 0; j < div; j++)
    {
        arr[*n].x = a.x + (b.x - a.x) * j / div;
        arr[*n].y = a.y + (b.y - a.y) * j / div;
        (*n)++;
    }
}

void buildControlPoints()
{
    Point2 c[4] = { {SQ_C.x - SQ_SIDE/2, SQ_C.y - SQ_SIDE/2}, {SQ_C.x + SQ_SIDE/2, SQ_C.y - SQ_SIDE/2},
                    {SQ_C.x + SQ_SIDE/2, SQ_C.y + SQ_SIDE/2}, {SQ_C.x - SQ_SIDE/2, SQ_C.y + SQ_SIDE/2} };
    for (int i = 0; i < 4; i++) addEdgePoints(sqPts, &sqN, c[i], c[(i + 1) % 4], 3);
    sqPts[sqN++] = SQ_C;

    for (int i = 0; i < 12; i++)
    {
        float a = i * 30.0f * PI / 180.0f;
        ciPts[ciN].x = CI_C.x + CI_R * cosf(a);
        ciPts[ciN].y = CI_C.y + CI_R * sinf(a);
        ciN++;
    }
    ciPts[ciN++] = CI_C;

    Point2 t[3] = { TR_A, TR_B, TR_T };
    for (int i = 0; i < 3; i++) addEdgePoints(trPts, &trN, t[i], t[(i + 1) % 3], 4);
    trPts[trN].x = (TR_A.x + TR_B.x + TR_T.x) / 3;
    trPts[trN].y = (TR_A.y + TR_B.y + TR_T.y) / 3;
    trN++;
}

void printPoints(const char* name, Point2* p, int n)
{
    printf("\n%s - %d control points (world coordinates)\n", name, n);
    for (int i = 0; i < n; i++) printf("  P%-2d = (%6.2f, %6.2f)\n", i + 1, p[i].x, p[i].y);
}

void drawControlPoints(Point2* p, int n, Point2 centre)
{
    glPointSize(8);
    glColor3f(0.85f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0; i < n; i++) glVertex2f(p[i].x, p[i].y);
    glEnd();

    for (int i = 0; i < n; i++)
    {
        float dx = p[i].x - centre.x, dy = p[i].y - centre.y;
        float len = sqrtf(dx * dx + dy * dy);
        if (len < 1e-3f) { dx = 0.7f; dy = 0.7f; len = 1.0f; }
        char buf[16];
        sprintf(buf, "%d", i + 1);
        glColor3f(0.0f, 0.0f, 0.0f);
        drawLabel(p[i].x + dx / len * 0.45f, p[i].y + dy / len * 0.45f - 0.1f,
                  buf, GLUT_BITMAP_HELVETICA_10, 0.4f);
    }
}

void drawGrid()
{
    glLineWidth(1);
    glColor3f(0.92f, 0.92f, 0.92f);
    glBegin(GL_LINES);
    for (int x = 0; x <= (int)WORLD_W; x++) { glVertex2f(x, 0); glVertex2f(x, WORLD_H); }
    for (int y = 0; y <= (int)WORLD_H; y++) { glVertex2f(0, y); glVertex2f(WORLD_W, y); }
    glEnd();

    glColor3f(0.4f, 0.4f, 0.4f);
    for (int x = 0; x <= (int)WORLD_W; x += 5)
    {
        char buf[16];
        sprintf(buf, "%d", x);
        drawLabel(x == 0 ? 0.3f : (x == (int)WORLD_W ? WORLD_W - 0.3f : (float)x), 0.12f,
                  buf, GLUT_BITMAP_HELVETICA_10, 0.4f);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);
    drawGrid();

    glLineWidth(2.5f);
    glColor3f(0.10f, 0.20f, 0.45f);

    glBegin(GL_LINE_LOOP);
        glVertex2f(SQ_C.x - SQ_SIDE/2, SQ_C.y - SQ_SIDE/2);
        glVertex2f(SQ_C.x + SQ_SIDE/2, SQ_C.y - SQ_SIDE/2);
        glVertex2f(SQ_C.x + SQ_SIDE/2, SQ_C.y + SQ_SIDE/2);
        glVertex2f(SQ_C.x - SQ_SIDE/2, SQ_C.y + SQ_SIDE/2);
    glEnd();

    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 120; i++)
    {
        float a = 2 * PI * i / 120;
        glVertex2f(CI_C.x + CI_R * cosf(a), CI_C.y + CI_R * sinf(a));
    }
    glEnd();

    glBegin(GL_LINE_LOOP);
        glVertex2f(TR_A.x, TR_A.y);
        glVertex2f(TR_B.x, TR_B.y);
        glVertex2f(TR_T.x, TR_T.y);
    glEnd();

    Point2 trCentre = { (TR_A.x + TR_B.x + TR_T.x) / 3, (TR_A.y + TR_B.y + TR_T.y) / 3 };
    drawControlPoints(sqPts, sqN, SQ_C);
    drawControlPoints(ciPts, ciN, CI_C);
    drawControlPoints(trPts, trN, trCentre);

    glColor3f(0, 0, 0);
    drawLabel(15, 9.35f, "Assignment 02 : Control points of Square, Circle and Triangle",
              GLUT_BITMAP_HELVETICA_18, 0.5f);
    drawLabel(SQ_C.x, 8.85f, "Square (13 control points)", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(CI_C.x, 8.85f, "Circle (13 control points)", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(TR_T.x, 8.85f, "Triangle (13 control points)", GLUT_BITMAP_HELVETICA_12, 0.4f);

    glColor3f(0.85f, 0, 0);
    drawLabel(15, 0.85f,
              "Red dots = control points (numbered P1...P13). Local coordinate system of each object = world coordinate system.",
              GLUT_BITMAP_HELVETICA_12, 0.4f);

    glFlush();
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WORLD_W, 0, WORLD_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    buildControlPoints();
    printPoints("Square", sqPts, sqN);
    printPoints("Circle", ciPts, ciN);
    printPoints("Triangle", trPts, trN);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
}
int main(int argc, char** argv){
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Assignment 02 - Control Points");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
