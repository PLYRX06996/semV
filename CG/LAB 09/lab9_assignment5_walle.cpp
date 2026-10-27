#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>

#define PI 3.14159265f

int   winW = 1100, winH = 720;
const float DEF_AZ = 35.0f, DEF_EL = 14.0f, DEF_DIST = 10.5f;
float camAz = DEF_AZ, camEl = DEF_EL, camDist = DEF_DIST;
const float TARGET[3] = { 0.0f, 1.9f, 0.0f };

bool dragging = false;
int  lastX = 0, lastY = 0;

bool  animate = true;
float animT = 0.0f;

struct Pose { float headYaw, headPitch, brow, armL, armR; };
Pose pose;

void updatePose()
{
    pose.headYaw   = 22.0f * sinf(0.8f * animT);
    pose.headPitch = 8.0f + 6.0f * sinf(1.3f * animT);
    pose.brow      = 16.0f + 8.0f * sinf(1.6f * animT);
    pose.armL      = 12.0f * sinf(1.2f * animT);
    pose.armR      = -12.0f * sinf(1.2f * animT);
}

void color(float r, float g, float b) { glColor3f(r, g, b); }

void box(float sx, float sy, float sz)
{
    glPushMatrix();
    glScalef(sx, sy, sz);
    glutSolidCube(1.0);
    glPopMatrix();
}

void sphere(float r)                 { glutSolidSphere(r, 24, 24); }
void cylinderZ(float r, float len)   { glutSolidCylinder(r, len, 28, 1); }

void cylinderX(float r, float len)
{
    glPushMatrix();
    glRotatef(90, 0, 1, 0);
    glTranslatef(0, 0, -len / 2);
    cylinderZ(r, len);
    glPopMatrix();
}

unsigned int seed = 1;
float frand()
{
    seed = seed * 1664525u + 1013904223u;
    return ((seed >> 8) & 0xFFFF) / 65535.0f;
}

void drawTread(float side)
{
    glPushMatrix();
    glTranslatef(side * 1.15f, 0, 0);

    color(0.22f, 0.20f, 0.18f);
    glPushMatrix();
    glTranslatef(0, 0.4f, 0);
    box(0.62f, 0.8f, 2.0f);
    glPopMatrix();

    for (int e = -1; e <= 1; e += 2)
    {
        glPushMatrix();
        glTranslatef(0, 0.4f, e * 1.0f);
        cylinderX(0.4f, 0.62f);
        glPopMatrix();
    }

    color(0.10f, 0.09f, 0.08f);
    for (int i = 0; i < 10; i++)
    {
        glPushMatrix();
        glTranslatef(0, 0.81f, -0.9f + i * 0.2f);
        box(0.66f, 0.05f, 0.1f);
        glPopMatrix();
    }

    float hx = side * 0.34f;
    for (int e = -1; e <= 1; e++)
    {
        glPushMatrix();
        glTranslatef(hx, 0.4f, e * 1.0f);
        color(0.45f, 0.42f, 0.36f);
        cylinderX(0.27f, 0.08f);
        color(0.25f, 0.23f, 0.20f);
        glTranslatef(side * 0.03f, 0, 0);
        cylinderX(0.1f, 0.12f);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawBody()
{
    color(0.90f, 0.66f, 0.13f);
    glPushMatrix();
    glTranslatef(0, 1.55f, 0);
    box(1.95f, 1.5f, 1.35f);
    glPopMatrix();

    color(0.40f, 0.31f, 0.12f);
    glPushMatrix();
    glTranslatef(0, 0.87f, 0);
    box(2.0f, 0.14f, 1.4f);
    glPopMatrix();

    color(0.78f, 0.56f, 0.10f);
    glPushMatrix();
    glTranslatef(0, 2.33f, 0);
    box(1.75f, 0.08f, 1.2f);
    glPopMatrix();

    color(0.60f, 0.43f, 0.08f);
    glPushMatrix();
    glTranslatef(0, 2.39f, -0.3f);
    box(0.7f, 0.06f, 0.4f);
    glPopMatrix();

    color(0.30f, 0.30f, 0.30f);
    glPushMatrix();
    glTranslatef(0, 1.62f, 0.685f);
    box(1.45f, 0.95f, 0.04f);
    glPopMatrix();

    color(0.16f, 0.16f, 0.17f);
    glPushMatrix();
    glTranslatef(0, 1.62f, 0.7f);
    box(1.25f, 0.75f, 0.04f);
    glPopMatrix();

    color(0.45f, 0.45f, 0.47f);
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glTranslatef(0, 1.85f + i * 0.08f, 0.73f);
        box(0.95f, 0.03f, 0.02f);
        glPopMatrix();
    }

    const float bc[3][3] = { {0.85f, 0.15f, 0.10f}, {0.20f, 0.75f, 0.25f}, {0.95f, 0.85f, 0.20f} };
    for (int i = 0; i < 3; i++)
    {
        glPushMatrix();
        glTranslatef(-0.35f + i * 0.35f, 1.45f, 0.72f);
        color(bc[i][0], bc[i][1], bc[i][2]);
        cylinderZ(0.075f, 0.05f);
        glPopMatrix();
    }

    color(0.30f, 0.30f, 0.30f);
    glPushMatrix(); glTranslatef(0, 1.6f, -0.685f); box(1.3f, 0.9f, 0.04f); glPopMatrix();
    color(0.45f, 0.45f, 0.47f);
    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(0, 1.3f + i * 0.2f, -0.71f);
        box(1.0f, 0.04f, 0.02f);
        glPopMatrix();
    }

    color(0.55f, 0.30f, 0.10f);
    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix(); glTranslatef(s * 0.98f, 1.35f, -0.15f); box(0.03f, 0.45f, 0.55f); glPopMatrix();
        glPushMatrix(); glTranslatef(s * 0.98f, 2.0f, 0.25f);   box(0.03f, 0.25f, 0.30f); glPopMatrix();
    }
    glPushMatrix(); glTranslatef(0.5f, 1.2f, 0.68f); box(0.4f, 0.22f, 0.03f); glPopMatrix();

    color(0.28f, 0.28f, 0.30f);
    glPushMatrix();
    glTranslatef(0, 2.37f, 0.15f);
    glRotatef(-90, 1, 0, 0);
    cylinderZ(0.3f, 0.1f);
    glPopMatrix();
}

const float NECK_LEAN = 12.0f;
const float NECK_LEN  = 0.84f;

void drawEye(float side)
{
    glPushMatrix();
    glTranslatef(side * 0.52f, 0.55f, -0.25f);

    color(0.80f, 0.70f, 0.40f);
    cylinderZ(0.38f, 0.85f);

    color(0.50f, 0.42f, 0.20f);
    cylinderZ(0.41f, 0.08f);

    glPushMatrix();
    glTranslatef(0, 0, 0.85f);
    color(0.25f, 0.25f, 0.26f);
    cylinderZ(0.40f, 0.06f);
    glTranslatef(0, 0, 0.06f);
    color(0.05f, 0.05f, 0.07f);
    cylinderZ(0.30f, 0.03f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0, 0.93f);
    glScalef(1, 1, 0.35f);
    color(0.12f, 0.33f, 0.55f);
    sphere(0.24f);
    glPopMatrix();

    glDisable(GL_LIGHTING);
    glColor3f(1, 1, 1);
    glPushMatrix();
    glTranslatef(0.08f, 0.09f, 0.99f);
    glutSolidSphere(0.04, 12, 12);
    glPopMatrix();
    glEnable(GL_LIGHTING);

    glPushMatrix();
    glTranslatef(0, 0.40f, 0.55f);
    glRotatef(-side * pose.brow, 0, 0, 1);
    color(0.58f, 0.48f, 0.24f);
    box(0.88f, 0.09f, 0.6f);
    glPopMatrix();

    glPopMatrix();
}

void drawNeckAndHead()
{
    glPushMatrix();
    glTranslatef(0, 2.4f, 0.15f);
    glRotatef(NECK_LEAN, 1, 0, 0);
    glRotatef(-90, 1, 0, 0);
    for (int i = 0; i < 6; i++)
    {
        glPushMatrix();
        glTranslatef(0, 0, i * 0.14f);
        color(0.30f, 0.30f, 0.32f);
        cylinderZ((i % 2) ? 0.17f : 0.21f, 0.1f);
        glPopMatrix();
    }
    glPopMatrix();

    float rad = NECK_LEAN * PI / 180.0f;
    glPushMatrix();
    glTranslatef(0, 2.4f + NECK_LEN * cosf(rad), 0.15f + NECK_LEN * sinf(rad));
    glRotatef(pose.headYaw, 0, 1, 0);
    glRotatef(pose.headPitch, 1, 0, 0);

    color(0.28f, 0.28f, 0.30f);
    glPushMatrix(); glTranslatef(0, 0.08f, 0); box(0.55f, 0.2f, 0.4f); glPopMatrix();

    color(0.55f, 0.45f, 0.20f);
    glPushMatrix(); glTranslatef(0, 0.5f, -0.2f); box(1.7f, 0.32f, 0.35f); glPopMatrix();

    drawEye(-1);
    drawEye(+1);
    glPopMatrix();
}

void drawArm(float side, float swing)
{
    glPushMatrix();
    glTranslatef(side * 1.0f, 1.85f, 0.2f);

    color(0.80f, 0.60f, 0.15f);
    sphere(0.17f);

    glRotatef(side * 18.0f, 0, 1, 0);
    glRotatef(58.0f + swing, 1, 0, 0);

    color(0.32f, 0.32f, 0.35f);
    cylinderZ(0.09f, 0.75f);

    glTranslatef(0, 0, 0.75f);
    color(0.80f, 0.60f, 0.15f);
    sphere(0.12f);

    glRotatef(-38.0f, 1, 0, 0);
    color(0.32f, 0.32f, 0.35f);
    cylinderZ(0.08f, 0.7f);

    glTranslatef(0, 0, 0.7f);
    color(0.22f, 0.22f, 0.24f);
    sphere(0.1f);

    color(0.38f, 0.38f, 0.40f);
    glPushMatrix(); glTranslatef(0, 0, 0.1f); box(0.24f, 0.14f, 0.18f); glPopMatrix();

    for (int f = -1; f <= 1; f += 2)
    {
        glPushMatrix();
        glTranslatef(f * 0.07f, 0, 0.18f);
        glRotatef(f * 14.0f, 0, 1, 0);
        glTranslatef(0, 0, 0.15f);
        box(0.06f, 0.08f, 0.32f);
        glPopMatrix();
    }
    glPushMatrix();
    glTranslatef(0, -0.08f, 0.14f);
    glRotatef(-30, 1, 0, 0);
    glTranslatef(0, 0, 0.09f);
    box(0.06f, 0.06f, 0.2f);
    glPopMatrix();

    glPopMatrix();
}

void drawWallE()
{
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glRotatef(0, 0, 1, 0);
    glScalef(1, 1, 1);

    drawTread(-1);
    drawTread(+1);
    drawBody();
    drawArm(-1, pose.armL);
    drawArm(+1, pose.armR);
    drawNeckAndHead();
    glPopMatrix();
}

void drawGround()
{
    color(0.60f, 0.47f, 0.34f);
    glPushMatrix();
    glTranslatef(0, -0.06f, 0);
    glRotatef(-90, 1, 0, 0);
    glutSolidCylinder(19.0, 0.06, 48, 1);
    glPopMatrix();
}

void drawShadow()
{
    glDisable(GL_LIGHTING);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthMask(GL_FALSE);

    glColor4f(0, 0, 0, 0.28f);
    glPushMatrix();
    glTranslatef(0.05f, 0.005f, 0.05f);
    glRotatef(-90, 1, 0, 0);
    glScalef(2.1f, 1.9f, 1);
    glutSolidCylinder(1.0, 0.002, 36, 1);
    glPopMatrix();

    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);
    glEnable(GL_LIGHTING);
}

const float junkPalette[5][3] = {
    {0.45f, 0.42f, 0.40f}, {0.62f, 0.38f, 0.20f}, {0.35f, 0.38f, 0.42f},
    {0.70f, 0.55f, 0.30f}, {0.30f, 0.28f, 0.25f}
};

void drawTrash()
{
    seed = 12345;

    for (int i = 0; i < 12; i++)
    {
        float ang = (i * 30.0f + frand() * 14.0f) * PI / 180.0f;
        float r   = 13.0f + frand() * 2.5f;
        float px  = r * sinf(ang), pz = r * cosf(ang);
        int   lv  = 3 + (int)(frand() * 4);
        float y   = 0;
        float w   = 2.2f + frand() * 0.8f;

        for (int k = 0; k < lv; k++)
        {
            const float* c = junkPalette[(int)(frand() * 4.99f)];
            float h = 0.7f + frand() * 0.5f;
            glPushMatrix();
            glTranslatef(px + (frand() - 0.5f) * 0.3f, y + h / 2, pz + (frand() - 0.5f) * 0.3f);
            glRotatef(frand() * 90.0f, 0, 1, 0);
            color(c[0], c[1], c[2]);
            box(w, h, w * (0.8f + frand() * 0.3f));
            glPopMatrix();
            y += h;
            w *= 0.88f;
        }
    }

    for (int i = 0; i < 18; i++)
    {
        float ang = frand() * 2 * PI;
        float r   = 3.2f + frand() * 5.0f;
        float sz  = 0.25f + frand() * 0.45f;
        const float* c = junkPalette[(int)(frand() * 4.99f)];

        glPushMatrix();
        glTranslatef(r * sinf(ang), sz * 0.45f, r * cosf(ang));
        glRotatef(frand() * 360.0f, 0, 1, 0);
        glRotatef((frand() - 0.5f) * 40.0f, 1, 0, 0);
        color(c[0], c[1], c[2]);
        box(sz, sz * (0.6f + frand() * 0.5f), sz * (0.7f + frand() * 0.5f));
        glPopMatrix();
    }
}

void drawPlant()
{
    glPushMatrix();
    glTranslatef(2.5f, 0, 1.9f);

    glPushMatrix();
    glRotatef(-90, 1, 0, 0);
    color(0.20f, 0.28f, 0.45f);
    cylinderZ(0.24f, 0.38f);
    color(0.08f, 0.10f, 0.18f);
    glTranslatef(0, 0, 0.36f);
    cylinderZ(0.26f, 0.05f);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0, 0.405f, 0);
    glRotatef(-90, 1, 0, 0);
    color(0.30f, 0.20f, 0.10f);
    cylinderZ(0.22f, 0.02f);
    glPopMatrix();

    color(0.20f, 0.55f, 0.15f);
    glPushMatrix();
    glTranslatef(0, 0.4f, 0);
    glRotatef(-90, 1, 0, 0);
    cylinderZ(0.025f, 0.45f);
    glPopMatrix();

    for (int s = -1; s <= 1; s += 2)
    {
        glPushMatrix();
        glTranslatef(s * 0.13f, 0.82f, 0);
        glRotatef(s * 28.0f, 0, 0, 1);
        glScalef(0.26f, 0.04f, 0.13f);
        color(0.35f, 0.75f, 0.20f);
        sphere(1.0f);
        glPopMatrix();
    }
    glPopMatrix();
}

void drawText(float nx, float ny, const char* s, void* font, bool centred)
{
    float w = glutBitmapLength(font, (const unsigned char*)s) / (float)winW;
    glRasterPos2f(centred ? nx - w / 2 : nx, ny);
    for (; *s; s++) glutBitmapCharacter(font, *s);
}

void drawOverlay()
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

    glColor3f(0.15f, 0.10f, 0.05f);
    drawText(0.5f, 0.945f, "WALL-E  -  Lab 09, Assignment 05 : 3D scene", GLUT_BITMAP_HELVETICA_18, true);
    glColor3f(0.25f, 0.18f, 0.10f);
    drawText(0.5f, 0.03f,
             "Drag mouse / arrow keys: orbit    +/- : zoom    SPACE: pause animation    R: reset view    ESC: quit",
             GLUT_BITMAP_HELVETICA_12, true);

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

    glViewport(0, 0, winW, winH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(40, (double)winW / winH, 0.5, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float az = camAz * PI / 180, el = camEl * PI / 180;
    gluLookAt(TARGET[0] + camDist * cosf(el) * sinf(az),
              TARGET[1] + camDist * sinf(el),
              TARGET[2] + camDist * cosf(el) * cosf(az),
              TARGET[0], TARGET[1], TARGET[2],
              0, 1, 0);

    GLfloat lightPos[] = { -6, 12, 10, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawGround();
    drawShadow();
    drawTrash();
    drawPlant();
    drawWallE();
    drawOverlay();

    glutSwapBuffers();
}

void init()
{
    glClearColor(0.95f, 0.82f, 0.62f, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat ambient[] = { 0.35f, 0.35f, 0.35f, 1 };
    GLfloat diffuse[] = { 1, 1, 1, 1 };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    updatePose();
    glMatrixMode(GL_MODELVIEW);
}

void reshape(int w, int h)
{
    winW = w;
    winH = (h > 0) ? h : 1;
}

void timer(int)
{
    if (animate) { animT += 0.03f; updatePose(); }
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0);
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
    if (key == ' ') animate = !animate;
    if (key == 'r' || key == 'R') { camAz = DEF_AZ; camEl = DEF_EL; camDist = DEF_DIST; }
    if (key == '+' || key == '=') camDist -= 0.5f;
    if (key == '-' || key == '_') camDist += 0.5f;
    if (camDist < 4)  camDist = 4;
    if (camDist > 12) camDist = 12;
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)  camAz -= 5;
    if (key == GLUT_KEY_RIGHT) camAz += 5;
    if (key == GLUT_KEY_UP)    camEl += 5;
    if (key == GLUT_KEY_DOWN)  camEl -= 5;
    if (camEl > 85) camEl = 85;
    if (camEl < 2)  camEl = 2;
    glutPostRedisplay();
}

void mouse(int button, int state, int x, int y)
{
    if (button == GLUT_LEFT_BUTTON)
    {
        dragging = (state == GLUT_DOWN);
        lastX = x; lastY = y;
    }
    else if (state == GLUT_DOWN && (button == 3 || button == 4))
    {
        camDist += (button == 3) ? -0.5f : 0.5f;
        if (camDist < 4)  camDist = 4;
        if (camDist > 12) camDist = 12;
        glutPostRedisplay();
    }
}

void motion(int x, int y)
{
    if (!dragging) return;
    camAz -= (x - lastX) * 0.4f;
    camEl += (y - lastY) * 0.4f;
    if (camEl > 85) camEl = 85;
    if (camEl < 2)  camEl = 2;
    lastX = x; lastY = y;
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("WALL-E - 3D Scene");
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
