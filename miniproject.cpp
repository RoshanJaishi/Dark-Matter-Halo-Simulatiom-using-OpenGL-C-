#include <GL/glut.h>
#include <cmath>

const int numParticles = 1000;
const float G = 6.67430e-11;  // Gravitational constant

float cameraZ = 5.0f;
int width = 800;
int height = 600;

struct Particle {
    float x, y, z;
    float vx, vy, vz;
    float mass;
    float r, g, b;
};

Particle particles[numParticles];

void initializeParticles() {
    for (int i = 0; i < numParticles; i++) {
        particles[i].x = 2.0f * rand() / RAND_MAX - 1.0f;
        particles[i].y = 2.0f * rand() / RAND_MAX - 1.0f;
        particles[i].z = 2.0f * rand() / RAND_MAX - 1.0f;
        particles[i].vx = 0.1f * rand() / RAND_MAX - 0.05f;
        particles[i].vy = 0.1f * rand() / RAND_MAX - 0.05f;
        particles[i].vz = 0.1f * rand() / RAND_MAX - 0.05f;
        particles[i].mass = 1.0f * rand() / RAND_MAX;
        particles[i].r = 1.0f * rand() / RAND_MAX;
        particles[i].g = 1.0f * rand() / RAND_MAX;
        particles[i].b = 1.0f * rand() / RAND_MAX;
    }
}

void calculateGravitationalForce(Particle& p1, Particle& p2, float& fx, float& fy, float& fz) {
    float dx = p2.x - p1.x;
    float dy = p2.y - p1.y;
    float dz = p2.z - p1.z;

    float r2 = dx * dx + dy * dy + dz * dz;
    float r = sqrt(r2);

    float F = (G * p1.mass * p2.mass) / (r2 * r);

    fx += F * dx / r;
    fy += F * dy / r;
    fz += F * dz / r;
}

void updateParticles(float dt) {
    float fx, fy, fz;

    for (int i = 0; i < numParticles; i++) {
        fx = fy = fz = 0.0f;

        for (int j = 0; j < numParticles; j++) {
            if (i != j) {
                calculateGravitationalForce(particles[i], particles[j], fx, fy, fz);
            }
        }

        particles[i].vx += fx * dt / particles[i].mass;
        particles[i].vy += fy * dt / particles[i].mass;
        particles[i].vz += fz * dt / particles[i].mass;

        particles[i].x += particles[i].vx * dt;
        particles[i].y += particles[i].vy * dt;
        particles[i].z += particles[i].vz * dt;
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0f, 0.0f, cameraZ, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
    glEnable(GL_POINT_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glBegin(GL_POINTS);
    for (int i = 0; i < numParticles; i++) {
        glColor3f(particles[i].r, particles[i].g, particles[i].b);
        glPointSize(5.0f);
        glVertex3f(particles[i].x, particles[i].y, particles[i].z);
    }
    glEnd();
    glutSwapBuffers();
}

void idle() {
    static int lastTime = glutGet(GLUT_ELAPSED_TIME);
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float dt = 0.001f * (currentTime - lastTime);
    updateParticles(dt);
    lastTime = currentTime;
    glutPostRedisplay();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
    glMatrixMode(GL_MODELVIEW);
    width = w;
    height = h;
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) {
        exit(0);
    }
}

void mouse(int button, int state, int x, int y) {
    if (button == 3) {
        // scroll up, zoom in
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float fovy = 45.0f - 2.0f;
        gluPerspective(fovy, (float)width / (float)height, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        cameraZ -= 0.1f;
    } else if (button == 4) {
        // scroll down, zoom out
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        float fovy = 45.0f + 2.0f;
        gluPerspective(fovy, (float)width / (float)height, 0.1f, 100.0f);
        glMatrixMode(GL_MODELVIEW);
        cameraZ += 0.1f;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(width, height);
    glutCreateWindow("Dark matter halo Simulation");
    glutDisplayFunc(display);
    glutIdleFunc(idle);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMouseFunc(mouse);
    initializeParticles();
    glEnable(GL_DEPTH_TEST);
    glutMainLoop();
    return 0;
}
