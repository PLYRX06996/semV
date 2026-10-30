#include <GL/freeglut.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>

#define PI 3.14159265f

struct V3 { float x, y, z; V3() : x(0), y(0), z(0) {} V3(float a, float b, float c) : x(a), y(b), z(c) {} };
V3 operator+(V3 a, V3 b) { return V3(a.x + b.x, a.y + b.y, a.z + b.z); }
V3 operator-(V3 a, V3 b) { return V3(a.x - b.x, a.y - b.y, a.z - b.z); }
V3 operator*(V3 a, float s) { return V3(a.x * s, a.y * s, a.z * s); }
V3 operator*(float s, V3 a) { return V3(a.x * s, a.y * s, a.z * s); }
V3 mulc(V3 a, V3 b) { return V3(a.x * b.x, a.y * b.y, a.z * b.z); }
float dot(V3 a, V3 b) { return a.x * b.x + a.y * b.y + a.z * b.z; }
V3 cross(V3 a, V3 b) { return V3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x); }
float len(V3 a) { return sqrtf(dot(a, a)); }
V3 normalize(V3 a) { float l = len(a); return (l > 0) ? a * (1.0f / l) : a; }
float clamp01(float v) { return v < 0 ? 0 : (v > 1 ? 1 : v); }

struct Mat4 { float m[16]; };

Mat4 mIdentity() { Mat4 r; for (int i = 0; i < 16; i++) r.m[i] = (i % 5 == 0) ? 1.0f : 0.0f; return r; }

Mat4 mMul(const Mat4& a, const Mat4& b)
{
    Mat4 r;
    for (int c = 0; c < 4; c++)
        for (int rr = 0; rr < 4; rr++)
        {
            float s = 0;
            for (int k = 0; k < 4; k++) s += a.m[k * 4 + rr] * b.m[c * 4 + k];
            r.m[c * 4 + rr] = s;
        }
    return r;
}

Mat4 mTranslate(float x, float y, float z) { Mat4 r = mIdentity(); r.m[12] = x; r.m[13] = y; r.m[14] = z; return r; }

Mat4 mRotate(float deg, float ax, float ay, float az)
{
    V3 a = normalize(V3(ax, ay, az));
    float r = deg * PI / 180, c = cosf(r), s = sinf(r), t = 1 - c;
    Mat4 m = mIdentity();
    m.m[0] = t * a.x * a.x + c;       m.m[4] = t * a.x * a.y - s * a.z;  m.m[8]  = t * a.x * a.z + s * a.y;
    m.m[1] = t * a.x * a.y + s * a.z; m.m[5] = t * a.y * a.y + c;        m.m[9]  = t * a.y * a.z - s * a.x;
    m.m[2] = t * a.x * a.z - s * a.y; m.m[6] = t * a.y * a.z + s * a.x;  m.m[10] = t * a.z * a.z + c;
    return m;
}

Mat4 mReflect(float a, float b, float c, float d)
{
    float n[3] = { a, b, c }, N = a * a + b * b + c * c;
    Mat4 m = mIdentity();
    for (int col = 0; col < 3; col++)
        for (int row = 0; row < 3; row++)
            m.m[col * 4 + row] = (row == col ? 1.0f : 0.0f) - 2.0f * n[row] * n[col] / N;
    for (int row = 0; row < 3; row++) m.m[12 + row] = -2.0f * d * n[row] / N;
    return m;
}

V3 mPoint(const Mat4& m, V3 p)
{
    return V3(m.m[0] * p.x + m.m[4] * p.y + m.m[8] * p.z + m.m[12],
              m.m[1] * p.x + m.m[5] * p.y + m.m[9] * p.z + m.m[13],
              m.m[2] * p.x + m.m[6] * p.y + m.m[10] * p.z + m.m[14]);
}

V3 mDir(const Mat4& m, V3 p)
{
    return V3(m.m[0] * p.x + m.m[4] * p.y + m.m[8] * p.z,
              m.m[1] * p.x + m.m[5] * p.y + m.m[9] * p.z,
              m.m[2] * p.x + m.m[6] * p.y + m.m[10] * p.z);
}

Mat4 mInverseAffine(const Mat4& s)
{
    float a = s.m[0], b = s.m[4], c = s.m[8], d = s.m[1], e = s.m[5], f = s.m[9], g = s.m[2], h = s.m[6], i = s.m[10];
    float A = e * i - f * h, B = -(d * i - f * g), C = d * h - e * g;
    float det = a * A + b * B + c * C;
    float id = 1.0f / det;
    Mat4 r = mIdentity();
    r.m[0] = A * id;                 r.m[4] = -(b * i - c * h) * id;  r.m[8]  = (b * f - c * e) * id;
    r.m[1] = B * id;                 r.m[5] = (a * i - c * g) * id;   r.m[9]  = -(a * f - c * d) * id;
    r.m[2] = C * id;                 r.m[6] = -(a * h - b * g) * id;  r.m[10] = (a * e - b * d) * id;
    V3 t(s.m[12], s.m[13], s.m[14]);
    r.m[12] = -(r.m[0] * t.x + r.m[4] * t.y + r.m[8]  * t.z);
    r.m[13] = -(r.m[1] * t.x + r.m[5] * t.y + r.m[9]  * t.z);
    r.m[14] = -(r.m[2] * t.x + r.m[6] * t.y + r.m[10] * t.z);
    return r;
}

const int NUM_SCENES = 3;
Mat4 sceneM[NUM_SCENES];

void buildScenes()
{
    sceneM[0] = mIdentity();

    sceneM[1] = mReflect(1, 2, 3, 1);

    sceneM[2] = mMul(mTranslate(1, 2, 3),
                     mMul(mRotate(30, 1, 2, 3), mTranslate(-1, -2, -3)));
}

const char* sceneName[NUM_SCENES]    = { "Scene-01", "Scene-02", "Scene-03" };
const char* sceneFormula[NUM_SCENES] = {
    "Unit cube w.r.t WCS",
    "Reflection about x + 2y + 3z + 1 = 0",
    "Rotate 30 deg about (x-1)/1=(y-2)/2=(z-3)/3"
};

const float FLOOR_Y = -1.7f;
const int   FLOOR_N = 8;
const float FLOOR_H = 4.0f;

struct Light { V3 pos, amb, dif, spec; };
const Light LIGHTS[2] = {
    { V3(-1.0f, 4.0f, -1.5f), V3(0.20f, 0.20f, 0.20f), V3(0.85f, 0.85f, 0.85f), V3(1, 1, 1) },
    { V3( 4.0f, 3.5f,  6.0f), V3(0.00f, 0.00f, 0.00f), V3(0.45f, 0.45f, 0.45f), V3(0, 0, 0) }
};
const V3 GLOBAL_AMB(0.08f, 0.08f, 0.08f);

struct Surface { V3 color, ks; float shin, refl; };
const Surface CUBE_SURF   = { V3(0.85f, 0.25f, 0.20f), V3(0.90f, 0.90f, 0.90f), 48.0f, 0.15f };
const Surface FLOOR_SURF0 = { V3(0.84f, 0.82f, 0.74f), V3(0.08f, 0.08f, 0.08f), 10.0f, 0.22f };
const Surface FLOOR_SURF1 = { V3(0.40f, 0.48f, 0.58f), V3(0.08f, 0.08f, 0.08f), 10.0f, 0.22f };
const V3 BACKGROUND(0.93f, 0.95f, 0.98f);

const Surface& floorSurface(int i, int j) { return ((i + j) & 1) ? FLOOR_SURF1 : FLOOR_SURF0; }

const float DEF_AZ = 35.0f, DEF_EL = 45.0f;
float camAz = DEF_AZ, camEl = DEF_EL;
const float CAM_DIST = 9.5f, FOVY = 40.0f;
const float TARGET[3] = { 0.1f, -0.4f, -0.4f };

const int CUBE_FACE[6][4] = { {1,3,7,5}, {0,4,6,2}, {2,6,7,3}, {0,1,5,4}, {4,5,7,6}, {0,2,3,1} };

struct CubeObj
{
    Mat4 M, Minv;
    V3 v[8];
    V3 fn[6];
};

CubeObj makeCube(int scene)
{
    CubeObj c;
    c.M = sceneM[scene];
    c.Minv = mInverseAffine(c.M);
    V3 centre(0, 0, 0);
    for (int k = 0; k < 8; k++)
    {
        c.v[k] = mPoint(c.M, V3((float)(k & 1), (float)((k >> 1) & 1), (float)((k >> 2) & 1)));
        centre = centre + c.v[k] * (1.0f / 8);
    }
    for (int f = 0; f < 6; f++)
    {
        V3 a = c.v[CUBE_FACE[f][0]], b = c.v[CUBE_FACE[f][1]], d = c.v[CUBE_FACE[f][2]];
        V3 n = normalize(cross(b - a, d - a));
        V3 fc = (a + c.v[CUBE_FACE[f][2]]) * 0.5f;
        if (dot(n, fc - centre) < 0) n = n * -1.0f;
        c.fn[f] = n;
    }
    return c;
}

V3 illuminate(V3 P, V3 N, V3 eye, const Surface& s, const bool* lit)
{
    V3 V = normalize(eye - P);
    V3 c = mulc(GLOBAL_AMB, s.color);
    for (int i = 0; i < 2; i++)
    {
        c = c + mulc(LIGHTS[i].amb, s.color);
        if (!lit[i]) continue;
        V3 L = normalize(LIGHTS[i].pos - P);
        float nl = dot(N, L);
        if (nl > 0)
        {
            c = c + mulc(LIGHTS[i].dif, s.color) * nl;
            V3 H = normalize(L + V);
            float nh = dot(N, H);
            if (nh > 0) c = c + mulc(LIGHTS[i].spec, s.ks) * powf(nh, s.shin);
        }
    }
    return c;
}

struct Cam { V3 eye, f, r, u; float tanH, aspect; int w, h; };

Cam makeCam(int w, int h)
{
    Cam c;
    float az = camAz * PI / 180, el = camEl * PI / 180;
    c.eye = V3(TARGET[0] + CAM_DIST * cosf(el) * sinf(az),
               TARGET[1] + CAM_DIST * sinf(el),
               TARGET[2] + CAM_DIST * cosf(el) * cosf(az));
    c.f = normalize(V3(TARGET[0], TARGET[1], TARGET[2]) - c.eye);
    c.r = normalize(cross(c.f, V3(0, 1, 0)));
    c.u = cross(c.r, c.f);
    c.tanH = tanf(FOVY * 0.5f * PI / 180);
    c.aspect = (float)w / h;
    c.w = w; c.h = h;
    return c;
}

bool project(const Cam& c, V3 P, float* px, float* py, float* invW)
{
    V3 v = P - c.eye;
    float zc = dot(v, c.f);
    if (zc < 0.2f) return false;
    float nx = dot(v, c.r) / (zc * c.tanH * c.aspect);
    float ny = dot(v, c.u) / (zc * c.tanH);
    *px = (nx * 0.5f + 0.5f) * c.w;
    *py = (ny * 0.5f + 0.5f) * c.h;
    *invW = 1.0f / zc;
    return true;
}

struct Image
{
    int w, h;
    std::vector<unsigned char> px;
    void init(int W, int H) { w = W; h = H; px.assign((size_t)W * H * 3, 0); }
    void set(int x, int y, V3 c)
    {
        unsigned char* p = &px[((size_t)y * w + x) * 3];
        p[0] = (unsigned char)(clamp01(c.x) * 255 + 0.5f);
        p[1] = (unsigned char)(clamp01(c.y) * 255 + 0.5f);
        p[2] = (unsigned char)(clamp01(c.z) * 255 + 0.5f);
    }
};

void phongTriangle(Image& img, std::vector<float>& zb, const Cam& cam,
                   V3 P0, V3 P1, V3 P2, V3 N0, V3 N1, V3 N2, const Surface& surf)
{
    float x[3], y[3], iw[3];
    if (!project(cam, P0, &x[0], &y[0], &iw[0]) || !project(cam, P1, &x[1], &y[1], &iw[1]) ||
        !project(cam, P2, &x[2], &y[2], &iw[2])) return;

    float area = (x[1] - x[0]) * (y[2] - y[0]) - (x[2] - x[0]) * (y[1] - y[0]);
    if (fabsf(area) < 1e-6f) return;

    int minx = (int)floorf(fminf(x[0], fminf(x[1], x[2]))), maxx = (int)ceilf(fmaxf(x[0], fmaxf(x[1], x[2])));
    int miny = (int)floorf(fminf(y[0], fminf(y[1], y[2]))), maxy = (int)ceilf(fmaxf(y[0], fmaxf(y[1], y[2])));
    if (minx < 0) minx = 0;
    if (miny < 0) miny = 0;
    if (maxx > img.w - 1) maxx = img.w - 1;
    if (maxy > img.h - 1) maxy = img.h - 1;

    const bool allLit[2] = { true, true };

    for (int j = miny; j <= maxy; j++)
        for (int i = minx; i <= maxx; i++)
        {
            float px = i + 0.5f, py = j + 0.5f;
            float l0 = ((x[1] - px) * (y[2] - py) - (x[2] - px) * (y[1] - py)) / area;
            float l1 = ((x[2] - px) * (y[0] - py) - (x[0] - px) * (y[2] - py)) / area;
            float l2 = 1.0f - l0 - l1;
            if (l0 < -1e-4f || l1 < -1e-4f || l2 < -1e-4f) continue;

            float invW = l0 * iw[0] + l1 * iw[1] + l2 * iw[2];
            if (invW <= zb[(size_t)j * img.w + i]) continue;
            zb[(size_t)j * img.w + i] = invW;

            float w0 = l0 * iw[0], w1 = l1 * iw[1], w2 = l2 * iw[2];
            float ws = w0 + w1 + w2;
            V3 P = (P0 * w0 + P1 * w1 + P2 * w2) * (1.0f / ws);
            V3 N = normalize((N0 * w0 + N1 * w1 + N2 * w2) * (1.0f / ws));

            img.set(i, j, illuminate(P, N, cam.eye, surf, allLit));
        }
}

void renderPhong(int scene, Image& img)
{
    std::vector<float> zb((size_t)img.w * img.h, 0.0f);
    for (int j = 0; j < img.h; j++)
        for (int i = 0; i < img.w; i++) img.set(i, j, BACKGROUND);
    Cam cam = makeCam(img.w, img.h);

    V3 up(0, 1, 0);
    for (int i = 0; i < FLOOR_N; i++)
        for (int j = 0; j < FLOOR_N; j++)
        {
            float x0 = -FLOOR_H + i, z0 = -FLOOR_H + j;
            V3 a(x0, FLOOR_Y, z0), b(x0, FLOOR_Y, z0 + 1), c(x0 + 1, FLOOR_Y, z0 + 1), d(x0 + 1, FLOOR_Y, z0);
            phongTriangle(img, zb, cam, a, b, c, up, up, up, floorSurface(i, j));
            phongTriangle(img, zb, cam, a, c, d, up, up, up, floorSurface(i, j));
        }

    CubeObj cube = makeCube(scene);
    for (int f = 0; f < 6; f++)
    {
        V3 a = cube.v[CUBE_FACE[f][0]], b = cube.v[CUBE_FACE[f][1]], c = cube.v[CUBE_FACE[f][2]], d = cube.v[CUBE_FACE[f][3]];
        V3 n = cube.fn[f];
        phongTriangle(img, zb, cam, a, b, c, n, n, n, CUBE_SURF);
        phongTriangle(img, zb, cam, a, c, d, n, n, n, CUBE_SURF);
    }
}

bool hitCube(const CubeObj& cube, V3 O, V3 D, float tMax, float* tHit, V3* nHit)
{
    V3 o = mPoint(cube.Minv, O), d = mDir(cube.Minv, D);
    float o_[3] = { o.x, o.y, o.z }, d_[3] = { d.x, d.y, d.z };
    float t0 = 1e-3f, t1 = tMax;
    int axis = -1; float sign = 0;
    for (int a = 0; a < 3; a++)
    {
        if (fabsf(d_[a]) < 1e-9f)
        {
            if (o_[a] < 0 || o_[a] > 1) return false;
            continue;
        }
        float ta = (0 - o_[a]) / d_[a], tb = (1 - o_[a]) / d_[a];
        float sa = -1;
        if (ta > tb) { float t = ta; ta = tb; tb = t; sa = 1; }
        if (ta > t0) { t0 = ta; axis = a; sign = sa; }
        if (tb < t1) t1 = tb;
        if (t0 > t1) return false;
    }
    if (axis < 0) return false;
    *tHit = t0;
    float nl[3] = { 0, 0, 0 }; nl[axis] = sign;
    const float* m = cube.Minv.m;
    V3 n(m[0] * nl[0] + m[1] * nl[1] + m[2] * nl[2],
         m[4] * nl[0] + m[5] * nl[1] + m[6] * nl[2],
         m[8] * nl[0] + m[9] * nl[1] + m[10] * nl[2]);
    *nHit = normalize(n);
    return true;
}

bool hitFloor(V3 O, V3 D, float tMax, float* tHit)
{
    if (D.y > -1e-6f) return false;
    float t = (FLOOR_Y - O.y) / D.y;
    if (t < 1e-3f || t > tMax) return false;
    float x = O.x + D.x * t, z = O.z + D.z * t;
    if (fabsf(x) > FLOOR_H || fabsf(z) > FLOOR_H) return false;
    *tHit = t;
    return true;
}

V3 traceRay(const CubeObj& cube, V3 O, V3 D, int depth)
{
    const float INF = 1e30f;
    float tc = INF, tf = INF;
    V3 nc;
    bool hc = hitCube(cube, O, D, INF, &tc, &nc);
    bool hf = hitFloor(O, D, INF, &tf);
    if (!hc && !hf) return BACKGROUND;

    V3 P, N;
    Surface surf;
    if (hc && (!hf || tc < tf))
    {
        P = O + D * tc; N = nc; surf = CUBE_SURF;
    }
    else
    {
        P = O + D * tf; N = V3(0, 1, 0);
        int i = (int)floorf(P.x + FLOOR_H), j = (int)floorf(P.z + FLOOR_H);
        if (i > FLOOR_N - 1) i = FLOOR_N - 1;
        if (j > FLOOR_N - 1) j = FLOOR_N - 1;
        surf = floorSurface(i, j);
    }

    bool lit[2];
    V3 start = P + N * 1e-3f;
    for (int i = 0; i < 2; i++)
    {
        V3 toL = LIGHTS[i].pos - start;
        float dist = len(toL), t; V3 n;
        lit[i] = !hitCube(cube, start, toL * (1.0f / dist), dist, &t, &n);
    }

    V3 c = illuminate(P, N, O, surf, lit);

    if (depth < 3 && surf.refl > 0)
    {
        V3 R = D - N * (2.0f * dot(D, N));
        V3 rc = traceRay(cube, start, normalize(R), depth + 1);
        c = c * (1.0f - surf.refl) + rc * surf.refl;
    }
    return c;
}

void renderRayTrace(int scene, Image& img)
{
    Cam cam = makeCam(img.w, img.h);
    CubeObj cube = makeCube(scene);
    const int SS = 2;

    for (int j = 0; j < img.h; j++)
        for (int i = 0; i < img.w; i++)
        {
            V3 sum(0, 0, 0);
            for (int sy = 0; sy < SS; sy++)
                for (int sx = 0; sx < SS; sx++)
                {
                    float nx = ((i + (sx + 0.5f) / SS) / img.w) * 2 - 1;
                    float ny = ((j + (sy + 0.5f) / SS) / img.h) * 2 - 1;
                    V3 D = normalize(cam.f + cam.r * (nx * cam.tanH * cam.aspect) + cam.u * (ny * cam.tanH));
                    sum = sum + traceRay(cube, cam.eye, D, 0);
                }
            img.set(i, j, sum * (1.0f / (SS * SS)));
        }
}

const int HEADER_H = 50, FOOTER_H = 26;
int winW = 1200, winH = 790;

Image phongImg[NUM_SCENES], rtImg[NUM_SCENES];
bool  needRender = true;

void cellRect(int row, int col, int* x, int* y, int* w, int* h)
{
    *w = winW / 4;
    *h = (winH - HEADER_H - FOOTER_H) / 3;
    *x = col * (*w);
    *y = FOOTER_H + (2 - row) * (*h);
}

void renderSoftware()
{
    int x, y, w, h;
    cellRect(0, 0, &x, &y, &w, &h);
    for (int s = 0; s < NUM_SCENES; s++)
    {
        phongImg[s].init(w, h);
        rtImg[s].init(w, h);
        renderPhong(s, phongImg[s]);
        renderRayTrace(s, rtImg[s]);
    }
    needRender = false;
}

void drawUnitCube()
{
    glBegin(GL_QUADS);
        glNormal3f( 1, 0, 0); glVertex3f(1,0,0); glVertex3f(1,1,0); glVertex3f(1,1,1); glVertex3f(1,0,1);
        glNormal3f(-1, 0, 0); glVertex3f(0,0,0); glVertex3f(0,0,1); glVertex3f(0,1,1); glVertex3f(0,1,0);
        glNormal3f( 0, 1, 0); glVertex3f(0,1,0); glVertex3f(0,1,1); glVertex3f(1,1,1); glVertex3f(1,1,0);
        glNormal3f( 0,-1, 0); glVertex3f(0,0,0); glVertex3f(1,0,0); glVertex3f(1,0,1); glVertex3f(0,0,1);
        glNormal3f( 0, 0, 1); glVertex3f(0,0,1); glVertex3f(1,0,1); glVertex3f(1,1,1); glVertex3f(0,1,1);
        glNormal3f( 0, 0,-1); glVertex3f(0,0,0); glVertex3f(0,1,0); glVertex3f(1,1,0); glVertex3f(1,0,0);
    glEnd();
}

void setMaterial(const Surface& s)
{
    glColor3f(s.color.x, s.color.y, s.color.z);
    GLfloat spec[] = { s.ks.x, s.ks.y, s.ks.z, 1 };
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, spec);
    glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, s.shin);
}

void drawFloorGL()
{
    glNormal3f(0, 1, 0);
    for (int i = 0; i < FLOOR_N; i++)
        for (int j = 0; j < FLOOR_N; j++)
        {
            float x0 = -FLOOR_H + i, z0 = -FLOOR_H + j;
            setMaterial(floorSurface(i, j));
            glBegin(GL_QUADS);
                glNormal3f(0, 1, 0);
                glVertex3f(x0, FLOOR_Y, z0);
                glVertex3f(x0, FLOOR_Y, z0 + 1);
                glVertex3f(x0 + 1, FLOOR_Y, z0 + 1);
                glVertex3f(x0 + 1, FLOOR_Y, z0);
            glEnd();
        }
}

void setupCameraGL(int w, int h)
{
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(FOVY, (double)w / h, 0.5, 100);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    float az = camAz * PI / 180, el = camEl * PI / 180;
    gluLookAt(TARGET[0] + CAM_DIST * cosf(el) * sinf(az),
              TARGET[1] + CAM_DIST * sinf(el),
              TARGET[2] + CAM_DIST * cosf(el) * cosf(az),
              TARGET[0], TARGET[1], TARGET[2], 0, 1, 0);

    GLfloat p0[] = { LIGHTS[0].pos.x, LIGHTS[0].pos.y, LIGHTS[0].pos.z, 1 };
    GLfloat p1[] = { LIGHTS[1].pos.x, LIGHTS[1].pos.y, LIGHTS[1].pos.z, 1 };
    glLightfv(GL_LIGHT0, GL_POSITION, p0);
    glLightfv(GL_LIGHT1, GL_POSITION, p1);
}

void drawOpenGLCell(int scene, bool smooth, int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
    glClear(GL_DEPTH_BUFFER_BIT);
    setupCameraGL(w, h);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glShadeModel(smooth ? GL_SMOOTH : GL_FLAT);

    drawFloorGL();

    setMaterial(CUBE_SURF);
    glPushMatrix();
    glMultMatrixf(sceneM[scene].m);
    drawUnitCube();
    glPopMatrix();
}

void beginOverlay(int x, int y, int w, int h)
{
    glViewport(x, y, w, h);
    glDisable(GL_LIGHTING);
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, w, 0, h);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void textAt(float x, float y, const char* s, void* font, bool centred)
{
    float w = (float)glutBitmapLength(font, (const unsigned char*)s);
    glRasterPos2f(centred ? x - w / 2 : x, y);
    for (; *s; s++) glutBitmapCharacter(font, *s);
}

void drawImageCell(const Image& img, int x, int y, int w, int h)
{
    beginOverlay(x, y, w, h);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glRasterPos2i(0, 0);
    glDrawPixels(img.w, img.h, GL_RGB, GL_UNSIGNED_BYTE, &img.px[0]);
}

void drawCellDecor(int row, int col, int x, int y, int w, int h)
{
    beginOverlay(x, y, w, h);
    glColor3f(0.7f, 0.7f, 0.7f);
    glLineWidth(1.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.5f, 0.5f); glVertex2f(w - 0.5f, 0.5f); glVertex2f(w - 0.5f, h - 0.5f); glVertex2f(0.5f, h - 0.5f);
    glEnd();

    glColor3f(0.1f, 0.1f, 0.1f);
    textAt(6, (float)h - 16, sceneName[row], GLUT_BITMAP_HELVETICA_12, false);
    if (col == 0)
    {
        glColor3f(0.15f, 0.25f, 0.6f);
        textAt(6, (float)h - 31, sceneFormula[row], GLUT_BITMAP_HELVETICA_10, false);
    }
}

void drawHeaderFooter()
{
    const char* title[4] = { "Flat shading", "Gouraud shading", "Phong shading", "Ray tracing" };
    const char* sub[4]   = { "glShadeModel(GL_FLAT) : one colour per polygon",
                             "glShadeModel(GL_SMOOTH) : per-vertex lighting",
                             "per-pixel lighting (software rasteriser)",
                             "shadows + reflections (software ray tracer)" };

    beginOverlay(0, winH - HEADER_H, winW, HEADER_H);
    for (int c = 0; c < 4; c++)
    {
        float cx = (c + 0.5f) * (winW / 4);
        glColor3f(0.05f, 0.05f, 0.05f);
        textAt(cx, (float)HEADER_H - 22, title[c], GLUT_BITMAP_HELVETICA_18, true);
        glColor3f(0.35f, 0.35f, 0.35f);
        textAt(cx, 8, sub[c], GLUT_BITMAP_HELVETICA_10, true);
    }

    beginOverlay(0, 0, winW, FOOTER_H);
    glColor3f(0.2f, 0.2f, 0.2f);
    textAt(winW / 2.0f, 9,
           "Same camera, lights and material in every cell.   Arrow keys: orbit camera    R: reset view    ESC: quit",
           GLUT_BITMAP_HELVETICA_12, true);
}

void display()
{
    if (needRender) renderSoftware();

    glViewport(0, 0, winW, winH);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (int row = 0; row < NUM_SCENES; row++)
        for (int col = 0; col < 4; col++)
        {
            int x, y, w, h;
            cellRect(row, col, &x, &y, &w, &h);
            if (col == 0)      drawOpenGLCell(row, false, x, y, w, h);
            else if (col == 1) drawOpenGLCell(row, true,  x, y, w, h);
            else if (col == 2) drawImageCell(phongImg[row], x, y, w, h);
            else               drawImageCell(rtImg[row], x, y, w, h);
            drawCellDecor(row, col, x, y, w, h);
        }

    drawHeaderFooter();
    glutSwapBuffers();
}

void init()
{
    glClearColor(BACKGROUND.x, BACKGROUND.y, BACKGROUND.z, 1);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glEnable(GL_NORMALIZE);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

    for (int i = 0; i < 2; i++)
    {
        GLfloat a[] = { LIGHTS[i].amb.x,  LIGHTS[i].amb.y,  LIGHTS[i].amb.z,  1 };
        GLfloat d[] = { LIGHTS[i].dif.x,  LIGHTS[i].dif.y,  LIGHTS[i].dif.z,  1 };
        GLfloat s[] = { LIGHTS[i].spec.x, LIGHTS[i].spec.y, LIGHTS[i].spec.z, 1 };
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, a);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, d);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, s);
    }
    GLfloat ga[] = { GLOBAL_AMB.x, GLOBAL_AMB.y, GLOBAL_AMB.z, 1 };
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ga);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    buildScenes();
}

void reshape(int w, int h)
{
    winW = w;
    winH = (h > 0) ? h : 1;
    needRender = true;
}

void keyboard(unsigned char key, int x, int y)
{
    if (key == 27) exit(0);
    if (key == 'r' || key == 'R') { camAz = DEF_AZ; camEl = DEF_EL; needRender = true; }
    glutPostRedisplay();
}

void special(int key, int x, int y)
{
    if (key == GLUT_KEY_LEFT)  camAz -= 5;
    if (key == GLUT_KEY_RIGHT) camAz += 5;
    if (key == GLUT_KEY_UP)    camEl += 5;
    if (key == GLUT_KEY_DOWN)  camEl -= 5;
    if (camEl > 85) camEl = 85;
    if (camEl < 10) camEl = 10;
    needRender = true;
    glutPostRedisplay();
}

int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(winW, winH);
    glutCreateWindow("Lab 10 - Assignment 04 : Flat / Gouraud / Phong / Ray Tracing");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}
