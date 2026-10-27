#include <GL/freeglut.h>
#include <string>

int activeScene = 0;
void drawUnitCube(float r, float g, float b)
{
    glPushMatrix();
    glTranslatef(0.5f, 0.5f, 0.5f);
    glColor3f(r, g, b);
    glutSolidCube(1.0);

    glDisable(GL_LIGHTING);
    glColor3f(0.1f, 0.1f, 0.1f);
    glLineWidth(1.5f);
    glutWireCube(1.002);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawWCSAxes()
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    glColor3f(0.9f, 0.1f, 0.1f);
    glVertex3f(-2.0f, 0.0f, 0.0f);
    glVertex3f(6.0f, 0.0f, 0.0f);

    glColor3f(0.1f, 0.8f, 0.1f);
    glVertex3f(0.0f, -2.0f, 0.0f);
    glVertex3f(0.0f, 6.0f, 0.0f);

    glColor3f(0.1f, 0.2f, 0.9f);
    glVertex3f(0.0f, 0.0f, -2.0f);
    glVertex3f(0.0f, 0.0f, 6.0f);

    glColor3f(0.6f, 0.0f, 0.8f);
    glVertex3f(1.0f - 3.0f * 1.0f, 2.0f - 3.0f * 2.0f, 3.0f - 3.0f * 3.0f);
    glVertex3f(1.0f + 2.0f * 1.0f, 2.0f + 2.0f * 2.0f, 3.0f + 2.0f * 3.0f);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawHUD(const std::string& text)
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 700, 0, 700);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.1f, 0.1f, 0.1f);
    glRasterPos2i(15, 670);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
    std::string help = "Keys [1-7]: View Scene 01-07   |   Key [0]: Show All";
    glRasterPos2i(15, 20);
    for (char c : help) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void applyScene03Transform()
{
    glScalef(2.0f, 2.0f, 2.0f);
    glTranslatef(1.0f, 1.0f, 1.0f);
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(9.0, 8.0, 12.0,
              2.0, 2.0, 2.0,
              0.0, 1.0, 0.0);

    GLfloat lightPos[] = { 8.0f, 12.0f, 15.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawWCSAxes();

    if (activeScene == 0 || activeScene == 1) {
        glPushMatrix();
        drawUnitCube(0.6f, 0.6f, 0.6f);
        glPopMatrix();
    }

    if (activeScene == 0 || activeScene == 2) {
        glPushMatrix();
        glTranslatef(1.0f, 1.0f, 1.0f);
        drawUnitCube(0.0f, 0.8f, 0.9f);
        glPopMatrix();
    }

    if (activeScene == 0 || activeScene == 3) {
        glPushMatrix();
        applyScene03Transform();
        drawUnitCube(0.2f, 0.4f, 1.0f);
        glPopMatrix();
    }

    if (activeScene == 0 || activeScene == 4) {
        glPushMatrix();
        glRotatef(30.0f, 1.0f, 0.0f, 0.0f);
        applyScene03Transform();
        drawUnitCube(0.95f, 0.35f, 0.2f);
        glPopMatrix();
    }

    if (activeScene == 0 || activeScene == 5) {
        glPushMatrix();
        glRotatef(30.0f, 0.0f, 1.0f, 0.0f);
        applyScene03Transform();
        drawUnitCube(0.2f, 0.8f, 0.3f);
        glPopMatrix();
    }

    if (activeScene == 0 || activeScene == 6) {
        glPushMatrix();
        glRotatef(30.0f, 0.0f, 0.0f, 1.0f);
        applyScene03Transform();
        drawUnitCube(0.95f, 0.8f, 0.1f);
        glPopMatrix();
    }

    if (activeScene == 0 || activeScene == 7) {
        glPushMatrix();
        glTranslatef(1.0f, 2.0f, 3.0f);
        glRotatef(30.0f, 1.0f, 2.0f, 3.0f);
        glTranslatef(-1.0f, -2.0f, -3.0f);
        applyScene03Transform();
        drawUnitCube(0.85f, 0.2f, 0.85f);
        glPopMatrix();
    }

    const char* titles[] = {
        "All Scenes (01 to 07)",
        "Scene-01: Unit Cube w.r.t WCS",
        "Scene-02: T(1, 1, 1)(Scene-01)",
        "Scene-03: S(2, 2, 2)(Scene-02)",
        "Scene-04: Rx(30 deg)(Scene-03)",
        "Scene-05: Ry(30 deg)(Scene-03)",
        "Scene-06: Rz(30 deg)(Scene-03)",
        "Scene-07: Rotate 30 deg about (x-1)/1 = (y-2)/2 = (z-3)/3 (Scene-03)"
    };
    drawHUD(titles[activeScene]);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key >= '0' && key <= '7') {
        activeScene = key - '0';
        glutPostRedisplay();
    }
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };
    GLfloat diffuse[] = { 0.9f, 0.9f, 0.9f, 1.0f };
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(700, 700);
    glutCreateWindow("Assignment 02 - 3D Translation, Scaling & Rotation");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
