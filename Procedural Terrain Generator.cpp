#include<iostream>
#include <vector>
#include <cmath>
#include <GL/glut.h>

std::vector< std::vector<GLfloat>  > t;
std::vector< std::vector< std::vector<GLfloat>  >  > c, n;
GLfloat seed, highest, unused, camY, dY, theta, dTheta, camDist;
int size, displayListId;
bool p, l, o;

GLfloat Gradient1[17][3] = {8.0 / 255.0,5.0 / 255.0,65.0 / 255.0,
    28.0 / 255.0,17.0 / 255.0,80.0 / 255.0,
    9.0 / 255.0,9.0 / 255.0,121.0 / 255.0,
    5.0 / 255.0,90.0 / 255.0,174.0 / 255.0,
    3.0 / 255.0,144.0 / 255.0,210.0 / 255.0,
    0.0,212.0 / 255.0,255.0 / 255.0,
    135.0 / 255.0,117.0 / 255.0,43.0 / 255.0,
    184.0 / 255.0,140.0 / 255.0,48.0 / 255.0,
    108.0 / 255.0,191.0 / 255.0,50.0 / 255.0,
    5.0 / 255.0,126.0 / 255.0,11.0 / 255.0,
    95.0 / 255.0,170.0 / 255.0,8.0 / 255.0,
    9.0 / 255.0,110.0 / 255.0,23.0 / 255.0,
    140.0 / 255.0, 145.0 / 255.0, 33.0 / 255.0,
    145.0 / 255.0,63.0 / 255.0,33.0 / 255.0,
    171.0 / 255.0,121.0 / 255.0,72.0 / 255.0,
    133.0 / 255.0,115.0 / 255.0,98.0 / 255.0,
    1.0,1.0,1.0 };

GLfloat scale(GLfloat x) { return x * ((((GLfloat)rand() / RAND_MAX) * 0.25) - 0.1); }

void calcTerrain(int x1, int y1, int x2, int y2) {
    int x3 = (x1 + x2) / 2;
    int y3 = (y1 + y2) / 2;
    if (y3 < y2) {
        if (t[x1][y3] == unused) {
            t[x1][y3] = (t[x1][y1] + t[x1][y2]) / 2 + scale(seed * (y2 - y1));
        }
        t[x2][y3] = (t[x2][y1] + t[x2][y2]) / 2 + scale(seed * (y2 - y1));
    }
    if (x3 < x2) {
        if (t[x3][y1] == unused) {
            t[x3][y1] = (t[x1][y1] + t[x2][y1]) / 2 + scale(seed * (x2 - x1));
        }
        t[x3][y2] = (t[x1][y2] + t[x2][y2]) / 2 + scale(seed * (x2 - x1));
    }
    if (x3 < x2 && y3 < y2) {
        t[x3][y3] = (t[x1][y1] + t[x2][y1] + t[x1][y2] + t[x2][y2]) / 4
            + scale(seed * (fabs((GLfloat)(x2 - x1)) + fabs((GLfloat)(y2 - y1))));
    }
    if (x3 < x2 - 1 || y3 < y2 - 1) {
        calcTerrain(x1, y1, x3, y3);
        calcTerrain(x1, y3, x3, y2);
        calcTerrain(x3, y1, x2, y3);
        calcTerrain(x3, y3, x2, y2);
    }
}

void calcNormal(GLfloat i, GLfloat j) {
    
    GLfloat p11[3] = { i + 1, t[i + 1][j + 1], j + 1 };
    GLfloat p12[3] = { i, t[i][j + 1], j + 1 };
    GLfloat p13[3] = { i, t[i][j], j };
    GLfloat u[3] = { (p12[0] - p11[0]),(p12[1] - p11[1]) ,(p12[2] - p11[2]) };
    GLfloat v[3] = { (p13[0] - p11[0]),(p13[1] - p11[1]) ,(p13[2] - p11[2]) };
    n[i][j][0] = (u[1] * v[2]) - (u[2] * v[1]);
    n[i][j][1] = (u[2] * v[0]) - (u[0] * v[2]);
    n[i][j][2] = (u[0] * v[1]) - (u[1] * v[0]);
    
    GLfloat p21[3] = { i,t[i][j],j };
    GLfloat p22[3] = { i + 1,t[i + 1][j],j };
    GLfloat p23[3] = { i + 1,t[i + 1][j + 1],j + 1 };
    GLfloat p[3] = { (p22[0] - p21[0]),(p22[1] - p21[1]) ,(p22[2] - p21[2]) };
    GLfloat q[3] = { (p23[0] - p21[0]),(p23[1] - p21[1]) ,(p23[2] - p21[2]) };
    n[i][j][3] = (p[1] * q[2]) - (p[2] * q[1]);
    n[i][j][4] = (p[2] * q[0]) - (p[0] * q[2]);
    n[i][j][5] = (p[0] * q[1]) - (p[1] * q[0]);

    for (int p = 0; i < 2; i++) {
        for (int q = 0; j < 3; j++) {
            if (n[i][j][p + q] < 0.0)n[i][j][p + q] *= -1;
            n[i][j][p + q] = (n[i][j][p + q] / sqrt((n[i][j][p + 0] * n[i][j][p + 0]) + (n[i][j][p + 1] * n[i][j][p + 1]) + (n[i][j][p + 2] * n[i][j][p + 2])));
        }
    }
}

void initNormal() {

    std::vector<GLfloat> nullRow(6, unused);
    std::vector< std::vector<GLfloat> > nullMatrix(size, nullRow);
    std::vector< std::vector< std::vector<GLfloat>  >  >nullCube(size, nullMatrix);
    n = nullCube;
    
    for (GLfloat i = 0.0; i < size - 1; i++) {
        for (GLfloat j = 0.0; j < size - 1; j++) {
            calcNormal(i, j);
        }
    }
}

void calcColor(GLfloat h, GLfloat a, GLfloat b, int u, int l, int i, int j) {
    GLfloat d = fabs(a - b);
    for (int m = 0; m < 3; m++)
        c[i][j][m] = Gradient1[u][m] * (fabs((h - a) / d)) + Gradient1[l][m] * (fabs((h - b) / d));
}

void initColor() {

    std::vector<GLfloat> nullRow(3, unused);
    std::vector< std::vector<GLfloat> > nullMatrix(size, nullRow);
    std::vector< std::vector< std::vector<GLfloat>  >  >nullCube(size, nullMatrix);
    c = nullCube;
    
    GLfloat y, h;
    for (int i = 0; i < size - 1; i++) {
        for (int j = 0; j < size - 1; j++) {
            y = t[i][j];
            h = y / highest;
            (h < -0.9) ? calcColor(h, -0.9, -1.0, 0, 1, i, j) :
                (h < -0.7) ? calcColor(h, -0.7, -0.9, 1, 2, i, j) :
                (h < -0.5) ? calcColor(h, -0.5, -0.7, 2, 3, i, j) :
                (h < -0.3) ? calcColor(h, -0.3, -0.5, 3, 4, i, j) :
                (h < -0.1) ? calcColor(h, -0.1, -0.3, 4, 5, i, j) :
                (h <= 0.0) ? calcColor(h, 0.0, -0.1, 5, 6, i, j) :
                (h < 0.1) ? calcColor(h, 0.1, 0.0, 6, 7, i, j) :
                (h < 0.2) ? calcColor(h, 0.2, 0.1, 7, 8, i, j) :
                (h < 0.3) ? calcColor(h, 0.3, 0.2, 8, 9, i, j) :
                (h < 0.4) ? calcColor(h, 0.4, 0.3, 9, 10, i, j) :
                (h < 0.5) ? calcColor(h, 0.5, 0.4, 10, 11, i, j) :
                (h < 0.6) ? calcColor(h, 0.6, 0.5, 11, 12, i, j) :
                (h < 0.7) ? calcColor(h, 0.7, 0.6, 12, 13, i, j) :
                (h < 0.8) ? calcColor(h, 0.8, 0.7, 13, 14, i, j) :
                (h < 0.9) ? calcColor(h, 0.9, 0.8, 14, 15, i, j) :
                calcColor(h, 1.0, 0.9, 15, 16, i, j);
        }
    }
}

void createDisplayList() {
    glNewList(displayListId, GL_COMPILE);
    glEnable(GL_LIGHTING);
    glBegin(GL_TRIANGLES);
    for (int x = 0; x < size - 1; x++) {
        for (int z = 0; z < size - 1; z++) {
    
            glNormal3f(n[x][z][0], n[x][z][1], n[x][z][2]);
            glColor3f(c[x][z][0], c[x][z][1], c[x][z][2]);
            glVertex3f(x, t[x][z], z);
            glColor3f(c[x][z + 1][0], c[x][z + 1][1], c[x][z + 1][2]);
            glVertex3f(x, t[x][z + 1], z + 1);
            glColor3f(c[x + 1][z + 1][0], c[x + 1][z + 1][1], c[x + 1][z + 1][2]);
            glVertex3f(x + 1, t[x + 1][z + 1], z + 1);

            glNormal3f(n[x][z][3], n[x][z][4], n[x][z][5]);
            glColor3f(c[x][z][0], c[x][z][1], c[x][z][2]);
            glVertex3f(x, t[x][z], z);
            glColor3f(c[x + 1][z][0], c[x + 1][z][1], c[x + 1][z][2]);
            glVertex3f(x + 1, t[x + 1][z], z);
            glColor3f(c[x + 1][z + 1][0], c[x + 1][z + 1][1], c[x + 1][z + 1][2]);
            glVertex3f(x + 1, t[x + 1][z + 1], z + 1);
        }
    }
    glEnd();
    glEndList();
}

void initTerrain() {
    int r, c;
    highest = 0.0;
    std::vector<GLfloat> nullRow(size, unused);
    std::vector< std::vector<GLfloat> > nullMatrix(size, nullRow);
    t = nullMatrix;
    
    for (r = 0; r < size; r++) t[r][0] = t[r][size - 1] = 0;
    for (c = 0; c < size; c++) t[0][c] = t[size - 1][c] = 0;
    t[1][1] = t[1][size - 2] = t[size - 2][1] = t[size - 2][size - 2] = 0;
    calcTerrain(1, 1, size - 2, size - 2);
    for (int i = 0; i < size; i++) for (int j = 0; j < size; j++) if (t[i][j] > highest) highest = t[i][j];
    
    initNormal();
    initColor();
    for (int i = 0; i < size; i++) for (int j = 0; j < size; j++) if (t[i][j] < -highest) t[i][j] *=0.9;
    for (int i = 0; i < size; i++) for (int j = 0; j < size; j++) if (t[i][j] < 0.0) t[i][j] *= 0.2;
    displayListId = glGenLists(1);
    createDisplayList();
}

void init() {
    srand(9913);
    glEnable(GL_DEPTH_TEST);

    unused = -9999.9999;
    size = 256;
    dY = 1.0;
    dTheta = 0.01;
    camY = size / 4;
    theta = 0;
    camDist = size;
    seed = ((GLfloat)rand()) / RAND_MAX;
    p = l = 0;
    o = 1;
    initTerrain();

    GLfloat dark[] = { 0.2, 0.2, 0.2, 1.0 };
    GLfloat light[] = { 0.4, 0.4, 0.4, 1.0 };
    GLfloat direction[] = { 0.0, -1.0, 0.0, 0.0 };

    glMaterialfv(GL_FRONT, GL_SPECULAR, light);
    glMaterialf(GL_FRONT, GL_SHININESS, 5);
    glEnable(GL_COLOR_MATERIAL);

    glLightfv(GL_LIGHT0, GL_AMBIENT, dark);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light);
    glLightfv(GL_LIGHT0, GL_SPECULAR, light);
    glLightfv(GL_LIGHT0, GL_POSITION, direction);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
}

void writeHelpToConsole() {
    std::cout << "\n Controls:\n\n";
    std::cout << " W - Zoom in\n";
    std::cout << " S - Zoom out\n";
    std::cout << " A - Rotate Left\n";
    std::cout << " D - Rotate Right\n";
    std::cout << " Z - Move up\n";
    std::cout << " X - Move down\n";
    std::cout << " Q - Generate new terrain of smaller size\n";
    std::cout << " E - Generate new terrain of larger size\n";
    std::cout << " R - Generate new terrain of same size\n";
    std::cout << " P - Display terrain points\n";
    std::cout << " L - Display terrain lines\n";
    std::cout << " O - Display terrain faces\n";
    std::cout << "\n Press Escape key to exit\n";
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (GLfloat)w / (GLfloat)h, 0.05, 2000.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int v) {
    glutPostRedisplay();
    glutTimerFunc(1000 / 60, timer, v);
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(camDist * cos(theta) + ((size - 1) / 2), camY, camDist * sin(theta) + ((size - 1) / 2),
        ((size - 1) / 2), 0.0, ((size - 1) / 2), 0.0, 1.0, 0.0);
    
    if (p) glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
    if (l) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    if (o) glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    glCallList(displayListId);
    glFlush();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int, int) {
    switch (key) {
    case 'd': theta -= dTheta; break;
    case 'a': theta += dTheta;; break;
    case 'w': if (camDist > (size / 4))camDist -= dY; break;
    case 's': if (camDist < (2 * size))camDist += dY; break;
    case 'z': if (camY < (2 * size)) camY += dY; break;
    case 'x': if (camY > highest) camY -= dY; break;
    case 'e': if (size < 1000) { size *= 2; camY*=2; camDist *= 2; dY *= 2;
                glDeleteLists(displayListId, 1); initTerrain(); } break;
    case 'q': if (size > 100) { size *= 0.5; camY *= 0.5; camDist *= 0.5; dY *= 0.5;
                glDeleteLists(displayListId, 1); initTerrain(); } break;
    case 'p': p = 1; l = 0; o = 0; break;
    case 'l': p = 0; l = 1; o = 0; break;
    case 'o': p = 0; l = 0; o = 1; break;
    case 'r': glDeleteLists(displayListId, 1); initTerrain(); break;
    case 27: exit(0);
    }
    glutPostRedisplay();
}

int main(int argc, char** argv) {
    writeHelpToConsole();
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowPosition(480, 270);
    glutInitWindowSize(960, 540);
    glutCreateWindow("Procedural Terrain Generator");
    glutReshapeFunc(reshape);
    glutTimerFunc(100, timer, 0);
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    init();
    glutMainLoop();
}