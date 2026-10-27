#include <GL/freeglut.h>
#include <vector>
#include <string>
#include <cmath>

struct Point2D {
    float x, y;
};

const std::vector<Point2D> butterfly_pts = {
    {5.0f, 6.5f}, {4.0f, 8.5f}, {2.0f, 9.0f}, {1.0f, 7.5f},
    {2.5f, 5.5f}, {1.5f, 3.5f}, {2.0f, 1.5f}, {4.0f, 2.0f},
    {5.0f, 3.5f},
    {6.0f, 2.0f}, {8.0f, 1.5f}, {8.5f, 3.5f}, {7.5f, 5.5f},
    {9.0f, 7.5f}, {8.0f, 9.0f}, {6.0f, 8.5f}
};

const std::vector<Point2D> mouse_pts = {
    {1.0f, 5.0f}, {2.0f, 6.5f}, {3.0f, 7.8f}, {3.8f, 6.8f},
    {5.5f, 7.2f}, {7.5f, 6.0f}, {8.5f, 4.0f}, {9.0f, 2.0f},
    {7.5f, 1.5f}, {5.5f, 2.0f}, {4.0f, 1.5f}, {2.5f, 2.2f},
    {1.5f, 3.5f}, {0.8f, 4.5f}
};

const std::vector<Point2D> elephant_pts = {
    {2.0f, 4.0f}, {1.2f, 5.5f}, {1.5f, 7.0f}, {2.5f, 7.2f},
    {3.5f, 8.5f}, {5.5f, 8.5f}, {7.5f, 8.0f}, {8.5f, 6.5f},
    {8.8f, 4.0f}, {8.0f, 2.0f}, {7.0f, 2.0f}, {6.5f, 3.5f},
    {5.0f, 3.5f}, {4.5f, 2.0f}, {3.5f, 2.0f}, {3.0f, 3.5f}
};

int currentObject = 1;
bool useRefinedPoints = false;
bool showPolygon = true;

std::vector<Point2D> getActivePoints() {
    std::vector<Point2D> base;
    if (currentObject == 1) base = butterfly_pts;
    else if (currentObject == 2) base = mouse_pts;
    else base = elephant_pts;

    if (!useRefinedPoints) return base;

    std::vector<Point2D> refined;
    int n = (int)base.size();
    for (int i = 0; i < n; i++) {
        Point2D p0 = base[(i - 1 + n) % n];
        Point2D p1 = base[i];
        Point2D p2 = base[(i + 1) % n];
        Point2D p3 = base[(i + 2) % n];

        float x1 = (p0.x + 6.0f * p1.x + p2.x) / 8.0f;
        float y1 = (p0.y + 6.0f * p1.y + p2.y) / 8.0f;
        refined.push_back({x1, y1});

        float x2 = (4.0f * p1.x + 4.0f * p2.x) / 8.0f;
        float y2 = (4.0f * p1.y + 4.0f * p2.y) / 8.0f;
        refined.push_back({x2, y2});
    }
    return refined;
}

Point2D getPoint(const std::vector<Point2D>& pts, int i) {
    int n = (int)pts.size();
    return pts[(i % n + n) % n];
}

void renderText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void display()
{
    glClear(GL_COLOR_BUFFER_BIT);

    std::vector<Point2D> pts = getActivePoints();
    int n = (int)pts.size();

    std::string objNames[4] = {"", "Butterfly", "Mouse / Rat", "Cartoon Elephant"};
    glColor3f(0.0f, 0.0f, 0.0f);
    std::string title = "Object: " + objNames[currentObject] +
                        "  |  Points: N = " + std::to_string(n) +
                        (useRefinedPoints ? " (Refined 2N - Q06)" : " (Original N - Q05)");
    renderText(0.4f, 9.4f, title);

    glColor3f(0.3f, 0.3f, 0.3f);
    renderText(0.4f, 9.0f, "Keys [1]: Butterfly  [2]: Mouse  [3]: Elephant  |  [N/Space]: Toggle N / 2N  |  [P]: Toggle Polygon");

    if (showPolygon) {
        glColor3f(0.75f, 0.75f, 0.75f);
        glLineWidth(1.2f);
        glBegin(GL_LINE_LOOP);
        for (const auto& p : pts) {
            glVertex2f(p.x, p.y);
        }
        glEnd();
    }

    glColor3f(0.85f, 0.0f, 0.0f);
    glPointSize(useRefinedPoints ? 6.0f : 9.0f);
    glBegin(GL_POINTS);
    for (const auto& p : pts) {
        glVertex2f(p.x, p.y);
    }
    glEnd();

    glColor3f(0.0f, 0.25f, 0.85f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_LOOP);
    const int SAMPLES_PER_SEGMENT = 60;

    for (int i = 0; i < n; i++) {
        Point2D p0 = getPoint(pts, i - 1);
        Point2D p1 = getPoint(pts, i);
        Point2D p2 = getPoint(pts, i + 1);
        Point2D p3 = getPoint(pts, i + 2);

        for (int j = 0; j < SAMPLES_PER_SEGMENT; j++) {
            float t = (float)j / SAMPLES_PER_SEGMENT;
            float t2 = t * t;
            float t3 = t2 * t;

            float b0 = (1.0f / 6.0f) * (-t3 + 3.0f * t2 - 3.0f * t + 1.0f);
            float b1 = (1.0f / 6.0f) * (3.0f * t3 - 6.0f * t2 + 4.0f);
            float b2 = (1.0f / 6.0f) * (-3.0f * t3 + 3.0f * t2 + 3.0f * t + 1.0f);
            float b3 = (1.0f / 6.0f) * t3;

            float x = b0 * p0.x + b1 * p1.x + b2 * p2.x + b3 * p3.x;
            float y = b0 * p0.y + b1 * p1.y + b2 * p2.y + b3 * p3.y;

            glVertex2f(x, y);
        }
    }
    glEnd();

    glFlush();
}

void keyboard(unsigned char key, int x, int y) {
    if (key == '1') currentObject = 1;
    if (key == '2') currentObject = 2;
    if (key == '3') currentObject = 3;
    if (key == 'n' || key == 'N' || key == ' ') useRefinedPoints = !useRefinedPoints;
    if (key == 'p' || key == 'P') showPolygon = !showPolygon;
    glutPostRedisplay();
}

void init()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0, 10, 0, 10, -1, 1);
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(650, 650);
    glutCreateWindow("Assignment 05 & 06 - 3rd Order Spline Representation (Keys 1, 2, 3, N, P)");
    init();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}
