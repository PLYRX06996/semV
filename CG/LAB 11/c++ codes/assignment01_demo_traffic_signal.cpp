#include <GL/glut.h>
#include <cmath>
#include <string>

enum SignalState {
    ALL_ON = 0,
    RED_ONLY,
    YELLOW_ONLY,
    GREEN_ONLY
};

SignalState currentState = ALL_ON;
bool autoCycle = false;
int cycleTimer = 0;

void drawText(float x, float y, const std::string& str) {
    glRasterPos2f(x, y);
    for (char c : str) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void circle(float cx, float cy, float r) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= 360; i += 5) {
        float rad = i * 3.14159265f / 180.0f;
        glVertex2f(cx + r * cosf(rad), cy + r * sinf(rad));
    }
    glEnd();
}

void drawVisor(float cy, float r) {
    glColor3f(0.15f, 0.15f, 0.15f);
    glLineWidth(2.5f);
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i <= 180; i += 5) {
        float rad = i * 3.14159265f / 180.0f;
        glVertex2f((r + 0.02f) * cosf(rad), cy + (r + 0.02f) * sinf(rad));
    }
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);

    glBegin(GL_QUADS);
        glColor3f(0.80f, 0.88f, 0.95f);
        glVertex2f(-1.0f, 1.0f);
        glVertex2f(1.0f, 1.0f);
        glColor3f(0.92f, 0.95f, 0.98f);
        glVertex2f(1.0f, 0.0f);
        glVertex2f(-1.0f, 0.0f);
    glEnd();

    glColor3f(0.40f, 0.42f, 0.45f);
    glRectf(-1.0f, 0.0f, 1.0f, 0.08f);
    glColor3f(0.95f, 0.95f, 0.95f);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
        glVertex2f(-1.0f, 0.04f); glVertex2f(1.0f, 0.04f);
    glEnd();

    glColor3f(0.35f, 0.38f, 0.40f);
    glRectf(-0.06f, 0.0f, 0.06f, 0.12f);
    glColor3f(0.25f, 0.28f, 0.30f);
    glRectf(-0.14f, 0.02f, 0.14f, 0.06f);

    glColor3f(0.12f, 0.12f, 0.12f);
    glRectf(-0.34f, 0.07f, 0.34f, 0.93f);
    circle(-0.30f, 0.89f, 0.04f);
    circle(0.30f, 0.89f, 0.04f);
    circle(-0.30f, 0.11f, 0.04f);
    circle(0.30f, 0.11f, 0.04f);

    glColor3f(0.02f, 0.02f, 0.02f);
    glRectf(-0.30f, 0.10f, 0.30f, 0.90f);

    glColor3f(0.28f, 0.28f, 0.28f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.30f, 0.10f);
        glVertex2f(0.30f, 0.10f);
        glVertex2f(0.30f, 0.90f);
        glVertex2f(-0.30f, 0.90f);
    glEnd();

    drawVisor(0.72f, 0.15f);
    drawVisor(0.50f, 0.15f);
    drawVisor(0.28f, 0.15f);

    if (currentState == ALL_ON || currentState == RED_ONLY) {
        glColor3f(1.0f, 0.05f, 0.05f);
        circle(0.0f, 0.72f, 0.15f);
        glColor3f(1.0f, 0.55f, 0.55f);
        circle(-0.04f, 0.75f, 0.04f);
    } else {
        glColor3f(0.35f, 0.05f, 0.05f);
        circle(0.0f, 0.72f, 0.15f);
    }

    if (currentState == ALL_ON || currentState == YELLOW_ONLY) {
        glColor3f(1.0f, 0.92f, 0.05f);
        circle(0.0f, 0.50f, 0.15f);
        glColor3f(1.0f, 1.0f, 0.65f);
        circle(-0.04f, 0.53f, 0.04f);
    } else {
        glColor3f(0.32f, 0.26f, 0.04f);
        circle(0.0f, 0.50f, 0.15f);
    }

    if (currentState == ALL_ON || currentState == GREEN_ONLY) {
        glColor3f(0.05f, 0.95f, 0.15f);
        circle(0.0f, 0.28f, 0.15f);
        glColor3f(0.60f, 1.0f, 0.65f);
        circle(-0.04f, 0.31f, 0.04f);
    } else {
        glColor3f(0.04f, 0.26f, 0.08f);
        circle(0.0f, 0.28f, 0.15f);
    }

    glColor3f(0.1f, 0.1f, 0.1f);
    std::string info = "Mode: ";
    if (currentState == ALL_ON) info += "All Lights ON (Slide 3 Demo)";
    else if (currentState == RED_ONLY) info += "RED (Stop)";
    else if (currentState == YELLOW_ONLY) info += "YELLOW (Caution)";
    else if (currentState == GREEN_ONLY) info += "GREEN (Go)";
    drawText(-0.95f, 0.95f, info);
    drawText(-0.95f, 0.91f, "Controls: [1] Red  [2] Yellow  [3] Green  [4] All On  [Space] Auto-cycle");

    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '1': currentState = RED_ONLY; autoCycle = false; break;
        case '2': currentState = YELLOW_ONLY; autoCycle = false; break;
        case '3': currentState = GREEN_ONLY; autoCycle = false; break;
        case '4': currentState = ALL_ON; autoCycle = false; break;
        case ' ':
        case 't':
        case 'T':
            autoCycle = !autoCycle;
            cycleTimer = 0;
            if (autoCycle && currentState == ALL_ON) currentState = RED_ONLY;
            break;
        case 27:
        case 'q':
        case 'Q':
            exit(0);
    }
    glutPostRedisplay();
}

void timer(int value) {
    if (autoCycle) {
        cycleTimer++;
        if (currentState == RED_ONLY && cycleTimer >= 30) {
            currentState = GREEN_ONLY;
            cycleTimer = 0;
        } else if (currentState == GREEN_ONLY && cycleTimer >= 30) {
            currentState = YELLOW_ONLY;
            cycleTimer = 0;
        } else if (currentState == YELLOW_ONLY && cycleTimer >= 12) {
            currentState = RED_ONLY;
            cycleTimer = 0;
        }
        glutPostRedisplay();
    }
    glutTimerFunc(100, timer, 0);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(500, 625);
    glutCreateWindow("Lab 11 Demo Assignment 01 - Traffic Signal");
    gluOrtho2D(-1, 1, 0, 1);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutTimerFunc(100, timer, 0);
    glutMainLoop();
    return 0;
}
