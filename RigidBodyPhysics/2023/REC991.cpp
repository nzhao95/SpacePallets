#include <Helpers.h>
#include "REC991.h"
#include <GLFW/glfw3.h>
#include <GL/glu.h>

namespace REC991
{
static GLFWwindow* GLwindow;

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
}

void GlobalInit()
{
    /* Initialize the library */
    if (!glfwInit())
        return;

    /* Create a windowed mode window and its OpenGL context */
    GLwindow = glfwCreateWindow(640, 480, "Rigid Body", NULL, NULL);
    if (!GLwindow)
    {
        glfwTerminate();
        return;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(GLwindow);
    glEnable(GL_DEPTH_TEST);

    glfwSetFramebufferSizeCallback(GLwindow, (GLFWframebuffersizefun) reshape);
}

void GlobalTeardown()
{
    glfwDestroyWindow(GLwindow);
    glfwTerminate();
}


void drawCube(const rigidbody::f3x3& orientation, const rigidbody::f3& lengths)
{
    GLfloat x = lengths[0];
    GLfloat y = lengths[1];
    GLfloat z = lengths[2];

    auto applyOrientationToVertex = [orientation](GLfloat& x, GLfloat& y, GLfloat& z) {
        GLfloat newX = orientation[0][0] * x + orientation[0][1] * y + orientation[0][2] * z;
        GLfloat newY = orientation[1][0] * x + orientation[1][1] * y + orientation[1][2] * z;
        GLfloat newZ = orientation[2][0] * x + orientation[2][1] * y + orientation[2][2] * z;
        x = newX;
        y = newY;
        z = newZ;
    };

    //init with initial pos
    GLfloat vertices[] =
    {
        -x, -y, -z,   -x, -y,  z,   -x,  y,  z,   -x,  y, -z,
         x, -y, -z,    x, -y,  z,    x,  y,  z,    x,  y, -z,
        -x, -y, -z,   -x, -y,  z,    x, -y,  z,    x, -y, -z,
        -x,  y, -z,   -x,  y,  z,    x,  y,  z,    x,  y, -z,
        -x, -y, -z,   -x,  y, -z,    x,  y, -z,    x, -y, -z,
        -x, -y,  z,   -x,  y,  z,    x,  y,  z,    x, -y,  z
    };

    GLfloat colors[] =
    {
        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,
        1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,
        0, 1, 1,   0, 1, 1,   0, 1, 1,   0, 1, 1,
        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,
        1, 0, 1,   1, 0, 1,   1, 0, 1,   1, 0, 1,
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1
    };

    static float alpha = 0;
    //attempt to rotate cube
    glRotatef(alpha, 0, 1, 1);

    /* We have a color array and a vertex array */
    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);
    glVertexPointer(3, GL_FLOAT, 0, vertices);
    glColorPointer(3, GL_FLOAT, 0, colors);

    /* Send data : 24 vertices */
    glDrawArrays(GL_QUADS, 0, 24);

    /* Cleanup states */
    glDisableClientState(GL_COLOR_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);
    alpha += 1;
}

void display(const rigidbody::f3x3& orientation, const rigidbody::f3& lengths) {
    // Scale to window size
    GLint windowWidth, windowHeight;
    glfwGetWindowSize(GLwindow, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    // Draw stuff
    glClearColor(0.f, 0.f, 0.4f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_PROJECTION_MATRIX);
    glLoadIdentity();
    gluPerspective(60, (double)windowWidth / (double)windowHeight, 0.1, 100);

    glMatrixMode(GL_MODELVIEW_MATRIX);
    glTranslatef(0, 0, -10);
    // Draw the cube
    drawCube(orientation, lengths);

    glfwSwapBuffers(GLwindow);
}

rigidbody::f3x3 Simulate(rigidbody::SimulationContext const& context)
{
    rigidbody::f3x3 orientation = rigidbody::f3x3::id();
    orientation[0][2] = 0.5f;
    orientation[1][2] = 0.5f;

    ((void)context);

    if (GLwindow) {

        while (!glfwWindowShouldClose(GLwindow)) {
            display(orientation, context.lengths);
            glfwPollEvents();
        }
    }
    return orientation;
}

} // namespace REC991
