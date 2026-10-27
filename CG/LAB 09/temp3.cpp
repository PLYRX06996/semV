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
    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(1.8f);
    glutWireCube(1.003);
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void drawWCSAxes()
{
    glDisable(GL_LIGHTING);
    glLineWidth(2.0f);
    glBegin(GL_LINES);

    glColor3f(0.9f, 0.1f, 0.1f);
    glVertex3f(-0.5f, 0.0f, 0.0f);
    glVertex3f(3.0f, 0.0f, 0.0f);

    glColor3f(0.1f, 0.8f, 0.1f);
    glVertex3f(0.0f, -0.5f, 0.0f);
    glVertex3f(0.0f, 3.0f, 0.0f);

    glColor3f(0.1f, 0.2f, 0.9f);
    glVertex3f(0.0f, 0.0f, -0.5f);
    glVertex3f(0.0f, 0.0f, 3.0f);
    glEnd();
    glEnable(GL_LIGHTING);
}

void drawLabel(const std::string& text)
{
    glDisable(GL_LIGHTING);
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0, 350, 0, 350);
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(0.1f, 0.1f, 0.1f);
    glRasterPos2i(10, 325);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }

    glPopMatrix();
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glEnable(GL_LIGHTING);
}

void renderScene(int sceneNum)
{
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(3.8, 3.2, 4.8,
              0.8, 0.8, 0.8,
              0.0, 1.0, 0.0);

    GLfloat lightPos[] = { 4.0f, 6.0f, 8.0f, 1.0f };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    drawWCSAxes();

    if (sceneNum != 1) {
        glDisable(GL_LIGHTING);
        glPushMatrix();
        glTranslatef(0.5f, 0.5f, 0.5f);
        glColor3f(0.7f, 0.7f, 0.7f);
        glLineWidth(1.0f);
        glutWireCube(1.0);
        glPopMatrix();
        glEnable(GL_LIGHTING);
    }

    glPushMatrix();
    if (sceneNum == 1) {
        drawUnitCube(0.2f, 0.5f, 0.9f);
        drawLabel("Scene-01: Unit Cube w.r.t WCS");
    }
    else if (sceneNum == 2) {
        GLfloat shX[16] = {
            1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(shX);
        drawUnitCube(0.9f, 0.35f, 0.2f);
        drawLabel("Scene-02: ShX(sh_yx=1, sh_zx=1)(Scene-01)");
    }
    else if (sceneNum == 3) {
        GLfloat shY[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(shY);
        drawUnitCube(0.2f, 0.75f, 0.35f);
        drawLabel("Scene-03: ShY(sh_xy=1, sh_zy=1)(Scene-01)");
    }
    else if (sceneNum == 4) {
        GLfloat shZ[16] = {
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 1.0f
        };
        glMultMatrixf(shZ);
        drawUnitCube(0.75f, 0.25f, 0.85f);
        drawLabel("Scene-04: ShZ(sh_xz=1, sh_yz=1)(Scene-01)");
    }
    glPopMatrix();
}

void display()
{
    int w = glutGet(GLUT_WINDOW_WIDTH);
    int h = glutGet(GLUT_WINDOW_HEIGHT);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (activeScene == 0) {
        glViewport(0, h / 2, w / 2, h / 2);
        renderScene(1);

        glViewport(w / 2, h / 2, w / 2, h / 2);
        renderScene(2);

        glViewport(0, 0, w / 2, h / 2);
        renderScene(3);

        glViewport(w / 2, 0, w / 2, h / 2);
        renderScene(4);
    } else {
        glViewport(0, 0, w, h);
        renderScene(activeScene);
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y)
{
    if (key >= '0' && key <= '4') {
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
    glutCreateWindow("Assignment 03 - 3D Shearing (Keys 0-4)");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
