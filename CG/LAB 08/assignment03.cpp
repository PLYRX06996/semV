#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define WIN_W 1200
#define WIN_H 400
#define WORLD_W 30.0f
#define WORLD_H 10.0f
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

void drawSquare(float r, float g, float b, float lineWidth)
{
    glColor4f(r, g, b, 0.22f);
    glBegin(GL_QUADS);
        glVertex2f(-3, -3); glVertex2f(3, -3);
        glVertex2f(3, 3);   glVertex2f(-3, 3);
    glEnd();

    glColor3f(r, g, b);
    glLineWidth(lineWidth);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-3, -3); glVertex2f(3, -3);
        glVertex2f(3, 3);   glVertex2f(-3, 3);
    glEnd();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(4, 5, 0);
    drawSquare(0.15f, 0.30f, 0.75f, 3.0f);
    glPopMatrix();

    drawArrow(8, 5, 11, 5);
    glColor3f(0, 0, 0);
    drawLabel(9.5f, 6.15f, "Scale by half\nin both directions", GLUT_BITMAP_HELVETICA_12, 0.5f);

    glPushMatrix();
    glTranslatef(14, 5, 0);
    glScalef(0.5f, 0.5f, 1);
    drawSquare(0.15f, 0.30f, 0.75f, 3.0f);
    glPopMatrix();

    drawArrow(17, 5, 20, 5);
    glColor3f(0, 0, 0);
    drawLabel(18.5f, 6.15f, "Create 3 copies,\nrotate each by 30 deg", GLUT_BITMAP_HELVETICA_12, 0.5f);

    const float col[4][3] = { {0.15f, 0.30f, 0.75f},
                              {0.90f, 0.45f, 0.05f},
                              {0.10f, 0.60f, 0.25f},
                              {0.60f, 0.15f, 0.70f} };
    const float lw[4] = { 5.0f, 3.0f, 3.0f, 1.8f };

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(25, 5, 0);
        glRotatef(30.0f * i, 0, 0, 1);
        glScalef(0.5f, 0.5f, 1);
        drawSquare(col[i][0], col[i][1], col[i][2], lw[i]);
        glPopMatrix();
    }

    glColor3f(0, 0, 0);
    drawLabel(15, 9.35f, "Assignment 03 : Scale by half, then 3 copies rotated by 30 deg each",
              GLUT_BITMAP_HELVETICA_18, 0.5f);
    drawLabel(4, 1.2f, "Original square (side 6)", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(14, 2.9f, "Scaled square (side 3)", GLUT_BITMAP_HELVETICA_12, 0.4f);
    drawLabel(25, 2.0f, "Show your scene", GLUT_BITMAP_HELVETICA_18, 0.5f);
    drawLabel(25, 1.2f, "0 deg (blue) + copies at 30, 60, 90 deg", GLUT_BITMAP_HELVETICA_12, 0.4f);

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
    glutCreateWindow("Assignment 03 - Scale and Rotate Copies");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}