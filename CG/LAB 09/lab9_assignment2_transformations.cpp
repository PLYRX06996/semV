#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265f

int   winW = 1400, winH = 700;
float camAz = 40.0f, camEl = 25.0f;
const float CAM_DIST = 13.0f;
const float TARGET[3] = { 1.5f, 1.5f, 1.5f };

const int NUM_SCENES = 7;
int activeView = 0;
bool showLegendInCell7 = false;

const int parentOf[NUM_SCENES + 1] = { 0, 0, 1, 2, 3, 3, 3, 3 };

const char* sceneTitle[NUM_SCENES + 1] = {
    "",
    "Scene-01",
    "Scene-02",
    "Scene-03",
    "Scene-04",
    "Scene-05",
    "Scene-06",
    "Scene-07"
};
const char* sceneFormula[NUM_SCENES + 1] = {
    "",
    "Unit cube w.r.t WCS",
    "T(1,1,1) (Scene-01)",
    "S(2,2,2) (Scene-02)",
    "Rx(30 deg) (Scene-03)",
    "Ry(30 deg) (Scene-03)",
    "Rz(30 deg) (Scene-03)",
    "Rotate 30 deg about (x-1)/1=(y-2)/2=(z-3)/3 (Scene-03)"
};
const char* sceneNote[NUM_SCENES + 1] = {
    "",
    "cube occupies [0,1] x [0,1] x [0,1]",
    "cube occupies [1,2] x [1,2] x [1,2]",
    "cube occupies [2,4] x [2,4] x [2,4]  (edge = 2)",
    "rotation about the X axis",
    "rotation about the Y axis",
    "rotation about the Z axis",
    "axis passes through (1,2,3), direction (1,2,3)"
};
const float sceneColor[NUM_SCENES + 1][3] = {
    {0, 0, 0},
    {0.10f, 0.30f, 1.00f},
    {0.10f, 0.65f, 0.85f},
    {0.15f, 0.70f, 0.30f},
    {0.95f, 0.55f, 0.10f},
    {0.85f, 0.20f, 0.25f},
    {0.60f, 0.30f, 0.80f},
    {0.90f, 0.75f, 0.10f}
};

void transformScene(int s)
{
    switch (s)
    {
    case 1:
        break;

    case 2:
        glTranslatef(1, 1, 1);
        transformScene(1);
        break;

    case 3:
        glScalef(2, 2, 2);
        transformScene(2);
        break;

    case 4:
        glRotatef(30, 1, 0, 0);
        transformScene(3);
        break;

    case 5:
        glRotatef(30, 0, 1, 0);
        transformScene(3);
        break;

    case 6:
        glRotatef(30, 0, 0, 1);
        transformScene(3);
        break;

    case 7:
        glTranslatef(1, 2, 3);
        glRotatef(30, 1, 2, 3);
        glTranslatef(-1, -2, -3);
        transformScene(3);
        break;
    }
}

void drawCubeSolid(const float* c)
{
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.5f);

    glEnable(GL_LIGHTING);
    glEnable(GL_POLYGON_OFFSET_FILL);
    glPolygonOffset(1.0f, 1.0f);
    glColor3f(c[0], c[1], c[2]);
    glutSolidCube(1.0);
    glDisable(GL_POLYGON_OFFSET_FILL);

    glDisable(GL_LIGHTING);
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glutWireCube(1.0);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawCubeGhost()
{
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.5f);
    glDisable(GL_LIGHTING);
    glColor3f(0.55f, 0.55f, 0.55f);
    glLineWidth(1.5f);
    glutWireCube(1.0);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawAxes(float len)
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glColor3f(0.9f, 0.0f, 0.0f); glVertex3f(0, 0, 0); glVertex3f(len, 0, 0);
        glColor3f(0.0f, 0.65f, 0.0f); glVertex3f(0, 0, 0); glVertex3f(0, len, 0);
        glColor3f(0.0f, 0.0f, 0.95f); glVertex3f(0, 0, 0); glVertex3f(0, 0, len);
    glEnd();

    glColor3f(0.9f, 0.0f, 0.0f);  glRasterPos3f(len + 0.15f, 0, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
    glColor3f(0.0f, 0.65f, 0.0f); glRasterPos3f(0, len + 0.15f, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
    glColor3f(0.0f, 0.0f, 0.95f); glRasterPos3f(0, 0, len + 0.15f); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
    glEnable(GL_LIGHTING);
}

void drawRotationAxis()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.85f, 0.0f, 0.85f);
    glLineWidth(2.5f);
    glBegin(GL_LINES);
        glVertex3f(1 - 1.3f, 2 - 2.6f, 3 - 3.9f);
        glVertex3f(1 + 0.6f, 2 + 1.2f, 3 + 1.8f);
    glEnd();
    glEnable(GL_LIGHTING);
}

void beginOverlay()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 1, 0, 1);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();
}

void endOverlay()
{
    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void drawText(float nx, float ny, const char* s, void* font, int cellW, bool centred)
{
    float w = glutBitmapLength(font, (const unsigned char*)s) / (float)cellW;
    glRasterPos2f(centred ? nx - w / 2 : nx, ny);
    for (; *s; s++) glutBitmapCharacter(font, *s);
}

void drawCellFrame()
{
    glColor3f(0.75f, 0.75f, 0.75f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.002f, 0.002f); glVertex2f(0.998f, 0.002f);
        glVertex2f(0.998f, 0.998f); glVertex2f(0.002f, 0.998f);
    glEnd();
}

void cellRect(int idx, int cols, int rows, int* x, int* y, int* w, int* h)
{
    int cw = winW / cols, ch = winH / rows;
    *x = (idx % cols) * cw;
    *y = winH - (idx / cols + 1) * ch;
    *w = cw;
    *h = ch;
}

void setupCamera(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, (double)w / h, 1, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float az = camAz * PI / 180, el = camEl * PI / 180;
    gluLookAt(TARGET[0] + CAM_DIST * cosf(el) * sinf(az),
              TARGET[1] + CAM_DIST * sinf(el),
              TARGET[2] + CAM_DIST * cosf(el) * cosf(az),
              TARGET[0], TARGET[1], TARGET[2],
              0, 1, 0);

    GLfloat lightPos[] = { 6, 10, 8, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void drawSceneCell(int s, int idx)
{
    int x, y, w, h;
    cellRect(idx, 4, 2, &x, &y, &w, &h);

    int vy = y + (int)(0.07f * h), vh = (int)(0.76f * h);
    glViewport(x, vy, w, vh);
    glClear(GL_DEPTH_BUFFER_BIT);
    setupCamera(w, vh);

    drawAxes(4.5f);
    if (parentOf[s]) {
        glPushMatrix();
        transformScene(parentOf[s]);
        drawCubeGhost();
        glPopMatrix();
    }
    glPushMatrix();
    transformScene(s);
    drawCubeSolid(sceneColor[s]);
    glPopMatrix();
    if (s == 7) drawRotationAxis();

    glViewport(x, y, w, h);
    beginOverlay();
    drawCellFrame();
    glColor3f(0, 0, 0);
    drawText(0.5f, 0.925f, sceneTitle[s], GLUT_BITMAP_HELVETICA_18, w, true);
    glColor3f(0.1f, 0.2f, 0.6f);
    drawText(0.5f, 0.865f, sceneFormula[s], GLUT_BITMAP_HELVETICA_12, w, true);
    glColor3f(0.3f, 0.3f, 0.3f);
    drawText(0.5f, 0.04f, sceneNote[s], GLUT_BITMAP_HELVETICA_12, w, true);
    endOverlay();
}

void drawCombinedScene()
{
    drawAxes(4.5f);
    for (int s = 1; s <= 7; s++)
    {
        glPushMatrix();
        transformScene(s);
        drawCubeSolid(sceneColor[s]);
        glPopMatrix();
    }
    drawRotationAxis();
}

void drawCombinedCell(int idx)
{
    int x, y, w, h;
    cellRect(idx, 4, 2, &x, &y, &w, &h);

    int vy = y + (int)(0.07f * h), vh = (int)(0.76f * h);
    glViewport(x, vy, w, vh);
    glClear(GL_DEPTH_BUFFER_BIT);
    setupCamera(w, vh);

    drawCombinedScene();

    glViewport(x, y, w, h);
    beginOverlay();
    drawCellFrame();
    glColor3f(0, 0, 0);
    drawText(0.5f, 0.925f, "Combined Scene", GLUT_BITMAP_HELVETICA_18, w, true);
    glColor3f(0.1f, 0.2f, 0.6f);
    drawText(0.5f, 0.865f, "All Transformations (Scene 01-07)", GLUT_BITMAP_HELVETICA_12, w, true);
    glColor3f(0.3f, 0.3f, 0.3f);
    drawText(0.5f, 0.04f, "all 7 transformed cubes in WCS (L: Legend)", GLUT_BITMAP_HELVETICA_12, w, true);
    endOverlay();
}

void drawLegendCell(int idx)
{
    int x, y, w, h;
    cellRect(idx, 4, 2, &x, &y, &w, &h);
    glViewport(x, y, w, h);

    beginOverlay();
    drawCellFrame();
    glColor3f(0, 0, 0);
    drawText(0.5f, 0.925f, "Legend", GLUT_BITMAP_HELVETICA_18, w, true);

    const char* lines[] = {
        "Assignment 02 : 3D transformations of a unit cube",
        "Unit cube = [0,1] x [0,1] x [0,1] in the WCS",
        "",
        "Solid cube : result of the scene",
        "Grey wire  : operand (scene it came from)",
        "Magenta line (Scene-07) : rotation axis",
        "X axis red, Y axis green, Z axis blue",
        "",
        "Keys [0]: Grid View   |   [1-7]: View Scene 01-07",
        "Key  [8/C]: Combined Scene full screen",
        "Key  [L]: Toggle Legend in Grid Cell 8",
        "Arrow keys : rotate camera view",
        "R : reset view      ESC : quit"
    };
    float ny = 0.84f;
    for (int i = 0; i < 13; i++)
    {
        glColor3f(0.15f, 0.15f, 0.15f);
        drawText(0.06f, ny, lines[i], GLUT_BITMAP_HELVETICA_12, w, false);
        ny -= 0.058f;
    }
    endOverlay();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (activeView == 0)
    {
        for (int s = 1; s <= NUM_SCENES; s++) drawSceneCell(s, s - 1);
        if (showLegendInCell7)
            drawLegendCell(7);
        else
            drawCombinedCell(7);
    }
    else if (activeView >= 1 && activeView <= 7)
    {
        glViewport(0, 0, winW, winH);
        glClear(GL_DEPTH_BUFFER_BIT);
        setupCamera(winW, winH);

        drawAxes(4.5f);
        if (parentOf[activeView])
        {
            glPushMatrix();
            transformScene(parentOf[activeView]);
            drawCubeGhost();
            glPopMatrix();
        }
        glPushMatrix();
        transformScene(activeView);
        drawCubeSolid(sceneColor[activeView]);
        glPopMatrix();
        if (activeView == 7) drawRotationAxis();

        beginOverlay();
        glColor3f(0, 0, 0);
        drawText(0.5f, 0.95f, sceneTitle[activeView], GLUT_BITMAP_HELVETICA_18, winW, true);
        glColor3f(0.1f, 0.2f, 0.6f);
        drawText(0.5f, 0.91f, sceneFormula[activeView], GLUT_BITMAP_HELVETICA_12, winW, true);
        glColor3f(0.3f, 0.3f, 0.3f);
        drawText(0.5f, 0.03f, sceneNote[activeView], GLUT_BITMAP_HELVETICA_12, winW, true);
        drawText(0.02f, 0.03f, "Key [0]: Back to Grid", GLUT_BITMAP_HELVETICA_12, winW, false);
        endOverlay();
    }
    else if (activeView == 8)
    {
        glViewport(0, 0, winW, winH);
        glClear(GL_DEPTH_BUFFER_BIT);
        setupCamera(winW, winH);

        drawCombinedScene();

        beginOverlay();
        glColor3f(0, 0, 0);
        drawText(0.5f, 0.95f, "Combined Scene - All Transformations", GLUT_BITMAP_HELVETICA_18, winW, true);
        glColor3f(0.1f, 0.2f, 0.6f);
        drawText(0.5f, 0.91f, "All 7 Transformed Cubes Superimposed in WCS", GLUT_BITMAP_HELVETICA_12, winW, true);
        glColor3f(0.3f, 0.3f, 0.3f);
        drawText(0.02f, 0.03f, "Key [0]: Back to Grid   |   Arrows: Rotate   |   R: Reset", GLUT_BITMAP_HELVETICA_12, winW, false);
        endOverlay();
    }

    glutSwapBuffers();
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat ambient[] = { 0.25f, 0.25f, 0.25f, 1 };
    GLfloat diffuse[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h)
{
    winW = w;
    winH = h;
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
    if (key == 'r' || key == 'R') { camAz = 40; camEl = 25; }
    if (key >= '0' && key <= '7') activeView = key - '0';
    if (key == '8' || key == 'c' || key == 'C') activeView = 8;
    if (key == 'l' || key == 'L') showLegendInCell7 = !showLegendInCell7;
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)  camAz -= 5;
    if (key == GLUT_KEY_RIGHT) camAz += 5;
    if (key == GLUT_KEY_UP)    camEl += 5;
    if (key == GLUT_KEY_DOWN)  camEl -= 5;
    if (camEl > 85) camEl = 85;
    if (camEl < -85) camEl = -85;
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Lab 09 - Assignment 02 : 3D Transformations (Keys [0]: Grid | [1-7]: Scenes | [8/C]: Combined | L: Legend)");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
