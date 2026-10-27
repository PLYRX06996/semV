#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265f

int   winW = 1350, winH = 560;
const float DEF_AZ = 40.0f, DEF_EL = 25.0f;
float camAz = DEF_AZ, camEl = DEF_EL;
const float CAM_DIST = 11.0f;
const float TARGET[3] = { 1.0f, 0.8f, 1.0f };

float lightAng = 63.0f;
const float LIGHT_R = 4.0f, LIGHT_Y = 6.0f;
bool  animate = false;

void getLightPos(float* p)
{
    p[0] = TARGET[0] + LIGHT_R * cosf(lightAng * PI / 180);
    p[1] = LIGHT_Y;
    p[2] = TARGET[2] + LIGHT_R * sinf(lightAng * PI / 180);
    p[3] = 1;
}

const int NUM_SCENES = 3;
const char* sceneTitle[NUM_SCENES + 1] = { "", "Scene-01", "Scene-02", "Scene-03" };
const char* sceneFormula[NUM_SCENES + 1] = {
    "",
    "Unit cube w.r.t WCS",
    "T(-1,-1,-1) Rx(30 deg) T(1,1,1) (Scene-01)",
    "T(-1,-1,-1) S(2,2,2) T(1,1,1) (Scene-01)"
};
const char* sceneNote[NUM_SCENES + 1] = {
    "",
    "cube occupies [0,1] x [0,1] x [0,1]",
    "rotation by 30 deg about the line y = -1, z = -1",
    "cube occupies [1,3] x [1,3] x [1,3]  (edge = 2)"
};
const float sceneColor[NUM_SCENES + 1][3] = {
    {0, 0, 0},
    {0.15f, 0.40f, 0.95f},
    {0.95f, 0.50f, 0.10f},
    {0.20f, 0.75f, 0.35f}
};

void transformScene(int s)
{
    switch (s)
    {
    case 1:
        break;
    case 2:
        glTranslatef(-1, -1, -1);
        glRotatef(30, 1, 0, 0);
        glTranslatef(1, 1, 1);
        break;
    case 3:
        glTranslatef(-1, -1, -1);
        glScalef(2, 2, 2);
        glTranslatef(1, 1, 1);
        break;
    }
}

void drawUnitCube()
{
    glBegin(GL_QUADS);
        glNormal3f( 1, 0, 0); glVertex3f(1,0,0); glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(1,0,1);
        glNormal3f(-1, 0, 0); glVertex3f(0,0,0); glVertex3f(0,0,1); glVertex3f(0,1,1); glVertex3f(0,1,0);
        glNormal3f( 0, 1, 0); glVertex3f(0,1,0); glVertex3f(0,1,1); glVertex3f(1,1,1); glVertex3f(1,1,0);
        glNormal3f( 0,-1, 0); glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(1,0,1); glVertex3f(0,0,1);
        glNormal3f( 0, 0, 1); glVertex3f(0,0,1); glVertex3f(1,0,1); glVertex3f(1,1,1); glVertex3f(0,1,1);
        glNormal3f( 0, 0,-1); glVertex3f(0,0,0); glVertex3f(0,1,0); glVertex3f(1,1,0); glVertex3f(1,0,0);
    glEnd();
}

void drawShadedCube(const float* c)
{
    glEnable(GL_LIGHTING);
    glColor3f(c[0], c[1], c[2]);
    drawUnitCube();
}

void drawCubeGhost()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.55f, 0.55f, 0.55f);
    glLineWidth(1.5f);
    glBegin(GL_LINES);
    for (int a = 0; a < 8; a++)
        for (int b = a + 1; b < 8; b++)
        {
            int d = a ^ b;
            if (d == 1 || d == 2 || d == 4)
            {
                glVertex3f((float)(a & 1), (float)((a >> 1) & 1), (float)((a >> 2) & 1));
                glVertex3f((float)(b & 1), (float)((b >> 1) & 1), (float)((b >> 2) & 1));
            }
        }
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawAxes(float len)
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);
        glColor3f(0.9f, 0.0f, 0.0f);  glVertex3f(0, 0, 0); glVertex3f(len, 0, 0);
        glColor3f(0.0f, 0.65f, 0.0f); glVertex3f(0, 0, 0); glVertex3f(0, len, 0);
        glColor3f(0.0f, 0.0f, 0.95f); glVertex3f(0, 0, 0); glVertex3f(0, 0, len);
    glEnd();
    glColor3f(0.9f, 0.0f, 0.0f);  glRasterPos3f(len + 0.15f, 0, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'X');
    glColor3f(0.0f, 0.65f, 0.0f); glRasterPos3f(0, len + 0.15f, 0); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Y');
    glColor3f(0.0f, 0.0f, 0.95f); glRasterPos3f(0, 0, len + 0.15f); glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, 'Z');
    glEnable(GL_LIGHTING);
}

void drawLightMarker(const float* p)
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.9f, 0.1f);
    glPushMatrix();
    glTranslatef(p[0], p[1], p[2]);
    glutSolidSphere(0.14, 16, 16);
    glPopMatrix();
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

void drawText(float nx, float ny, const char* s, void* font, int cellW)
{
    float w = glutBitmapLength(font, (const unsigned char*)s) / (float)cellW;
    glRasterPos2f(nx - w / 2, ny);
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

void cellRect(int idx, int cols, int* x, int* y, int* w, int* h)
{
    *w = winW / cols;
    *h = winH;
    *x = idx * (*w);
    *y = 0;
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

    GLfloat lightPos[4];
    getLightPos(lightPos);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
}

void drawSceneCell(int s, int idx)
{
    int x, y, w, h;
    cellRect(idx, NUM_SCENES, &x, &y, &w, &h);

    int vy = y + (int)(0.07f * h), vh = (int)(0.76f * h);
    glViewport(x, vy, w, vh);
    glClear(GL_DEPTH_BUFFER_BIT);
    setupCamera(w, vh);

    GLfloat lp[4];
    getLightPos(lp);

    drawAxes(3.6f);
    if (s != 1) drawCubeGhost();
    glPushMatrix();
    transformScene(s);
    drawShadedCube(sceneColor[s]);
    glPopMatrix();
    drawLightMarker(lp);

    glViewport(x, y, w, h);
    beginOverlay();
    drawCellFrame();
    glColor3f(0, 0, 0);
    drawText(0.5f, 0.935f, sceneTitle[s], GLUT_BITMAP_HELVETICA_18, w);
    glColor3f(0.1f, 0.2f, 0.6f);
    drawText(0.5f, 0.88f, sceneFormula[s], GLUT_BITMAP_HELVETICA_12, w);
    glColor3f(0.3f, 0.3f, 0.3f);
    drawText(0.5f, 0.075f, sceneNote[s], GLUT_BITMAP_HELVETICA_12, w);
    glColor3f(0.75f, 0.1f, 0.1f);
    drawText(0.5f, 0.03f, "Flat shading : glShadeModel(GL_FLAT) - one colour per face", GLUT_BITMAP_HELVETICA_12, w);
    endOverlay();
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_FLAT);
    for (int s = 1; s <= NUM_SCENES; s++) drawSceneCell(s, s - 1);
    glutSwapBuffers();
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    GLfloat ambient[]  = { 0.22f, 0.22f, 0.22f, 1 };
    GLfloat diffuse[]  = { 1.00f, 1.00f, 1.00f, 1 };
    GLfloat specular[] = { 1.00f, 1.00f, 1.00f, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT,  ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

    GLfloat globalAmbient[] = { 0.08f, 0.08f, 0.08f, 1 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    GLfloat matSpec[] = { 0.35f, 0.35f, 0.35f, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, matSpec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 30.0f);

    glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h)
{
    winW = w;
    winH = (h > 0) ? h : 1;
}

void timer(int)
{
    if (animate) lightAng += 1.5f;
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
    if (key == 'a' || key == 'A') animate = !animate;
    if (key == 'r' || key == 'R') { camAz = DEF_AZ; camEl = DEF_EL; lightAng = 63.0f; animate = false; }
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
    glutCreateWindow("Lab 10 - Assignment 02 : Flat Shading  (arrows: rotate view, A: move light, ESC: quit)");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(0, timer, 0);
    glutMainLoop();
    return 0;
}
