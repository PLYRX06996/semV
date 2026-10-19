#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265f
#define WIN_W 1200
#define WIN_H 560
#define WORLD_W 30.0f
#define WORLD_H 14.0f
#define PX_PER_UNIT (WIN_W / WORLD_W)

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

void drawArrow(float x1, float y1, float x2, float y2)
{
    glColor3f(0.25f, 0.45f, 0.75f);
    glLineWidth(1.8f);
    glBegin(GL_LINES);
        glVertex2f(x1, y1); glVertex2f(x2, y2);
    glEnd();

    float a = atan2f(y2 - y1, x2 - x1), h = 0.35f;
    glBegin(GL_TRIANGLES);
        glVertex2f(x2, y2);
        glVertex2f(x2 - h * cosf(a - 0.4f), y2 - h * sinf(a - 0.4f));
        glVertex2f(x2 - h * cosf(a + 0.4f), y2 - h * sinf(a + 0.4f));
    glEnd();
}

void drawSquare(float r, float g, float b)
{
    glColor4f(r, g, b, 0.22f);
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, -2.5f); glVertex2f(2.5f, -2.5f);
        glVertex2f(2.5f, 2.5f);   glVertex2f(-2.5f, 2.5f);
    glEnd();

    glColor3f(r, g, b);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.5f, -2.5f); glVertex2f(2.5f, -2.5f);
        glVertex2f(2.5f, 2.5f);   glVertex2f(-2.5f, 2.5f);
    glEnd();
}

void drawCircle(float r, float g, float b)
{
    const int SEG = 90;

    glColor4f(r, g, b, 0.22f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0, 0);
        for (int i = 0; i <= SEG; i++)
            glVertex2f(2.5f * cosf(2 * PI * i / SEG), 2.5f * sinf(2 * PI * i / SEG));
    glEnd();

    glColor3f(r, g, b);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < SEG; i++)
            glVertex2f(2.5f * cosf(2 * PI * i / SEG), 2.5f * sinf(2 * PI * i / SEG));
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    const float SQ[3] = { 0.15f, 0.30f, 0.75f };
    const float CI[3] = { 0.85f, 0.35f, 0.10f };

    glPushMatrix();
    glTranslatef(4, 10.2f, 0);
    drawSquare(SQ[0], SQ[1], SQ[2]);
    glPopMatrix();

    drawArrow(7.5f, 10.2f, 11, 10.2f);
    glColor3f(0, 0, 0);
    drawLabel(9.25f, 11.3f, "Scale by half\nin both directions", GLUT_BITMAP_HELVETICA_12, 0.5f);

    glPushMatrix();
    glTranslatef(14, 10.2f, 0);
    glScalef(0.5f, 0.5f, 1);
    drawSquare(SQ[0], SQ[1], SQ[2]);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(4, 3.4f, 0);
    drawCircle(CI[0], CI[1], CI[2]);
    glPopMatrix();

    drawArrow(7.5f, 3.4f, 11, 3.4f);
    glColor3f(0, 0, 0);
    drawLabel(9.25f, 4.5f, "Scale by half\nin both directions", GLUT_BITMAP_HELVETICA_12, 0.5f);

    glPushMatrix();
    glTranslatef(14, 3.4f, 0);
    glScalef(0.5f, 0.5f, 1);
    drawCircle(CI[0], CI[1], CI[2]);
    glPopMatrix();

    drawArrow(16.5f, 9.7f, 20.3f, 7.8f);
    drawArrow(16.5f, 4.0f, 20.3f, 6.2f);

    const float cx = 25, cy = 7;

    glPushMatrix();
    glTranslatef(cx, cy, 0);
    glScalef(0.5f, 0.5f, 1);
    drawSquare(SQ[0], SQ[1], SQ[2]);
    glPopMatrix();

    const float dx[4] = { -1.25f,  1.25f, -1.25f, 1.25f };
    const float dy[4] = {  1.25f,  1.25f, -1.25f, -1.25f };
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(cx + dx[i], cy + dy[i], 0);
        glScalef(0.5f, 0.5f, 1);
        drawCircle(CI[0], CI[1], CI[2]);
        glPopMatrix();
    }

    glColor3f(0, 0, 0);
    drawLabel(15, 13.3f, "Assignment 04 : Scale square and circle by half, then combine them",
              GLUT_BITMAP_HELVETICA_18, 0.5f);
    drawLabel(4, 7.1f, "Original square", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(14, 8.4f, "Scaled square", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(4, 0.4f, "Original circle", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(14, 1.9f, "Scaled circle", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(cx, 3.4f, "Show your scene", GLUT_BITMAP_HELVETICA_18, 0.5f);

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Assignment 04 - Square and Circles");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}