#include <GL/freeglut.h>
#include <bits/stdc++.h>
using namespace std;

struct Point3D {
    float x, y, z;
};

vector<Point3D> originalControlPoints = {
    {3.0f, 1.5f, 0.0f},
    {4.5f, 1.0f, 0.0f},
    {6.5f, 1.2f, 0.0f},
    {8.0f, 2.5f, 0.0f},
    {8.8f, 4.0f, 0.0f},
    {8.2f, 5.0f, 0.0f},
    {7.0f, 4.5f, 0.0f},
    {5.5f, 3.5f, 0.0f},
    {4.0f, 2.5f, 0.0f},
    {3.0f, 2.0f, 0.0f}
};

vector<Point3D> controlPoints;
int displayMode = 3;

Point3D getPoint(int i) {
    int n = controlPoints.size();
    return controlPoints[(i % n + n) % n];
}

void renderText(float x, float y, const string& text) {
    glColor3f(0.0f, 0.0f, 0.0f);
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, c);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    string modeText = "1000 Pts - Order: " + to_string(displayMode) + " (Press 1, 2, 3)";
    renderText(0.5f, 9.2f, modeText);

    glColor3f(0.0f, 0.0f, 1.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    int n = controlPoints.size();
    
for (int i = 0; i < n; i++) {
        for (int j = 0; j <= 2; j++) {
            float t = j / 2.0f;
            float x = 0, y = 0;
            
            if (displayMode == 1) {
                Point3D p0 = getPoint(i);
                Point3D p1 = getPoint(i + 1);
                x = (1 - t) * p0.x + t * p1.x;
                y = (1 - t) * p0.y + t * p1.y;
            }
            else if (displayMode == 2) {
                Point3D p0 = getPoint(i - 1);
                Point3D p1 = getPoint(i);
                Point3D p2 = getPoint(i + 1);
                
                float b0 = 0.5f * (1 - t) * (1 - t);
                float b1 = 0.5f * (-2 * t * t + 2 * t + 1);
                float b2 = 0.5f * t * t;
                
                x = b0 * p0.x + b1 * p1.x + b2 * p2.x;
                y = b0 * p0.y + b1 * p1.y + b2 * p2.y;
            }
            else if (displayMode == 3) {
                Point3D p0 = getPoint(i - 1);
                Point3D p1 = getPoint(i);
                Point3D p2 = getPoint(i + 1);
                Point3D p3 = getPoint(i + 2);
                
                float b0 = (1.0f / 6.0f) * (1 - t) * (1 - t) * (1 - t);
                float b1 = (1.0f / 6.0f) * (3 * t * t * t - 6 * t * t + 4);
                float b2 = (1.0f / 6.0f) * (-3 * t * t * t + 3 * t * t + 3 * t + 1);
                float b3 = (1.0f / 6.0f) * t * t * t;
                
                x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
                y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
            }
            glVertex3f(x, y, 0.0f);
        }
    }
    glEnd();
    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '1') displayMode = 1;
    if (key == '2') displayMode = 2;
    if (key == '3') displayMode = 3;
    glutPostRedisplay();
}

void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 10, 0, 10, -1, 1);
    
    int nOrig = originalControlPoints.size();
    for (int i = 0; i < 1000; i++) {
        float global_t = (float)i / 1000.0f * nOrig;
        int seg = (int)global_t;
        float t = global_t - seg;
        
        Point3D p0 = originalControlPoints[(seg - 1 + nOrig) % nOrig];
        Point3D p1 = originalControlPoints[seg % nOrig];
        Point3D p2 = originalControlPoints[(seg + 1) % nOrig];
        Point3D p3 = originalControlPoints[(seg + 2) % nOrig];
        
        float b0 = (1.0f / 6.0f) * (1 - t) * (1 - t) * (1 - t);
        float b1 = (1.0f / 6.0f) * (3 * t * t * t - 6 * t * t + 4);
        float b2 = (1.0f / 6.0f) * (-3 * t * t * t + 3 * t * t + 3 * t + 1);
        float b3 = (1.0f / 6.0f) * t * t * t;
        
        float x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
        float y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;
        
        controlPoints.push_back({x, y, 0.0f});
    }
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Assignment 4 - Mouse with 1000 Points");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
