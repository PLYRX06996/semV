#include <GL/freeglut.h>
#include <cmath>
#include <string>

int viewMode = 0;

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

void applyPlaneReflection()
{
    const float RAD2DEG = 180.0f / M_PI;
    float thetaX = std::atan2(2.0f, 3.0f) * RAD2DEG;
    float thetaY = std::atan2(1.0f, std::sqrt(13.0f)) * RAD2DEG;

    glTranslatef(-1.0f, 0.0f, 0.0f);
    glRotatef(-thetaX, 1.0f, 0.0f, 0.0f);
    glRotatef(thetaY, 0.0f, 1.0f, 0.0f);
    glScalef(1.0f, 1.0f, -1.0f);
    glRotatef(-thetaY, 0.0f, 1.0f, 0.0f);
    glRotatef(thetaX, 1.0f, 0.0f, 0.0f);
    glTranslatef(1.0f, 0.0f, 0.0f);
}

void drawReflectionPlane()
{
    glDisable(GL_LIGHTING);
    glColor3f(0.55f, 0.55f, 0.55f);
    glLineWidth(1.0f);

    for (float x = -4.0f; x <= 4.0f; x += 1.0f) {
        glBegin(GL_LINE_STRIP);
        for (float y = -4.0f; y <= 4.0f; y += 1.0f) {
            float z = (-1.0f - x - 2.0f * y) / 3.0f;
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    for (float y = -4.0f; y <= 4.0f; y += 1.0f) {
        glBegin(GL_LINE_STRIP);
        for (float x = -4.0f; x <= 4.0f; x += 1.0f) {
            float z = (-1.0f - x - 2.0f * y) / 3.0f;
            glVertex3f(x, y, z);
        }
        glEnd();
    }
    glEnable(GL_LIGHTING);
}

void drawWCSAxes()
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    glColor3f(0.9f, 0.1f, 0.1f);
    glVertex3f(-5.0f, 0.0f, 0.0f);
    glVertex3f(5.0f, 0.0f, 0.0f);

    glColor3f(0.1f, 0.8f, 0.1f);
    glVertex3f(0.0f, -5.0f, 0.0f);
    glVertex3f(0.0f, 5.0f, 0.0f);

    glColor3f(0.1f, 0.2f, 0.9f);
    glVertex3f(0.0f, 0.0f, -5.0f);
    glVertex3f(0.0f, 0.0f, 5.0f);
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
    std::string help = "Key [0]: Show All | Key [1]: Reflect Scene-01 | Key [2]: Reflect Scene-03";
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

void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(9.0, -9.0, 9.0,
              0.0,  0.0, 0.0,
              0.0,  0.0, 1.0);

    GLfloat lightPos[] = { 8.0f, -8.0f, 12.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawWCSAxes();
    drawReflectionPlane();

    if (viewMode == 0 || viewMode == 1) {
        glPushMatrix();
        drawUnitCube(0.2f, 0.5f, 1.0f);
        glPopMatrix();

        glPushMatrix();
        applyPlaneReflection();
        drawUnitCube(1.0f, 0.5f, 0.1f);
        glPopMatrix();
    }

    if (viewMode == 0 || viewMode == 2) {

        glPushMatrix();
        glScalef(2.0f, 2.0f, 2.0f);
        glTranslatef(1.0f, 1.0f, 1.0f);
        drawUnitCube(0.1f, 0.8f, 0.7f);
        glPopMatrix();

        glPushMatrix();
        applyPlaneReflection();
        glScalef(2.0f, 2.0f, 2.0f);
        glTranslatef(1.0f, 1.0f, 1.0f);
        drawUnitCube(0.9f, 0.2f, 0.4f);
        glPopMatrix();
    }

    const char* titles[] = {
        "Plane x+2y+3z+1=0: Scene-01 (Blue->Orange) & Scene-03 (Cyan->Crimson)",
        "Scene-01 (Blue) & Reflection about x+2y+3z+1=0 (Orange)",
        "Scene-03 (Cyan) & Scene-02: Reflection of Scene-03 (Crimson)"
    };
    drawHUD(titles[viewMode]);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key >= '0' && key <= '2') {
        viewMode = key - '0';
        glutPostRedisplay();
    }
}

void init()
{
    glClearColor(1, 1, 1, 1);
    glEnable(GL_DEPTH_TEST);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_NORMALIZE);

    GLfloat ambient[] = { 0.35f, 0.35f, 0.35f, 1.0f };
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
    glutCreateWindow("Assignment 04 - 3D Reflection about x + 2y + 3z + 1 = 0");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
