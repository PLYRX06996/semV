#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <string>
#include <functional>
#include <algorithm>

#define PI 3.14159265358979323846f

const int WIN_W = 735;
const int WIN_H = 743;

bool wireframeMode = false;
bool smoothShading = false;
bool animated = false;
float animTime = 0.0f;
float zoom = 1.0f;
float panX = 0.0f, panY = 0.0f;

struct Vec2 {
    float x, y;
};

struct Color {
    float r, g, b;
};

struct Vertex {
    float x, y;
    float r, g, b;
};

struct Triangle {
    Vertex v[3];
    Color flatColor;
};

std::vector<Triangle> camelMesh;
std::vector<float> posBuffer;
std::vector<float> flatColBuffer;
std::vector<float> smoothColBuffer;
std::vector<float> wireColBuffer;

const Color C_DARKEST  = {0.23f, 0.06f, 0.04f};
const Color C_DARK     = {0.34f, 0.13f, 0.06f};
const Color C_MID_DARK = {0.45f, 0.18f, 0.06f};
const Color C_MID      = {0.56f, 0.24f, 0.09f};
const Color C_ORANGE   = {0.70f, 0.35f, 0.14f};
const Color C_GOLD     = {0.80f, 0.49f, 0.18f};
const Color C_LIGHT    = {0.86f, 0.58f, 0.24f};
const Color C_WARM_TAN = {0.89f, 0.66f, 0.31f};
const Color C_PALE_GLD = {0.91f, 0.73f, 0.42f};
const Color C_SOFT_CRM = {0.91f, 0.81f, 0.59f};
const Color C_CREAM    = {0.93f, 0.85f, 0.67f};
const Color C_SAND_TOP = {0.75f, 0.62f, 0.49f};
const Color C_SAND_BOT = {0.61f, 0.46f, 0.31f};

Color lerpColor(const Color& a, const Color& b, float t) {
    t = std::max(0.0f, std::min(1.0f, t));
    return {
        a.r + (b.r - a.r) * t,
        a.g + (b.g - a.g) * t,
        a.b + (b.b - a.b) * t
    };
}

Vec2 catmullRom(const Vec2& p0, const Vec2& p1, const Vec2& p2, const Vec2& p3, float t) {
    float t2 = t * t;
    float t3 = t2 * t;
    float x = 0.5f * ((2.0f * p1.x) +
                      (-p0.x + p2.x) * t +
                      (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                      (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
    float y = 0.5f * ((2.0f * p1.y) +
                      (-p0.y + p2.y) * t +
                      (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                      (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
    return {x, y};
}

std::vector<Vec2> evaluateSpline(const std::vector<Vec2>& ctrlPts, int numSamples) {
    std::vector<Vec2> result;
    result.reserve(numSamples);
    int n = static_cast<int>(ctrlPts.size());
    for (int i = 0; i < numSamples; ++i) {
        float u = (static_cast<float>(i) / static_cast<float>(numSamples - 1)) * (n - 1);
        int seg = std::min(static_cast<int>(u), n - 2);
        float t = u - seg;
        const Vec2& p0 = ctrlPts[std::max(0, seg - 1)];
        const Vec2& p1 = ctrlPts[seg];
        const Vec2& p2 = ctrlPts[seg + 1];
        const Vec2& p3 = ctrlPts[std::min(n - 1, seg + 2)];
        result.push_back(catmullRom(p0, p1, p2, p3, t));
    }
    return result;
}

using ColorFunc = std::function<Color(float u, float v, float x, float y)>;

void addSplineRibbonPatch(const std::vector<Vec2>& curveA,
                          const std::vector<Vec2>& curveB,
                          int numU, int numV,
                          ColorFunc colorFn) {
    std::vector<Vec2> ptsA = evaluateSpline(curveA, numU);
    std::vector<Vec2> ptsB = evaluateSpline(curveB, numU);

    auto evalGridPoint = [&](int i, int j) -> Vertex {
        float u = static_cast<float>(i) / static_cast<float>(numU - 1);
        float v = static_cast<float>(j) / static_cast<float>(numV);
        float x = (1.0f - v) * ptsA[i].x + v * ptsB[i].x;
        float y = (1.0f - v) * ptsA[i].y + v * ptsB[i].y;
        Color c = colorFn(u, v, x, y);
        return {x, y, c.r, c.g, c.b};
    };

    for (int i = 0; i < numU - 1; ++i) {
        for (int j = 0; j < numV; ++j) {
            Vertex p00 = evalGridPoint(i, j);
            Vertex p10 = evalGridPoint(i + 1, j);
            Vertex p01 = evalGridPoint(i, j + 1);
            Vertex p11 = evalGridPoint(i + 1, j + 1);

            float uMid = (i + 0.5f) / static_cast<float>(numU - 1);
            float vMid = (j + 0.5f) / static_cast<float>(numV);

            Vertex tris[2][3];
            if ((i + j) % 2 == 0) {
                tris[0][0] = p00; tris[0][1] = p10; tris[0][2] = p11;
                tris[1][0] = p00; tris[1][1] = p11; tris[1][2] = p01;
            } else {
                tris[0][0] = p00; tris[0][1] = p10; tris[0][2] = p01;
                tris[1][0] = p10; tris[1][1] = p11; tris[1][2] = p01;
            }

            for (int k = 0; k < 2; ++k) {
                float cx = (tris[k][0].x + tris[k][1].x + tris[k][2].x) / 3.0f;
                float cy = (tris[k][0].y + tris[k][1].y + tris[k][2].y) / 3.0f;
                Color fc = colorFn(uMid, vMid, cx, cy);
                camelMesh.push_back({{tris[k][0], tris[k][1], tris[k][2]}, fc});
            }
        }
    }
}

void buildProceduralCamel() {
    camelMesh.clear();

    addSplineRibbonPatch(
        {{168, 211}, {220, 214}, {285, 215}, {350, 215}, {415, 214}, {446, 210}},
        {{168, 209}, {220, 204}, {285, 201}, {350, 201}, {415, 204}, {446, 208}},
        28, 3,
        [](float u, float v, float x, float y) {
            return (v < 0.45f) ? C_SAND_TOP : C_SAND_BOT;
        }
    );

    addSplineRibbonPatch(
        {{248, 410}, {235, 365}, {220, 322}, {227, 298}, {244, 255}, {251, 226}, {253, 213}},
        {{282, 405}, {262, 372}, {248, 338}, {247, 302}, {257, 258}, {269, 228}, {296, 213}},
        22, 4,
        [](float u, float v, float x, float y) {
            if (y > 348.0f || y < 226.0f) return C_DARKEST;
            if (y > 325.0f || y < 248.0f || v > 0.72f) return C_DARK;
            return C_MID;
        }
    );

    addSplineRibbonPatch(
        {{364, 405}, {368, 355}, {367, 305}, {362, 265}, {352, 232}, {349, 213}},
        {{392, 405}, {389, 360}, {380, 305}, {370, 265}, {369, 228}, {390, 213}},
        22, 4,
        [](float u, float v, float x, float y) {
            return (y > 358.0f) ? C_DARKEST : C_DARK;
        }
    );

    addSplineRibbonPatch(
        {{216, 492}, {206, 470}, {203, 450}, {203, 435}},
        {{224, 486}, {214, 466}, {212, 448}, {208, 436}},
        12, 2,
        [](float u, float v, float x, float y) {
            return (y < 445.0f || v < 0.4f) ? C_DARK : C_GOLD;
        }
    );

    addSplineRibbonPatch(
        {{250, 492}, {290, 465}, {340, 450}, {390, 460}, {435, 445}, {475, 422}, {510, 440}, {525, 495}, {542, 520}},
        {{255, 385}, {295, 402}, {345, 392}, {390, 385}, {435, 395}, {482, 385}, {525, 412}, {537, 450}, {555, 486}},
        28, 5,
        [](float u, float v, float x, float y) {
            return C_DARKEST;
        }
    );

    addSplineRibbonPatch(
        {{205, 450}, {215, 488}, {242, 505}, {295, 528}, {328, 552}, {348, 558}, {366, 552}, {392, 526}, {422, 507}, {436, 492}},
        {{214, 448}, {228, 482}, {255, 496}, {302, 518}, {332, 542}, {348, 546}, {362, 540}, {384, 516}, {412, 498}, {424, 488}},
        28, 3,
        [](float u, float v, float x, float y) {
            if (x > 388.0f) return C_DARK;
            if (x > 362.0f) return C_MID_DARK;
            return C_GOLD;
        }
    );

    addSplineRibbonPatch(
        {{246, 498}, {280, 512}, {315, 532}, {336, 546}, {352, 546}, {368, 534}, {390, 512}, {416, 497}},
        {{246, 496}, {275, 474}, {308, 456}, {336, 450}, {358, 450}, {376, 460}, {396, 476}, {416, 495}},
        26, 7,
        [](float u, float v, float x, float y) {
            if (y >= 482.0f) return C_CREAM;
            if (y >= 464.0f) return C_SOFT_CRM;
            return C_PALE_GLD;
        }
    );

    addSplineRibbonPatch(
        {{266, 476}, {292, 454}, {320, 440}, {350, 438}, {375, 446}},
        {{270, 458}, {296, 438}, {324, 428}, {352, 428}, {376, 436}},
        18, 3,
        [](float u, float v, float x, float y) {
            return (v < 0.35f) ? C_MID : C_ORANGE;
        }
    );
    addSplineRibbonPatch(
        {{272, 452}, {298, 432}, {326, 422}, {354, 422}, {376, 430}},
        {{278, 418}, {304, 410}, {332, 408}, {358, 410}, {376, 418}},
        18, 3,
        [](float u, float v, float x, float y) {
            return (v < 0.7f) ? C_ORANGE : C_MID;
        }
    );

    addSplineRibbonPatch(
        {{246, 498}, {226, 488}, {213, 462}, {214, 430}, {215, 395}, {204, 365}, {183, 335}, {189, 295}, {190, 260}, {183, 226}, {180, 213}},
        {{256, 492}, {268, 465}, {272, 435}, {264, 400}, {242, 370}, {222, 345}, {202, 322}, {201, 290}, {201, 255}, {212, 224}, {227, 213}},
        34, 6,
        [](float u, float v, float x, float y) {
            if (y >= 480.0f) return C_CREAM;
            if (y >= 458.0f) return C_SOFT_CRM;
            if (y >= 440.0f) return C_PALE_GLD;
            if (y >= 416.0f) return C_WARM_TAN;
            if (y >= 400.0f) return C_LIGHT;
            if (y >= 384.0f) return C_GOLD;
            if (y >= 264.0f) return C_ORANGE;
            if (y >= 244.0f) return C_MID;
            if (y >= 225.0f) return C_DARK;
            return C_DARKEST;
        }
    );

    addSplineRibbonPatch(
        {{416, 496}, {398, 472}, {384, 440}, {382, 405}, {390, 365}, {391, 320}, {391, 275}, {389, 235}, {385, 213}},
        {{418, 494}, {432, 466}, {432, 434}, {425, 400}, {416, 365}, {413, 320}, {404, 275}, {402, 235}, {420, 213}},
        30, 5,
        [](float u, float v, float x, float y) {
            if (y >= 465.0f) return C_WARM_TAN;
            if (y >= 434.0f) return C_LIGHT;
            if (y >= 402.0f) return C_GOLD;
            if (y >= 385.0f) return C_ORANGE;
            if (y >= 360.0f) return C_MID;
            if (y >= 338.0f) return C_MID_DARK;
            if (y >= 222.0f) return C_DARK;
            return C_DARKEST;
        }
    );

    addSplineRibbonPatch(
        {{426, 452}, {455, 412}, {485, 405}, {512, 430}, {522, 475}, {524, 515}},
        {{434, 426}, {464, 396}, {496, 396}, {522, 422}, {532, 470}, {530, 512}},
        24, 3,
        [](float u, float v, float x, float y) {
            return (v > 0.35f) ? C_MID : C_ORANGE;
        }
    );
    addSplineRibbonPatch(
        {{416, 496}, {442, 465}, {468, 448}, {490, 452}, {503, 480}, {506, 515}, {516, 535}},
        {{428, 442}, {456, 412}, {484, 410}, {508, 432}, {518, 475}, {522, 512}, {528, 528}},
        28, 5,
        [](float u, float v, float x, float y) {
            if (y > 435.0f && v < 0.55f) return C_LIGHT;
            if (v < 0.75f) return C_GOLD;
            return C_ORANGE;
        }
    );

    addSplineRibbonPatch(
        {{516, 535}, {535, 548}, {552, 540}, {575, 530}, {590, 522}, {597, 505}},
        {{520, 510}, {540, 495}, {558, 500}, {576, 495}, {590, 490}, {598, 492}},
        18, 5,
        [](float u, float v, float x, float y) {
            if (x > 584.0f && y < 516.0f) return C_DARKEST;
            if (x > 565.0f && v > 0.4f) return C_MID;
            if (x > 555.0f) return C_ORANGE;
            return C_GOLD;
        }
    );
    addSplineRibbonPatch(
        {{518, 520}, {530, 525}, {545, 518}, {552, 505}},
        {{520, 508}, {532, 490}, {546, 490}, {552, 502}},
        12, 4,
        [](float u, float v, float x, float y) {
            return (v < 0.6f) ? C_LIGHT : C_GOLD;
        }
    );
    addSplineRibbonPatch(
        {{542, 528}, {550, 531}, {558, 525}},
        {{542, 526}, {550, 520}, {558, 523}},
        6, 2,
        [](float u, float v, float x, float y) { return C_DARKEST; }
    );
    addSplineRibbonPatch(
        {{505, 526}, {512, 535}, {522, 535}},
        {{506, 522}, {514, 520}, {522, 526}},
        8, 2,
        [](float u, float v, float x, float y) { return C_MID; }
    );
    addSplineRibbonPatch(
        {{518, 542}, {525, 551}, {533, 550}},
        {{520, 536}, {527, 544}, {535, 544}},
        8, 2,
        [](float u, float v, float x, float y) {
            return (u < 0.5f) ? C_CREAM : C_DARK;
        }
    );

    size_t numVerts = camelMesh.size() * 3;
    posBuffer.reserve(numVerts * 2);
    flatColBuffer.reserve(numVerts * 3);
    smoothColBuffer.reserve(numVerts * 3);
    wireColBuffer.reserve(numVerts * 3);

    for (const auto& tri : camelMesh) {
        for (int k = 0; k < 3; ++k) {
            posBuffer.push_back(tri.v[k].x);
            posBuffer.push_back(tri.v[k].y);

            flatColBuffer.push_back(tri.flatColor.r);
            flatColBuffer.push_back(tri.flatColor.g);
            flatColBuffer.push_back(tri.flatColor.b);

            smoothColBuffer.push_back(tri.v[k].r);
            smoothColBuffer.push_back(tri.v[k].g);
            smoothColBuffer.push_back(tri.v[k].b);

            wireColBuffer.push_back(0.9f);
            wireColBuffer.push_back(0.7f);
            wireColBuffer.push_back(0.4f);
        }
    }
}

void drawText(float x, float y, const std::string& text) {
    glRasterPos2f(x, y);
    for (char c : text) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_12, c);
    }
}

void drawCamel() {
    if (wireframeMode) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1.0f);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    glShadeModel(smoothShading ? GL_SMOOTH : GL_FLAT);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glVertexPointer(2, GL_FLOAT, 0, posBuffer.data());

    if (smoothShading) {
        glColorPointer(3, GL_FLOAT, 0, smoothColBuffer.data());
    } else if (wireframeMode) {
        glColorPointer(3, GL_FLOAT, 0, wireColBuffer.data());
    } else {
        glColorPointer(3, GL_FLOAT, 0, flatColBuffer.data());
    }

    glDrawArrays(GL_TRIANGLES, 0, static_cast<GLsizei>(camelMesh.size() * 3));

    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void display() {
    glClearColor(92.0f / 255.0f, 51.0f / 255.0f, 55.0f / 255.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glLoadIdentity();

    glTranslatef(panX, panY, 0.0f);
    glTranslatef(WIN_W * 0.5f, WIN_H * 0.5f, 0.0f);
    glScalef(zoom, zoom, 1.0f);
    glTranslatef(-WIN_W * 0.5f, -WIN_H * 0.5f, 0.0f);

    if (animated) {
        float bobY = sinf(animTime * 4.0f) * 6.0f;
        float swayRot = sinf(animTime * 2.0f) * 1.5f;
        glTranslatef(WIN_W * 0.5f, 350.0f, 0.0f);
        glRotatef(swayRot, 0.0f, 0.0f, 1.0f);
        glTranslatef(-WIN_W * 0.5f, -350.0f + bobY, 0.0f);
    }

    drawCamel();

    glLoadIdentity();
    glColor3f(0.95f, 0.90f, 0.85f);
    std::string modeStr = "Shading: ";
    modeStr += (smoothShading ? "[Smooth Gouraud]" : "[Stylized Flat Facets]");
    if (wireframeMode) modeStr += "  Mesh: [Wireframe ON]";
    if (animated) modeStr += "  Animation: [Active]";
    drawText(20.0f, WIN_H - 25.0f, modeStr);
    drawText(20.0f, WIN_H - 42.0f, "Controls: [S] Shading Toggle  [W] Wireframe  [Space] Walk Animation  [+/-] Zoom  [Arrows] Pan  [R] Reset");

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case 's':
        case 'S':
            smoothShading = !smoothShading;
            break;
        case 'w':
        case 'W':
            wireframeMode = !wireframeMode;
            break;
        case ' ':
            animated = !animated;
            break;
        case '+':
        case '=':
            zoom *= 1.1f;
            break;
        case '-':
        case '_':
            zoom /= 1.1f;
            break;
        case 'r':
        case 'R':
            zoom = 1.0f;
            panX = 0.0f;
            panY = 0.0f;
            break;
        case 27:
        case 'q':
        case 'Q':
            exit(0);
    }
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    const float step = 25.0f / zoom;
    switch (key) {
        case GLUT_KEY_LEFT:  panX -= step; break;
        case GLUT_KEY_RIGHT: panX += step; break;
        case GLUT_KEY_UP:    panY += step; break;
        case GLUT_KEY_DOWN:  panY -= step; break;
    }
    glutPostRedisplay();
}

void timer(int value) {
    if (animated) {
        animTime += 0.035f;
        glutPostRedisplay();
    }
    glutTimerFunc(16, timer, 0);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("Lab 11 Assignment 03 - Procedural Spline Vector Camel");

    buildProceduralCamel();

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIN_W, 0, WIN_H);
    glMatrixMode(GL_MODELVIEW);

    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);
    glutTimerFunc(16, timer, 0);

    glutMainLoop();
    return 0;
}
