#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265f
#define WIN_W 1344
#define WIN_H 512
#define WORLD_W 42.0f
#define WORLD_H 16.0f
#define PX_PER_UNIT (WIN_W / WORLD_W)

#define TRI_H 4.3301f

const float BLUE[3]   = { 0.15f, 0.30f, 0.75f };
const float ORANGE[3] = { 0.90f, 0.45f, 0.05f };

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

    float a = atan2f(y2 - y1, x2 - x1), h = 0.4f;
    glBegin(GL_TRIANGLES);
        glVertex2f(x2, y2);
        glVertex2f(x2 - h * cosf(a - 0.4f), y2 - h * sinf(a - 0.4f));
        glVertex2f(x2 - h * cosf(a + 0.4f), y2 - h * sinf(a + 0.4f));
    glEnd();
}

void drawSquare(const float* c)
{
    glColor4f(c[0], c[1], c[2], 0.22f);
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, -2.5f); glVertex2f(2.5f, -2.5f);
        glVertex2f(2.5f, 2.5f);   glVertex2f(-2.5f, 2.5f);
    glEnd();

    glColor3f(c[0], c[1], c[2]);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.5f, -2.5f); glVertex2f(2.5f, -2.5f);
        glVertex2f(2.5f, 2.5f);   glVertex2f(-2.5f, 2.5f);
    glEnd();
}

void drawTriangle(const float* c)
{
    glColor4f(c[0], c[1], c[2], 0.22f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-2.5f, 0); glVertex2f(2.5f, 0); glVertex2f(0, TRI_H);
    glEnd();

    glColor3f(c[0], c[1], c[2]);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.5f, 0); glVertex2f(2.5f, 0); glVertex2f(0, TRI_H);
    glEnd();
}

void shearX(float k)
{
    GLfloat m[16] = { 1, 0, 0, 0,
                      k, 1, 0, 0,
                      0, 0, 1, 0,
                      0, 0, 0, 1 };
    glMultMatrixf(m);
}

void drawShearedSquare(float cx, float cy)
{
    glPushMatrix();
    glTranslatef(cx, cy, 0);
    shearX(2.0f);
    glScalef(0.5f, 0.5f, 1);
    drawSquare(BLUE);
    glPopMatrix();
}

void drawRotatedTriangle(float bx, float by)
{
    glPushMatrix();
    glTranslatef(bx, by, 0);
    glRotatef(-30, 0, 0, 1);
    glScalef(0.5f, 0.5f, 1);
    drawTriangle(ORANGE);
    glPopMatrix();
}


void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    glPushMatrix();
    glTranslatef(4, 12, 0);
    drawSquare(BLUE);
    glPopMatrix();

    drawArrow(7.2f, 12, 10.2f, 12);
    glColor3f(0, 0, 0);
    drawLabel(8.7f, 13.1f, "Scale by half\nin both directions", GLUT_BITMAP_HELVETICA_12, 0.6f);

    glPushMatrix();
    glTranslatef(13, 12, 0);
    glScalef(0.5f, 0.5f, 1);
    drawSquare(BLUE);
    glPopMatrix();

    drawArrow(16.2f, 12, 18.4f, 12);
    glColor3f(0, 0, 0);
    drawLabel(17.3f, 13.1f, "Shear along\nx-direction by 2", GLUT_BITMAP_HELVETICA_12, 0.6f);

    drawShearedSquare(23, 12);

    glPushMatrix();
    glTranslatef(4, 1.5f, 0);
    drawTriangle(ORANGE);
    glPopMatrix();

    drawArrow(7.2f, 3.5f, 10.2f, 3.5f);
    glColor3f(0, 0, 0);
    drawLabel(8.7f, 4.6f, "Scale by half\nin both directions", GLUT_BITMAP_HELVETICA_12, 0.6f);

    glPushMatrix();
    glTranslatef(13, 2.5f, 0);
    glScalef(0.5f, 0.5f, 1);
    drawTriangle(ORANGE);
    glPopMatrix();

    drawArrow(16.2f, 3.5f, 19.5f, 3.5f);
    glColor3f(0, 0, 0);
    drawLabel(17.85f, 4.6f, "Rotate it by\n-30 degrees", GLUT_BITMAP_HELVETICA_12, 0.6f);

    drawRotatedTriangle(23, 3);

    drawArrow(27.8f, 11.0f, 31.2f, 8.6f);
    drawArrow(26.0f, 4.0f, 31.2f, 5.8f);

    const float hullX = 36, hullY = 6.5f;
    drawShearedSquare(hullX, hullY);

    const float footX = hullX + 2.0f * 1.25f;
    const float footY = hullY + 1.25f;

    const float a = -30.0f * PI / 180.0f;
    const float cornerX = 1.25f * cosf(a);
    const float cornerY = 1.25f * sinf(a);
    drawRotatedTriangle(footX - cornerX, footY - cornerY);

    glColor3f(0, 0, 0);
    drawLabel(21, 15.2f, "Assignment 05 : Scale, shear and rotate, then join the shapes",
              GLUT_BITMAP_HELVETICA_18, 0.6f);
    drawLabel(4, 9.0f, "Original square", GLUT_BITMAP_HELVETICA_12, 0.5f);
    drawLabel(13, 10.0f, "Scaled square", GLUT_BITMAP_HELVETICA_12, 0.5f);
    drawLabel(23, 9.8f, "Sheared square", GLUT_BITMAP_HELVETICA_12, 0.5f);
    drawLabel(4, 0.7f, "Original triangle", GLUT_BITMAP_HELVETICA_12, 0.5f);
    drawLabel(13, 1.4f, "Scaled triangle", GLUT_BITMAP_HELVETICA_12, 0.5f);
    drawLabel(23, 1.4f, "Rotated triangle", GLUT_BITMAP_HELVETICA_12, 0.5f);
    drawLabel(36, 3.2f, "Show your scene", GLUT_BITMAP_HELVETICA_18, 0.6f);
    drawLabel(36, 2.4f, "(sheared hull + rotated sail)", GLUT_BITMAP_HELVETICA_12, 0.5f);

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
    glutCreateWindow("Assignment 05 - Shear, Scale, Rotate");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}