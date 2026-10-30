#include <GL/freeglut.h>
#include <cmath>
#include <cstdio>
#include <cstdlib>

#define PI 3.14159265f

int   winW = 1200, winH = 750;
const float DEF_AZ = 35.0f, DEF_EL = 16.0f, DEF_DIST = 10.5f;
float camAz = DEF_AZ, camEl = DEF_EL, camDist = DEF_DIST;
const float TARGET[3] = { 0.0f, 1.8f, 0.0f };

bool dragging = false;
int  lastX = 0, lastY = 0;

GLenum currentShadeModel = GL_SMOOTH;

float sunAngle = 45.0f;
const float SUN_DIST = 9.5f;
const float SUN_Y = 8.5f;
bool  animateSun = true;

bool  fillLightEnabled = true;

bool  headlampEnabled = true;

bool  animatePose = true;
float animT = 0.0f;

struct Pose {
    float headYaw;
    float headPitch;
    float brow;
    float armL;
    float armR;
};
Pose pose;

void updatePose()
{
    pose.headYaw   = 20.0f * sinf(0.8f * animT);
    pose.headPitch = 7.0f + 6.0f * sinf(1.3f * animT);
    pose.brow      = 15.0f + 8.0f * sinf(1.6f * animT);
    pose.armL      = 12.0f * sinf(1.1f * animT);
    pose.armR      = -12.0f * sinf(1.1f * animT);
}

void setMaterial(float dr, float dg, float db,
                 float sr, float sg, float sb,
                 float shininess, float ambScale = 0.22f)
{
    GLfloat ambient[]  = { dr * ambScale, dg * ambScale, db * ambScale, 1.0f };
    GLfloat diffuse[]  = { dr, dg, db, 1.0f };
    GLfloat specular[] = { sr, sg, sb, 1.0f };

    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);
    glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

    glColor3f(dr, dg, db);
}

void matYellowPaint()   { setMaterial(0.92f, 0.68f, 0.12f,  0.40f, 0.35f, 0.20f, 32.0f); }
void matDarkChassis()   { setMaterial(0.24f, 0.20f, 0.16f,  0.15f, 0.15f, 0.15f, 16.0f); }
void matRustPanel()     { setMaterial(0.55f, 0.32f, 0.12f,  0.10f, 0.08f, 0.05f, 8.0f);  }
void matChromeMetal()   { setMaterial(0.75f, 0.75f, 0.78f,  0.85f, 0.85f, 0.90f, 96.0f); }
void matGoldMetal()     { setMaterial(0.80f, 0.65f, 0.25f,  0.70f, 0.60f, 0.30f, 64.0f); }
void matRubberTread()   { setMaterial(0.12f, 0.11f, 0.10f,  0.05f, 0.05f, 0.05f, 6.0f);  }
void matGlassLens()     { setMaterial(0.10f, 0.28f, 0.52f,  0.95f, 0.95f, 1.00f, 128.0f); }
void matGreenLeaf()     { setMaterial(0.28f, 0.75f, 0.18f,  0.30f, 0.45f, 0.20f, 28.0f); }
void matEarthGround()   { setMaterial(0.62f, 0.48f, 0.35f,  0.04f, 0.03f, 0.02f, 4.0f);  }
void matScrapMetal(float r, float g, float b) { setMaterial(r, g, b, 0.25f, 0.25f, 0.25f, 20.0f); }

void drawBox(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

void drawCylinderZ(float r, float len)
{
    glutSolidCylinder(r, len, 24, 1);
}

void drawCylinderX(float r, float len)
{
    glPushMatrix();
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, -len * 0.5f);
    drawCylinderZ(r, len);
    glPopMatrix();
}

unsigned int rngSeed = 1234567;
float fastRand()
{
    rngSeed = rngSeed * 1664525u + 1013904223u;
    return ((rngSeed >> 8) & 0xFFFF) / 65535.0f;
}

void drawTread(float side)
{
    glPushMatrix();
    glTranslatef(side * 1.15f, 0.0f, 0.0f);

    matDarkChassis();
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    drawBox(0.62f, 0.8f, 2.0f);
    glPopMatrix();

    matRubberTread();
    for (int e = -1; e <= 1; e += 2)
    {
        glPushMatrix();
        glTranslatef(0.0f, 0.4f, e * 1.0f);
        drawCylinderX(0.4f, 0.62f);
        glPopMatrix();
    }

    for (int i = 0; i < 10; i++)
    {
        glPushMatrix();
        glTranslatef(0.0f, 0.81f, -0.9f + i * 0.2f);
        drawBox(0.66f, 0.05f, 0.1f);
        glPopMatrix();
    }

    float hx = side * 0.34f;
    for (int e = -1; e <= 1; e++)
    {
        glPushMatrix();
        glTranslatef(hx, 0.4f, e * 1.0f);
        matChromeMetal();
        drawCylinderX(0.27f, 0.08f);
        matDarkChassis();
        glTranslatef(side * 0.03f, 0.0f, 0.0f);
        drawCylinderX(0.10f, 0.12f);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawBody()
{
    matYellowPaint();
    glPushMatrix();
    glTranslatef(0.0f, 1.55f, 0.0f);
    drawBox(1.95f, 1.5f, 1.35f);
    glPopMatrix();

    matDarkChassis();
    glPushMatrix();
    glTranslatef(0.0f, 0.87f, 0.0f);
    drawBox(2.0f, 0.14f, 1.4f);
    glPopMatrix();

    matGoldMetal();
    glPushMatrix();
    glTranslatef(0.0f, 2.33f, 0.0f);
    drawBox(1.75f, 0.08f, 1.2f);
    glPopMatrix();

    matRustPanel();
    glPushMatrix();
    glTranslatef(0.0f, 2.39f, -0.3f);
    drawBox(0.7f, 0.06f, 0.4f);
    glPopMatrix();

    matDarkChassis();
    glPushMatrix();
    glTranslatef(0.0f, 1.62f, 0.685f);
    drawBox(1.45f, 0.95f, 0.04f);
    glPopMatrix();

    matRubberTread();
    glPushMatrix();
    glTranslatef(0.0f, 1.62f, 0.70f);
    drawBox(1.25f, 0.75f, 0.04f);
    glPopMatrix();

    matChromeMetal();
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glTranslatef(0.0f, 1.85f + i * 0.08f, 0.73f);
        drawBox(0.95f, 0.03f, 0.02f);
        glPopMatrix();
    }

    const float dialColors[3][3] = {
        {0.90f, 0.15f, 0.10f},
        {0.20f, 0.85f, 0.25f},
        {0.95f, 0.85f, 0.20f}
    };
    for (int i = 0; i < 3; i++)
    {
        setMaterial(dialColors[i][0], dialColors[i][1], dialColors[i][2],
                    0.8f, 0.8f, 0.8f, 48.0f);
        glPushMatrix();
        glTranslatef(-0.35f + i * 0.35f, 1.45f, 0.72f);
        drawCylinderZ(0.075f, 0.05f);
        glPopMatrix();
    }

    matRustPanel();
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.98f, 1.35f, -0.15f);
        drawBox(0.03f, 0.45f, 0.55f);
        glPopMatrix();

        glPushMatrix();
        glTranslatef(s * 0.98f, 2.0f, 0.25f);
        drawBox(0.03f, 0.25f, 0.30f);
        glPopMatrix();
    }

    matChromeMetal();
    glPushMatrix();
    glTranslatef(0.0f, 2.37f, 0.15f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinderZ(0.3f, 0.1f);
    glPopMatrix();
}

const float NECK_LEAN = 12.0f;
const float NECK_LEN  = 0.84f;

void drawEye(float side)
{
    glPushMatrix();
    glTranslatef(side * 0.52f, 0.55f, -0.25f);

    matGoldMetal();
    drawCylinderZ(0.38f, 0.85f);

    matRustPanel();
    drawCylinderZ(0.41f, 0.08f);

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.85f);
    matChromeMetal();
    drawCylinderZ(0.40f, 0.06f);

    glTranslatef(0.0f, 0.0f, 0.06f);
    matDarkChassis();
    drawCylinderZ(0.30f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.93f);
    glScalef(1.0f, 1.0f, 0.35f);
    matGlassLens();
    glutSolidSphere(0.24, 20, 20);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0f, 0.40f, 0.55f);
    glRotatef(-side * pose.brow, 0.0f, 0.0f, 1.0f);
    matGoldMetal();
    drawBox(0.88f, 0.09f, 0.6f);
    glPopMatrix();

    glPopMatrix();
}

void drawNeckAndHead()
{
    glPushMatrix();
    glTranslatef(0.0f, 2.4f, 0.15f);
    glRotatef(NECK_LEAN, 1.0f, 0.0f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);

    matChromeMetal();
    for (int i = 0; i < 6; i++)
    {
        glPushMatrix();
        glTranslatef(0.0f, 0.0f, i * 0.14f);
        drawCylinderZ((i % 2) ? 0.17f : 0.21f, 0.1f);
        glPopMatrix();
    }
    glPopMatrix();

    float rad = NECK_LEAN * PI / 180.0f;
    glPushMatrix();
    glTranslatef(0.0f, 2.4f + NECK_LEN * cosf(rad), 0.15f + NECK_LEN * sinf(rad));
    glRotatef(pose.headYaw, 0.0f, 1.0f, 0.0f);
    glRotatef(pose.headPitch, 1.0f, 0.0f, 0.0f);

    matChromeMetal();
    glPushMatrix();
    glTranslatef(0.0f, 0.08f, 0.0f);
    drawBox(0.55f, 0.2f, 0.4f);
    glPopMatrix();

    matYellowPaint();
    glPushMatrix();
    glTranslatef(0.0f, 0.5f, -0.2f);
    drawBox(1.7f, 0.32f, 0.35f);
    glPopMatrix();

    drawEye(-1.0f);
    drawEye(+1.0f);

    if (headlampEnabled)
    {
        GLfloat lampPos[] = { 0.0f, 0.6f, 0.7f, 1.0f };
        GLfloat lampDir[] = { 0.0f, -0.1f, 1.0f };
        glLightfv(GL_LIGHT2, GL_POSITION, lampPos);
        glLightfv(GL_LIGHT2, GL_SPOT_DIRECTION, lampDir);
    }

    glPopMatrix();
}

void drawArm(float side, float swing)
{
    glPushMatrix();
    glTranslatef(side * 1.0f, 1.85f, 0.2f);

    matGoldMetal();
    glutSolidSphere(0.17, 16, 16);

    glRotatef(side * 18.0f, 0.0f, 1.0f, 0.0f);
    glRotatef(58.0f + swing, 1.0f, 0.0f, 0.0f);

    matChromeMetal();
    drawCylinderZ(0.09f, 0.75f);

    glTranslatef(0.0f, 0.0f, 0.75f);
    matGoldMetal();
    glutSolidSphere(0.12, 16, 16);

    glRotatef(-38.0f, 1.0f, 0.0f, 0.0f);
    matChromeMetal();
    drawCylinderZ(0.08f, 0.7f);

    glTranslatef(0.0f, 0.0f, 0.7f);
    matDarkChassis();
    glutSolidSphere(0.10, 16, 16);

    matChromeMetal();
    glPushMatrix();
    glTranslatef(0.0f, 0.0f, 0.1f);
    drawBox(0.24f, 0.14f, 0.18f);
    glPopMatrix();

    matDarkChassis();
    for (int f = -1; f <= 1; f += 2)
    {
        glPushMatrix();
        glTranslatef(f * 0.07f, 0.0f, 0.18f);
        glRotatef(f * 14.0f, 0.0f, 1.0f, 0.0f);
        glTranslatef(0.0f, 0.0f, 0.15f);
        drawBox(0.06f, 0.08f, 0.32f);
        glPopMatrix();
    }
    glPushMatrix();
    glTranslatef(0.0f, -0.08f, 0.14f);
    glRotatef(-30.0f, 1.0f, 0.0f, 0.0f);
    glTranslatef(0.0f, 0.0f, 0.09f);
    drawBox(0.06f, 0.06f, 0.2f);
    glPopMatrix();

    glPopMatrix();
}

void drawWallE()
{
    glPushMatrix();
    drawTread(-1.0f);
    drawTread(+1.0f);
    drawBody();
    drawArm(-1.0f, pose.armL);
    drawArm(+1.0f, pose.armR);
    drawNeckAndHead();
    glPopMatrix();
}

void drawGround()
{
    matEarthGround();
    glPushMatrix();
    glTranslatef(0.0f, -0.06f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    glutSolidCylinder(20.0, 0.06, 48, 1);
    glPopMatrix();
}

void drawTrashTowers()
{
    rngSeed = 777123;
    const float palette[5][3] = {
        {0.45f, 0.40f, 0.35f},
        {0.60f, 0.36f, 0.18f},
        {0.32f, 0.35f, 0.40f},
        {0.68f, 0.52f, 0.28f},
        {0.28f, 0.26f, 0.24f}
    };

    for (int i = 0; i < 12; i++)
    {
        float ang = (i * 30.0f + fastRand() * 12.0f) * PI / 180.0f;
        float r   = 13.0f + fastRand() * 2.5f;
        float px  = r * sinf(ang), pz = r * cosf(ang);
        int   lv  = 3 + (int)(fastRand() * 4);
        float y   = 0.0f;
        float w   = 2.2f + fastRand() * 0.8f;

        for (int k = 0; k < lv; k++)
        {
            const float* c = palette[(int)(fastRand() * 4.99f)];
            matScrapMetal(c[0], c[1], c[2]);
            float h = 0.7f + fastRand() * 0.5f;

            glPushMatrix();
            glTranslatef(px + (fastRand() - 0.5f) * 0.3f, y + h * 0.5f, pz + (fastRand() - 0.5f) * 0.3f);
            glRotatef(fastRand() * 90.0f, 0.0f, 1.0f, 0.0f);
            drawBox(w, h, w * (0.8f + fastRand() * 0.3f));
            glPopMatrix();

            y += h;
            w *= 0.88f;
        }
    }

    for (int i = 0; i < 16; i++)
    {
        float ang = fastRand() * 2.0f * PI;
        float r   = 3.2f + fastRand() * 4.5f;
        float sz  = 0.25f + fastRand() * 0.4f;
        const float* c = palette[(int)(fastRand() * 4.99f)];
        matScrapMetal(c[0], c[1], c[2]);

        glPushMatrix();
        glTranslatef(r * sinf(ang), sz * 0.45f, r * cosf(ang));
        glRotatef(fastRand() * 360.0f, 0.0f, 1.0f, 0.0f);
        drawBox(sz, sz * (0.6f + fastRand() * 0.5f), sz * (0.7f + fastRand() * 0.5f));
        glPopMatrix();
    }
}

void drawPlantInBoot()
{
    glPushMatrix();
    glTranslatef(2.5f, 0.0f, 1.9f);

    setMaterial(0.18f, 0.25f, 0.40f, 0.35f, 0.35f, 0.45f, 24.0f);
    glPushMatrix();
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinderZ(0.24f, 0.38f);
    glTranslatef(0.0f, 0.0f, 0.36f);
    drawCylinderZ(0.26f, 0.05f);
    glPopMatrix();

    setMaterial(0.28f, 0.18f, 0.08f, 0.05f, 0.05f, 0.05f, 4.0f);
    glPushMatrix();
    glTranslatef(0.0f, 0.405f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinderZ(0.22f, 0.02f);
    glPopMatrix();

    matGreenLeaf();
    glPushMatrix();
    glTranslatef(0.0f, 0.4f, 0.0f);
    glRotatef(-90.0f, 1.0f, 0.0f, 0.0f);
    drawCylinderZ(0.025f, 0.45f);
    glPopMatrix();

    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.13f, 0.82f, 0.0f);
        glRotatef(s * 28.0f, 0.0f, 0.0f, 1.0f);
        glScalef(0.26f, 0.04f, 0.13f);
        glutSolidSphere(1.0, 16, 16);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawSunMarker(float x, float y, float z)
{
    glDisable(GL_LIGHTING);
    glColor3f(1.0f, 0.95f, 0.4f);
    glPushMatrix();
    glTranslatef(x, y, z);
    glutSolidSphere(0.35, 16, 16);
    glPopMatrix();
    glEnable(GL_LIGHTING);
}

void renderBitmapString(float x, float y, const char* str, void* font = GLUT_BITMAP_HELVETICA_12)
{
    glRasterPos2f(x, y);
    while (*str) {
        glutBitmapCharacter(font, *str);
        str++;
    }
}

void drawHUD()
{
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, winW, 0, winH);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.05f, 0.05f, 0.08f, 0.65f);
    glBegin(GL_QUADS);
        glVertex2i(10, winH - 85);
        glVertex2i(winW - 10, winH - 85);
        glVertex2i(winW - 10, winH - 10);
        glVertex2i(10, winH - 10);
    glEnd();

    glColor4f(0.05f, 0.05f, 0.08f, 0.65f);
    glBegin(GL_QUADS);
        glVertex2i(10, 10);
        glVertex2i(winW - 10, 10);
        glVertex2i(winW - 10, 50);
        glVertex2i(10, 50);
    glEnd();
    glDisable(GL_BLEND);

    glColor3f(1.0f, 0.85f, 0.3f);
    renderBitmapString(25, winH - 32, "Lab 10 - Assignment 05 : 3D Scene with Lighting & Shading Models (WALL-E)", GLUT_BITMAP_HELVETICA_18);

    char statusBuf[256];
    if (currentShadeModel == GL_FLAT) {
        glColor3f(1.0f, 0.45f, 0.35f);
        sprintf(statusBuf, "Shading: [FLAT SHADING]  (Key 'G' for Gouraud)  |  Sun Orbit: %s ('A')  |  Fill Light: %s ('L')  |  Headlamp: %s ('H')",
                animateSun ? "ON" : "OFF", fillLightEnabled ? "ON" : "OFF", headlampEnabled ? "ON" : "OFF");
    } else {
        glColor3f(0.35f, 1.0f, 0.45f);
        sprintf(statusBuf, "Shading: [GOURAUD / SMOOTH]  (Key 'F' for Flat)  |  Sun Orbit: %s ('A')  |  Fill Light: %s ('L')  |  Headlamp: %s ('H')",
                animateSun ? "ON" : "OFF", fillLightEnabled ? "ON" : "OFF", headlampEnabled ? "ON" : "OFF");
    }
    renderBitmapString(25, winH - 56, statusBuf, GLUT_BITMAP_HELVETICA_12);

    glColor3f(0.85f, 0.85f, 0.90f);
    renderBitmapString(25, 24, "Controls: [F] Flat  [G] Gouraud  [A] Sun Orbit  [L] Fill Light  [H] Headlamp  [Space] Pose Anim  [Arrows/Mouse] Orbit  [+/-] Zoom  [R] Reset  [ESC] Quit");

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glShadeModel(currentShadeModel);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40.0, (double)winW / winH, 0.5, 100.0);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float azRad = camAz * PI / 180.0f;
    float elRad = camEl * PI / 180.0f;
    gluLookAt(TARGET[0] + camDist * cosf(elRad) * sinf(azRad),
              TARGET[1] + camDist * sinf(elRad),
              TARGET[2] + camDist * cosf(elRad) * cosf(azRad),
              TARGET[0], TARGET[1], TARGET[2],
              0.0f, 1.0f, 0.0f);

    float sunX = TARGET[0] + SUN_DIST * cosf(sunAngle * PI / 180.0f);
    float sunY = SUN_Y;
    float sunZ = TARGET[2] + SUN_DIST * sinf(sunAngle * PI / 180.0f);
    GLfloat sunPos[]  = { sunX, sunY, sunZ, 1.0f };
    GLfloat sunAmb[]  = { 0.22f, 0.20f, 0.18f, 1.0f };
    GLfloat sunDiff[] = { 0.95f, 0.92f, 0.85f, 1.0f };
    GLfloat sunSpec[] = { 1.00f, 0.98f, 0.92f, 1.0f };

    glLightfv(GL_LIGHT0, GL_POSITION, sunPos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sunAmb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sunDiff);
    glLightfv(GL_LIGHT0, GL_SPECULAR, sunSpec);
    glEnable(GL_LIGHT0);

    if (fillLightEnabled)
    {
        GLfloat fillPos[]  = { -sunX * 0.7f, 5.0f, -sunZ * 0.7f, 1.0f };
        GLfloat fillAmb[]  = { 0.05f, 0.08f, 0.12f, 1.0f };
        GLfloat fillDiff[] = { 0.25f, 0.35f, 0.45f, 1.0f };
        GLfloat fillSpec[] = { 0.10f, 0.12f, 0.15f, 1.0f };

        glLightfv(GL_LIGHT1, GL_POSITION, fillPos);
        glLightfv(GL_LIGHT1, GL_AMBIENT, fillAmb);
        glLightfv(GL_LIGHT1, GL_DIFFUSE, fillDiff);
        glLightfv(GL_LIGHT1, GL_SPECULAR, fillSpec);
        glEnable(GL_LIGHT1);
    }
    else
    {
        glDisable(GL_LIGHT1);
    }

    if (headlampEnabled)
    {
        GLfloat lampDiff[] = { 0.9f, 0.95f, 1.0f, 1.0f };
        GLfloat lampSpec[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glLightfv(GL_LIGHT2, GL_DIFFUSE, lampDiff);
        glLightfv(GL_LIGHT2, GL_SPECULAR, lampSpec);
        glLightf(GL_LIGHT2, GL_SPOT_CUTOFF, 38.0f);
        glLightf(GL_LIGHT2, GL_SPOT_EXPONENT, 16.0f);
        glEnable(GL_LIGHT2);
    }
    else
    {
        glDisable(GL_LIGHT2);
    }

    drawSunMarker(sunX, sunY, sunZ);
    drawGround();
    drawTrashTowers();
    drawPlantInBoot();
    drawWallE();

    drawHUD();

    glutSwapBuffers();
}

void init()
{
    glClearColor(0.85f, 0.76f, 0.65f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_NORMALIZE);

    GLfloat globalAmbient[] = { 0.15f, 0.15f, 0.15f, 1.0f };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbient);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    updatePose();
}

void timer(int)
{
    if (animatePose)
    {
        animT += 0.03f;
        updatePose();
    }
    if (animateSun)
    {
        sunAngle += 0.4f;
        if (sunAngle >= 360.0f) sunAngle -= 360.0f;
    }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h)
{
    winW = w;
    winH = (h > 0) ? h : 1;
    glViewport(0, 0, winW, winH);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
    if (key == 'f' || key == 'F') currentShadeModel = GL_FLAT;
    if (key == 'g' || key == 'G') currentShadeModel = GL_SMOOTH;
    if (key == 'a' || key == 'A') animateSun = !animateSun;
    if (key == 'l' || key == 'L') fillLightEnabled = !fillLightEnabled;
    if (key == 'h' || key == 'H') headlampEnabled = !headlampEnabled;
    if (key == ' ') animatePose = !animatePose;
    if (key == 'r' || key == 'R')
    {
        camAz = DEF_AZ;
        camEl = DEF_EL;
        camDist = DEF_DIST;
        sunAngle = 45.0f;
    }
    if (key == '+' || key == '=') camDist -= 0.5f;
    if (key == '-' || key == '_') camDist += 0.5f;
    if (camDist < 3.5f) camDist = 3.5f;
    if (camDist > 16.0f) camDist = 16.0f;

    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)  camAz -= 5.0f;
    if (key == GLUT_KEY_RIGHT) camAz += 5.0f;
    if (key == GLUT_KEY_UP)    camEl += 5.0f;
    if (key == GLUT_KEY_DOWN)  camEl -= 5.0f;

    if (camEl > 85.0f) camEl = 85.0f;
    if (camEl < 2.0f)  camEl = 2.0f;

    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        dragging = (state == GLUT_DOWN);
        lastX = x;
        lastY = y;
    }
    else if (state == GLUT_DOWN && (button == 3 || button == 4))
    {
        camDist += (button == 3) ? -0.5f : 0.5f;
        if (camDist < 3.5f) camDist = 3.5f;
        if (camDist > 16.0f) camDist = 16.0f;
        glutPostRedisplay();
    }
}

void motion(int x, int y)
{
    if (!dragging) return;
    camAz -= (x - lastX) * 0.4f;
    camEl += (y - lastY) * 0.4f;

    if (camEl > 85.0f) camEl = 85.0f;
    if (camEl < 2.0f)  camEl = 2.0f;

    lastX = x;
    lastY = y;
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Lab 10 - Assignment 05 : WALL-E Scene with Lighting & Shading Models");

    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMouseFunc(mouse);
    glutMotionFunc(motion);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
