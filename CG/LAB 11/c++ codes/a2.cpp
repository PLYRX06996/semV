#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <string>
#include <vector>
#include <iostream>

#define PI 3.14159265358979323846f

const float W = 1440.0f;
const float H = 896.0f;
const int INITIAL_WINDOW_W = 1440;
const int INITIAL_WINDOW_H = 896;

int windowWidth = INITIAL_WINDOW_W;
int windowHeight = INITIAL_WINDOW_H;
float rasterScale = 1.0f;

int viewMode = 1;
bool nightMode = false;
float zoom = 1.0f;
float panX = 0.0f, panY = 0.0f;

inline void col(int r, int g, int b, float a = 1.0f) {
    glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a);
}
inline void vcol(int r, int g, int b, float a = 1.0f) {
    glColor4f(r / 255.0f, g / 255.0f, b / 255.0f, a);
}

void rect(float x1, float y1, float x2, float y2) {
    glBegin(GL_QUADS);
        glVertex2f(x1, y1); glVertex2f(x2, y1);
        glVertex2f(x2, y2); glVertex2f(x1, y2);
    glEnd();
}

void quad(float x1, float y1, float x2, float y2,
          float x3, float y3, float x4, float y4) {
    glBegin(GL_QUADS);
        glVertex2f(x1, y1); glVertex2f(x2, y2);
        glVertex2f(x3, y3); glVertex2f(x4, y4);
    glEnd();
}

void line(float x1, float y1, float x2, float y2, float w = 1.5f) {
    glLineWidth(w);
    glBegin(GL_LINES);
        glVertex2f(x1, y1); glVertex2f(x2, y2);
    glEnd();
}

void circle(float cx, float cy, float r, int segs = 96) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; ++i) {
        float a = 2.0f * PI * i / segs;
        glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
    }
    glEnd();
}

void ring(float cx, float cy, float rOut, float rIn, int segs = 96) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float a = 2.0f * PI * i / segs;
        glVertex2f(cx + rOut * cosf(a), cy + rOut * sinf(a));
        glVertex2f(cx + rIn  * cosf(a), cy + rIn  * sinf(a));
    }
    glEnd();
}

void ringArc(float cx, float cy, float rOut, float rIn, float startA, float endA, int segs = 64) {
    glBegin(GL_TRIANGLE_STRIP);
    for (int i = 0; i <= segs; ++i) {
        float a = startA + (endA - startA) * i / segs;
        glVertex2f(cx + rOut * cosf(a), cy + rOut * sinf(a));
        glVertex2f(cx + rIn  * cosf(a), cy + rIn  * sinf(a));
    }
    glEnd();
}

void ellipse(float cx, float cy, float rx, float ry, int segs = 96) {
    glBegin(GL_TRIANGLE_FAN);
    glVertex2f(cx, cy);
    for (int i = 0; i <= segs; ++i) {
        float a = 2.0f * PI * i / segs;
        glVertex2f(cx + rx * cosf(a), cy + ry * sinf(a));
    }
    glEnd();
}

void roundRect(float x1, float y1, float x2, float y2, float r, int segs = 24) {
    rect(x1 + r, y1, x2 - r, y2);
    rect(x1, y1 + r, x1 + r, y2 - r);
    rect(x2 - r, y1 + r, x2, y2 - r);
    auto drawCorner = [&](float cx, float cy, float startAng) {
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(cx, cy);
        for (int i = 0; i <= segs; ++i) {
            float a = startAng + (0.5f * PI * i) / segs;
            glVertex2f(cx + r * cosf(a), cy + r * sinf(a));
        }
        glEnd();
    };
    drawCorner(x1 + r, y1 + r, PI);
    drawCorner(x2 - r, y1 + r, 1.5f * PI);
    drawCorner(x2 - r, y2 - r, 0.0f);
    drawCorner(x1 + r, y2 - r, 0.5f * PI);
}

void strokeTextBold(const char* s, float x, float y, float height, float lineW) {
    float sc = height / 119.05f;
    glLineWidth(lineW);
    float offsets[3][2] = { {0.0f, 0.0f}, {0.8f, 0.0f}, {0.0f, 0.8f} };
    for (int pass = 0; pass < 3; ++pass) {
        glPushMatrix();
        glTranslatef(x + offsets[pass][0], y + offsets[pass][1], 0);
        glScalef(sc, -sc, 1.0f);
        for (const char* p = s; *p; ++p) {
            if (*p == ' ') {
                glTranslatef(55.0f, 0.0f, 0.0f);
            } else {
                glutStrokeCharacter(GLUT_STROKE_ROMAN, *p);
            }
        }
        glPopMatrix();
    }
}

void drawBitmapText(float x, float y, const std::string& str, void* font = GLUT_BITMAP_HELVETICA_18) {
    glRasterPos2f(x, y);
    for (char c : str) glutBitmapCharacter(font, c);
}

void drawDevanagariMotto(float cx, float cy, float scale) {
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scale, scale, 1.0f);
    glLineWidth(2.2f);

    glBegin(GL_LINES);
        glVertex2f(-105.0f, 0.0f); glVertex2f(105.0f, 0.0f);
    glEnd();

    glBegin(GL_LINES);
        glVertex2f(-95.0f, 0.0f); glVertex2f(-95.0f, 22.0f);
        glVertex2f(-75.0f, 0.0f); glVertex2f(-75.0f, 22.0f);
        glVertex2f(-52.0f, 0.0f); glVertex2f(-52.0f, 22.0f);
        glVertex2f(-30.0f, 0.0f); glVertex2f(-30.0f, 22.0f);
        glVertex2f(-10.0f, 0.0f); glVertex2f(-10.0f, 22.0f);
        glVertex2f(12.0f, 0.0f);  glVertex2f(12.0f, 22.0f);
        glVertex2f(34.0f, 0.0f);  glVertex2f(34.0f, 22.0f);
    glEnd();
    ringArc(-99.0f, 11.0f, 7.0f, 5.0f, -0.5f * PI, 0.5f * PI, 16);
    ringArc(-34.0f, 11.0f, 7.0f, 5.0f, -0.5f * PI, 0.5f * PI, 16);
    ringArc(-52.0f, 11.0f, 6.5f, 4.5f, 0.5f * PI, 1.5f * PI, 16);
    ringArc(12.0f, 22.0f, 5.5f, 3.5f, 0.0f, PI, 16);
    circle(34.0f, -5.0f, 2.4f, 12);

    glBegin(GL_LINES);
        glVertex2f(60.0f, 0.0f);  glVertex2f(60.0f, 22.0f);
        glVertex2f(72.0f, 0.0f);  glVertex2f(72.0f, 22.0f);
        glVertex2f(88.0f, 0.0f);  glVertex2f(88.0f, 22.0f);
        glVertex2f(100.0f, 0.0f); glVertex2f(100.0f, 22.0f);
        glVertex2f(100.0f, 22.0f); glVertex2f(107.0f, 29.0f);
    glEnd();
    ringArc(53.0f, 10.0f, 6.0f, 4.0f, 0.5f * PI, 1.5f * PI, 16);
    circle(88.0f, -5.0f, 2.4f, 12);

    glPopMatrix();
}

void drawOfficialLogo(float cx, float cy, float scale) {
    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glScalef(scale, scale, 1.0f);

    const int NAVY_R = 7,   NAVY_G = 19,  NAVY_B = 99;
    const int RING_R = 175, RING_G = 182, RING_B = 192;
    const int PED_R  = 162, PED_G  = 170, PED_B  = 178;

    float ringCy = -90.0f;
    col(RING_R, RING_G, RING_B);
    ring(0.0f, ringCy, 56.0f, 51.5f, 72);
    ring(0.0f, ringCy, 43.0f, 38.5f, 72);
    ring(0.0f, ringCy, 30.5f, 26.5f, 72);

    col(NAVY_R, NAVY_G, NAVY_B);
    circle(0.0f, ringCy, 20.0f, 64);

    col(PED_R, PED_G, PED_B);
    rect(-20.0f, -12.0f, 20.0f, 70.0f);

    col(NAVY_R, NAVY_G, NAVY_B);

    glBegin(GL_POLYGON);
        glVertex2f(-56.0f, 18.0f);
        glVertex2f(-24.0f, 18.0f);
        glVertex2f(-24.0f, -38.0f);
        for (int i = 0; i <= 24; ++i) {
            float th = PI + i * (0.5f * PI) / 24;
            glVertex2f(-40.0f + 16.0f * cosf(th), -22.0f + 16.0f * sinf(th));
        }
        glVertex2f(-56.0f, -22.0f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(-15.0f, 18.0f);
        glVertex2f(15.0f, 18.0f);
        glVertex2f(15.0f, -38.0f);
        for (int i = 0; i <= 24; ++i) {
            float th = i * PI / 24;
            glVertex2f(15.0f * cosf(th), -32.0f - 8.0f * sinf(th));
        }
        glVertex2f(-15.0f, -38.0f);
    glEnd();

    glBegin(GL_POLYGON);
        glVertex2f(24.0f, 18.0f);
        glVertex2f(56.0f, 18.0f);
        glVertex2f(56.0f, -22.0f);
        for (int i = 0; i <= 24; ++i) {
            float th = 1.5f * PI + i * (0.5f * PI) / 24;
            glVertex2f(40.0f + 16.0f * cosf(th), -22.0f + 16.0f * sinf(th));
        }
        glVertex2f(24.0f, -38.0f);
    glEnd();

    rect(-56.0f, 18.0f, 56.0f, 26.0f);
    circle(-50.0f, 22.0f, 4.0f, 24);
    circle(50.0f, 22.0f, 4.0f, 24);

    col(20, 25, 45);
    drawDevanagariMotto(0.0f, 92.0f, 0.72f);

    glPopMatrix();
}

void drawSky() {
    glBegin(GL_QUADS);
        if (!nightMode) {
            vcol(172, 188, 222); glVertex2f(0.0f, 0.0f);
            vcol(172, 188, 222); glVertex2f(W, 0.0f);
            vcol(228, 218, 228); glVertex2f(W, 547.2f);
            vcol(228, 218, 228); glVertex2f(0.0f, 547.2f);
        } else {
            vcol(12, 16, 32);   glVertex2f(0.0f, 0.0f);
            vcol(12, 16, 32);   glVertex2f(W, 0.0f);
            vcol(25, 32, 55);   glVertex2f(W, 547.2f);
            vcol(25, 32, 55);   glVertex2f(0.0f, 547.2f);
        }
    glEnd();
}

void drawPerspectiveBuilding() {
    if (!nightMode) col(186, 120, 104);
    else col(95, 60, 52);
    quad(585.6f, 137.6f, W, -96.0f, W, 451.2f, 585.6f, 406.4f);

    if (!nightMode) col(208, 142, 122);
    else col(110, 72, 62);
    quad(585.6f, 137.6f, W, -96.0f, W, -48.0f, 585.6f, 160.0f);

    if (!nightMode) col(152, 94, 82);
    else col(75, 46, 40);
    for (int k = 1; k <= 5; ++k) {
        float yL = 358.4f + k * 10.0f;
        float yR = 150.4f + k * 10.0f + 50.0f;
        line(649.6f, yL, W, yR, 1.5f);
    }
    for (float x = 660.0f; x < W; x += 85.0f) {
        float yTop = 368.0f - (x - 649.6f) * 0.26f;
        line(x, yTop, x, 450.0f, 1.5f);
    }

    glBegin(GL_QUADS);
        vcol(35, 42, 52);   glVertex2f(649.6f, 185.6f);
        vcol(50, 58, 70);   glVertex2f(1137.6f, 0.0f);
        vcol(90, 98, 110);  glVertex2f(1137.6f, 230.4f);
        vcol(65, 72, 85);   glVertex2f(649.6f, 358.4f);
    glEnd();
    glBegin(GL_QUADS);
        vcol(50, 58, 70);   glVertex2f(1137.6f, 0.0f);
        vcol(40, 48, 58);   glVertex2f(W, -76.8f);
        vcol(78, 86, 98);   glVertex2f(W, 150.4f);
        vcol(90, 98, 110);  glVertex2f(1137.6f, 230.4f);
    glEnd();

    col(28, 32, 38);
    int numBays = 13;
    float bayStep = (W - 649.6f) / numBays;
    for (int i = 0; i <= numBays; ++i) {
        float x = 649.6f + i * bayStep;
        float yTop = 185.6f - (x - 649.6f) * 0.38f;
        float yBot = 358.4f - (x - 649.6f) * 0.262f;
        line(x, yTop, x, yBot, 3.5f);

        if (i < numBays) {
            float xNext = x + bayStep;
            float yTopN = 185.6f - (xNext - 649.6f) * 0.38f;
            float yBotN = 358.4f - (xNext - 649.6f) * 0.262f;
            if (!nightMode) {
                glBegin(GL_TRIANGLES);
                    vcol(180, 210, 240, 0.18f);
                    glVertex2f(x + 4.0f, yTop + 6.0f);
                    glVertex2f(xNext - 4.0f, yTopN + 6.0f);
                    glVertex2f(x + 4.0f, yBot - 12.0f);
                glEnd();
            } else if ((i % 3) == 0) {
                glBegin(GL_QUADS);
                    vcol(245, 215, 110, 0.75f);
                    glVertex2f(x + 3.0f, yTop + 3.0f);
                    glVertex2f(xNext - 3.0f, yTopN + 3.0f);
                    glVertex2f(xNext - 3.0f, yBotN - 3.0f);
                    glVertex2f(x + 3.0f, yBot - 3.0f);
                glEnd();
            }
        }
    }
    line(649.6f, 185.6f, W, -76.8f, 4.5f);
    line(649.6f, 358.4f, W, 150.4f, 4.5f);
    line(649.6f, 272.0f, W, 36.8f, 3.5f);

    float plX = 870.0f, plY = 320.0f;
    col(160, 162, 168); roundRect(plX, plY, plX + 46.0f, plY + 70.0f, 4.0f);
    col(245, 245, 248); rect(plX + 13.0f, plY + 12.0f, plX + 19.0f, plY + 45.0f);
    rect(plX + 23.0f, plY + 12.0f, plX + 29.0f, plY + 45.0f);
    rect(plX + 33.0f, plY + 12.0f, plX + 39.0f, plY + 45.0f);
    col(7, 19, 99); circle(plX + 23.0f, plY + 7.0f, 4.0f, 16);

    if (!nightMode) col(196, 118, 106);
    else col(102, 60, 54);
    rect(432.0f, 64.0f, 624.0f, 480.0f);
    if (!nightMode) col(218, 148, 130);
    else col(115, 78, 68);
    rect(425.0f, 57.0f, 630.0f, 74.0f);
    if (!nightMode) col(188, 200, 225);
    else col(15, 20, 40);
    rect(467.0f, 99.0f, 589.0f, 195.0f);
    if (!nightMode) col(176, 104, 92);
    else col(90, 52, 46);
    rect(523.0f, 99.0f, 533.0f, 195.0f);
    rect(467.0f, 99.0f, 589.0f, 106.0f);
    col(215, 215, 222); line(504.0f, 57.0f, 504.0f, 0.0f, 2.5f);

    glBegin(GL_POLYGON);
        if (!nightMode) {
            vcol(85, 115, 35);   glVertex2f(480.0f, 406.4f);
            vcol(195, 220, 80);  glVertex2f(W, 451.2f);
            vcol(225, 242, 95);  glVertex2f(W, 510.0f);
        } else {
            vcol(65, 85, 25);   glVertex2f(480.0f, 406.4f);
            vcol(140, 160, 50); glVertex2f(W, 451.2f);
            vcol(160, 180, 60); glVertex2f(W, 510.0f);
        }
        int archSteps = 40;
        for (int i = archSteps; i >= 0; --i) {
            float t = (float)i / archSteps;
            float ax = 480.0f + t * (W - 480.0f);
            float normX = (ax - 960.0f) / 480.0f;
            float ay = 460.0f + 45.0f * (normX * normX);
            glVertex2f(ax, ay);
        }
    glEnd();

    col(40, 50, 32);
    quad(480.0f, 467.2f, W, 508.8f, W, 547.2f, 480.0f, 547.2f);
    col(95, 75, 65);
    for (float x = 528.0f; x < W; x += 140.0f) {
        rect(x, 480.0f, x + 16.0f, 547.2f);
    }
    for (int i = 0; i < 5; ++i) {
        float tx = 1180.0f + i * 55.0f;
        col(35 + i * 5, 105 + i * 8, 45);
        circle(tx, 520.0f, 28.0f, 24);
        circle(tx - 10.0f, 510.0f, 22.0f, 20);
    }
}

void drawLeftGraphicPanel() {
    col(242, 218, 232);
    glBegin(GL_TRIANGLES);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(97.6f, 0.0f);
        glVertex2f(0.0f, 195.2f);
    glEnd();

    col(138, 202, 232);
    quad(97.6f, 0.0f, 150.4f, 0.0f, 0.0f, 300.8f, 0.0f, 195.2f);

    glBegin(GL_POLYGON);
        vcol(88, 75, 165);   glVertex2f(150.4f, 0.0f);
        vcol(92, 78, 165);   glVertex2f(441.6f, 0.0f);
        vcol(205, 70, 150);  glVertex2f(168.0f, 547.2f);
        vcol(215, 70, 150);  glVertex2f(0.0f, 547.2f);
        vcol(150, 70, 160);  glVertex2f(0.0f, 300.8f);
    glEnd();
}

void drawTopRightSun() {
    glBegin(GL_TRIANGLE_FAN);
        vcol(255, 230, 125, 0.95f); glVertex2f(W + 10.0f, -10.0f);
        vcol(242, 152, 148, 0.0f);
        for (int i = 0; i <= 36; ++i) {
            float a = PI + i * (0.5f * PI) / 36;
            glVertex2f(W + 10.0f + 140.0f * cosf(a), -10.0f + 140.0f * sinf(a));
        }
    glEnd();
}

void drawPoleAndStreetLamp() {
    glBegin(GL_QUADS);
        vcol(152, 162, 172); glVertex2f(1337.6f, 0.0f);   glVertex2f(1347.2f, 0.0f);
        vcol(152, 162, 172); glVertex2f(1347.2f, 704.0f); glVertex2f(1337.6f, 704.0f);
        vcol(238, 242, 246); glVertex2f(1347.2f, 0.0f);   glVertex2f(1360.0f, 0.0f);
        vcol(238, 242, 246); glVertex2f(1360.0f, 704.0f); glVertex2f(1347.2f, 704.0f);
    glEnd();

    col(80, 85, 95);
    rect(1334.0f, 150.0f, 1362.0f, 165.0f);

    col(255, 255, 250);
    circle(1348.0f, 135.0f, 16.0f, 32);

    col(255, 255, 210, nightMode ? 0.65f : 0.35f);
    circle(1348.0f, 135.0f, nightMode ? 45.0f : 28.0f, 32);
}

void drawBandAndGround() {
    col(208, 186, 168); rect(0.0f, 547.2f, W, 742.4f);

    glBegin(GL_QUADS);
        if (!nightMode) {
            vcol(152, 142, 144); glVertex2f(0.0f, 742.4f); glVertex2f(W, 742.4f);
            vcol(112, 102, 108); glVertex2f(W, H);          glVertex2f(0.0f, H);
        } else {
            vcol(65, 60, 62);   glVertex2f(0.0f, 742.4f); glVertex2f(W, 742.4f);
            vcol(45, 40, 44);   glVertex2f(W, H);          glVertex2f(0.0f, H);
        }
    glEnd();

    col(128, 118, 122);
    for (float y = 765.0f; y < H; y += 26.0f) line(0.0f, y, W, y, 1.2f);
    for (float x = 95.0f; x < W; x += 110.0f) line(x, 742.4f, x - 45.0f, H, 1.2f);

    glBegin(GL_TRIANGLES);
        vcol(226, 122, 172); glVertex2f(0.0f, 742.4f);
        vcol(236, 142, 186); glVertex2f(88.0f, 742.4f);
        vcol(230, 126, 176); glVertex2f(0.0f, H);
    glEnd();

    float baseL = 1326.4f, baseR = 1425.6f;
    float baseCx = (baseL + baseR) * 0.5f;
    float rx = (baseR - baseL) * 0.5f;

    col(226, 230, 232); rect(baseL, 704.0f, baseR, 784.0f);
    col(192, 196, 200); ellipse(baseCx, 784.0f, rx, 12.0f);
    col(246, 246, 248); ellipse(baseCx, 704.0f, rx, 12.0f);
}

void drawLogoCard() {
    float cardX1 = 124.8f, cardY1 = 329.6f;
    float cardX2 = 489.6f, cardY2 = 729.6f;

    col(185, 212, 228, 0.75f);
    roundRect(cardX1 - 8.0f, cardY1 - 8.0f, cardX2 + 8.0f, cardY2 + 8.0f, 36.0f);

    col(214, 235, 243);
    roundRect(cardX1, cardY1, cardX2, cardY2, 32.0f);

    col(255, 255, 255);
    roundRect(cardX1 + 18.0f, cardY1 + 14.0f, cardX2 - 18.0f, cardY2 - 14.0f, 22.0f);

    float logoCx = (cardX1 + cardX2) * 0.5f;
    float logoCy = cardY1 + 185.0f;
    drawOfficialLogo(logoCx, logoCy, 1.45f);
}

void drawBannerTypography() {
    float icX = 206.4f, icY = 124.8f;
    col(18, 168, 168); circle(icX, icY, 15.0f, 32);
    col(255, 255, 255); rect(icX - 9.0f, icY - 2.5f, icX + 9.0f, icY + 2.5f);
    col(242, 128, 32);
    strokeTextBold("IndCareer", icX + 24.0f, icY + 8.0f, 24.0f, 2.4f);

    float txX = 518.4f;

    col(15, 22, 32);
    strokeTextBold("ABV-IIITM Gwalior", txX, 615.0f, 44.0f, 2.6f);

    col(35, 75, 185);
    strokeTextBold("Admission", txX, 675.0f, 40.0f, 2.6f);

    col(55, 58, 65);
    strokeTextBold("www.indcareer.com", txX, 720.0f, 20.0f, 1.8f);
}

void configureProjection() {
    const float scale = std::min(windowWidth / W, windowHeight / H);
    const int viewportW = static_cast<int>(W * scale + 0.5f);
    const int viewportH = static_cast<int>(H * scale + 0.5f);
    const int viewportX = (windowWidth - viewportW) / 2;
    const int viewportY = (windowHeight - viewportH) / 2;

    rasterScale = scale;
    glViewport(viewportX, viewportY, viewportW, viewportH);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0.0, W, H, 0.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    glTranslatef(panX, panY, 0.0f);
    glTranslatef(W * 0.5f, H * 0.5f, 0.0f);
    glScalef(zoom, zoom, 1.0f);
    glTranslatef(-W * 0.5f, -H * 0.5f, 0.0f);

    if (viewMode == 1) {
        drawSky();
        drawPerspectiveBuilding();
        drawLeftGraphicPanel();
        drawTopRightSun();
        drawPoleAndStreetLamp();
        drawBandAndGround();
        drawLogoCard();
        drawBannerTypography();
    } else if (viewMode == 2) {
        drawSky();
        drawPerspectiveBuilding();
        drawPoleAndStreetLamp();
        col(208, 186, 168); rect(0.0f, 547.2f, W, 742.4f);
        glBegin(GL_QUADS);
            vcol(152, 142, 144); glVertex2f(0.0f, 742.4f); glVertex2f(W, 742.4f);
            vcol(112, 102, 108); glVertex2f(W, H);          glVertex2f(0.0f, H);
        glEnd();
        col(128, 118, 122);
        for (float y = 765.0f; y < H; y += 26.0f) line(0.0f, y, W, y, 1.2f);
        for (float x = 95.0f; x < W; x += 110.0f) line(x, 742.4f, x - 45.0f, H, 1.2f);
    } else if (viewMode == 3) {
        glClearColor(0.96f, 0.97f, 0.98f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        drawOfficialLogo(W * 0.5f, H * 0.48f, 2.5f);
        col(7, 19, 99);
        strokeTextBold("ABV-IIITM GWALIOR", W * 0.5f - 240.0f, H * 0.82f, 36.0f, 2.5f);
    }

    glLoadIdentity();
    col(20, 20, 25);
    std::string modeStr = "Mode: ";
    if (viewMode == 1) modeStr += "[1] Poster View";
    else if (viewMode == 2) modeStr += "[2] Campus Architecture View";
    else if (viewMode == 3) modeStr += "[3] Official Institute Emblem View";
    if (nightMode) modeStr += "  [Night Lighting Active]";
    drawBitmapText(30.0f, 35.0f, modeStr, GLUT_BITMAP_HELVETICA_12);
    drawBitmapText(30.0f, 55.0f, "Controls: [1] Poster  [2] Campus  [3] Emblem  [L] Day/Night  [+/-] Zoom  [Arrows] Pan  [R] Reset", GLUT_BITMAP_HELVETICA_10);

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    switch (key) {
        case '1': viewMode = 1; break;
        case '2': viewMode = 2; break;
        case '3': viewMode = 3; break;
        case 'l':
        case 'L': nightMode = !nightMode; break;
        case '+':
        case '=': zoom *= 1.1f; break;
        case '-':
        case '_': zoom /= 1.1f; break;
        case 'r':
        case 'R': zoom = 1.0f; panX = 0.0f; panY = 0.0f; break;
        case 27:
        case 'q':
        case 'Q': exit(0);
    }
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    const float step = 35.0f / zoom;
    switch (key) {
        case GLUT_KEY_LEFT:  panX -= step; break;
        case GLUT_KEY_RIGHT: panX += step; break;
        case GLUT_KEY_UP:    panY += step; break;
        case GLUT_KEY_DOWN:  panY -= step; break;
    }
    glutPostRedisplay();
}

void reshape(int width, int height) {
    windowWidth = std::max(width, 1);
    windowHeight = std::max(height, 1);
    configureProjection();
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE);
    glutInitWindowSize(INITIAL_WINDOW_W, INITIAL_WINDOW_H);
    glutCreateWindow("Assignment 02 - ABV-IIITM Gwalior Admission (High Resolution)");

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    configureProjection();
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutReshapeFunc(reshape);

    glutMainLoop();
    return 0;
}
