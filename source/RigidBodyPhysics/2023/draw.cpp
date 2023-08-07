#include "draw.h"

#include <Helpers.h>
#include <GLFW/glfw3.h>
#include <GL/glu.h>

namespace draw
{
    using namespace rigidbody;

    static GLFWwindow* GLwindow;

    void reshape(int width, int height) {
        glViewport(0, 0, width, height);
        glMatrixMode(GL_PROJECTION);
        glLoadIdentity();
        gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
    }

    void Init()
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

        glfwSetFramebufferSizeCallback(GLwindow, (GLFWframebuffersizefun)reshape);
    }

    void End()
    {
        glfwDestroyWindow(GLwindow);
        glfwTerminate();
    }

    void drawCube(const f angle, const f3& axis, rigidbody::SimulationContext const& context)
    {
        GLfloat x = context.lengths[0] * 0.5f;
        GLfloat y = context.lengths[1] * 0.5f;
        GLfloat z = context.lengths[2] * 0.5f;

        const f3x3 rotMat = matrixFromAxisAngle(angle, axis);

        f3 v[] =
        {
            f3(x,  y,  z), f3(x, -y,  z), f3(x,  -y,  -z), f3(x,  y,  -z),
            f3(-x,  y,  z), f3(-x,  -y,  z), f3(-x,  y, -z), f3(-x,  -y,  -z)
        };

        for (f3& vec : v)
        {
            vec = rotMat * vec;
        }

        //init with initial pos
        GLfloat vertices[] =
        {
            v[7][0], v[7][1], v[7][2],    v[5][0], v[5][1], v[5][2],    v[4][0], v[4][1], v[4][2],    v[6][0], v[6][1], v[6][2],
            v[2][0], v[2][1], v[2][2],    v[1][0], v[1][1], v[1][2],    v[0][0], v[0][1], v[0][2],    v[3][0], v[3][1], v[3][2],
            v[7][0], v[7][1], v[7][2],    v[5][0], v[5][1], v[5][2],    v[1][0], v[1][1], v[1][2],    v[2][0], v[2][1], v[2][2],
            v[6][0], v[6][1], v[6][2],    v[4][0], v[4][1], v[4][2],    v[0][0], v[0][1], v[0][2],    v[3][0], v[3][1], v[3][2],
            v[7][0], v[7][1], v[7][2],    v[6][0], v[6][1], v[6][2],    v[3][0], v[3][1], v[3][2],    v[2][0], v[2][1], v[2][2],
            v[5][0], v[5][1], v[5][2],    v[4][0], v[4][1], v[4][2],    v[0][0], v[0][1], v[0][2],    v[1][0], v[1][1], v[1][2]
        };

        GLfloat colors[] =
        {
            1, 0, 0,    1, 0, 0,    1, 0, 0,    1, 0, 0,
            1, 1, 0,    1, 1, 0,    1, 1, 0,    1, 1, 0,
            0, 1, 1,    0, 1, 1,    0, 1, 1,    0, 1, 1,
            0, 1, 0,    0, 1, 0,    0, 1, 0,    0, 1, 0,
            1, 0, 1,    1, 0, 1,    1, 0, 1,    1, 0, 1,
            1, 1, 1,    1, 1, 1,    1, 1, 1,    1, 1, 1
        };

        //glRotatef(angle, axis.x, axis.y, axis.z);

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
    }

    void drawLine(const f3& direction, const f3& origin) {
        glDisable(GL_DEPTH_TEST);
        glBegin(GL_LINES);
        glVertex3f(origin[0], origin[1], origin[2]); // Start point of the line
        glVertex3f(direction[0], direction[1], direction[2]);   // End point of the line
        glEnd();
        glEnable(GL_DEPTH_TEST);
    }

    void display(const SimulationContext& context, const f angle, const f3& axis) {

        GLint windowX, windowY;
        glfwGetWindowSize(GLwindow, &windowX, &windowY);
        glViewport(0, 0, windowX, windowY);

        // Draw stuff
        glClearColor(0.f, 0.f, 0.4f, 1.f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glMatrixMode(GL_PROJECTION_MATRIX);
        glLoadIdentity();
        gluPerspective(60, (double)windowX / (double)windowY, 0.1, 100);

        glMatrixMode(GL_MODELVIEW_MATRIX);
        glTranslatef(0, 0, -10);
        glRotatef(20, 0, 1, 0);
        // Draw the cube
        drawCube(angle, axis, context);

        glColor3f(0.0f, 1.0f, 0.0f);
        drawLine(context.initial_impulse_application_point - context.initial_impulse, context.initial_impulse_application_point);

        const f3x3 rotMat = matrixFromAxisAngle(angle, axis).transpose();

        glColor3f(1.0f, 1.0f, 0.0f);
        drawLine(rotMat[0] * (context.lengths[0] + 1.f));

        glColor3f(1.0f, 0.0f, 0.0f);
        drawLine(axis * 5.f);

        glfwSwapBuffers(GLwindow);
    }
}