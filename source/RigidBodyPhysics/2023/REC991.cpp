#include <Helpers.h>
#include "REC991.h"
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cmath>
#include <thread>

namespace REC991
{
using namespace rigidbody;
static constexpr f time_step = 1.e-2f;
static constexpr f radToDeg = 180.f / M_PI;

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

    glfwSetFramebufferSizeCallback(GLwindow, (GLFWframebuffersizefun)reshape);
}

void GlobalTeardown()
{
    glfwDestroyWindow(GLwindow);
    glfwTerminate();
}

void drawCube(const quat& orientation, const rigidbody::f3& lengths)
{
    GLfloat x = lengths[0] * 0.5f;
    GLfloat y = lengths[1] * 0.5f;
    GLfloat z = lengths[2] * 0.5f;

    const f3x3 rotMat = quaternionToMatrix(orientation);

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
        1, 0, 0,   1, 0, 0,   1, 0, 0,   1, 0, 0,
        1, 1, 0,   1, 1, 0,   1, 1, 0,   1, 1, 0,
        0, 1, 1,   0, 1, 1,   0, 1, 1,   0, 1, 1,
        0, 1, 0,   0, 1, 0,   0, 1, 0,   0, 1, 0,
        1, 0, 1,   1, 0, 1,   1, 0, 1,   1, 0, 1,
        1, 1, 1,   1, 1, 1,   1, 1, 1,   1, 1, 1
    };

    //f alpha = (2 * acosf(orientation.w))* radToDeg;

    //f sin_half_theta = sqrt(1.f - pow(orientation.w, 2));
    //if (sin_half_theta < 1.e-9)
    //    glRotatef(alpha, 1.f, 0, 0);
    //else
    //    glRotatef(alpha, orientation.x / sin_half_theta, orientation.y / sin_half_theta, orientation.z / sin_half_theta);

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

void drawLine(const f3& direction) {
    glBegin(GL_LINES);
    glVertex3f(0.f, 0.f, 0.f); // Start point of the line
    glVertex3f(direction[0] * 10.f, direction[1] * 10.f, direction[2] * 10.f);   // End point of the line
    glEnd();
}

void display(const quat& orientation, const rigidbody::f3& lengths) {
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
    const f3x3 rotMat = quaternionToMatrix(orientation).transpose();
    glColor3f(1.0f, 0.0f, 0.0f);
    drawLine(rotMat[0]);
    glColor3f(0.0f, 1.0f, 0.0f);
    drawLine(rotMat[1]);
    glColor3f(0.0f, 0.0f, 1.0f);
    drawLine(rotMat[2]);

    glfwSwapBuffers(GLwindow);
}

rigidbody::f3x3 Simulate(rigidbody::SimulationContext const& context)
{
    using namespace rigidbody;

    ((void)context);

    const f density = context.density;                           
    const f3& lengths = context.lengths;                          
    const f3& initial_impulse = context.initial_impulse;                  
    const f3& direction = context.initial_impulse_application_point;

    const f final_time = context.final_time;

    f current_time = 0.f;
    quat orientation;
    const quat angular_velocity = context.ComputeInitialAngularVelocity();

    f currentTime = 0.0;
    while (currentTime < final_time) {
        orientation.applyAngularVelocity(angular_velocity, time_step);

        currentTime += time_step;

        if (GLwindow && !glfwWindowShouldClose(GLwindow)) {

            display(orientation, context.lengths);
            glfwPollEvents();
        }
    }

    std::cout << orientation << "\n";
    f3x3 final_orientation = quaternionToMatrix(orientation);

    return final_orientation.transpose();
}

} // namespace REC991
