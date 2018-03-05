#include <cstdio>

#include <GL/glew.h>
#include <GL/glut.h>

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutCreateWindow("GLEW test");
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
    }
    fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
}
