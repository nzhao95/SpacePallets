#include <Helpers.h>
#include "REC991.h"
#include <GLFW/glfw3.h>
#include <GL/glu.h>
#include <cmath>
#include <thread>

namespace REC991
{
using namespace rigidbody;
static constexpr f time_step = 1e-2f;
static constexpr f radToDeg = 180.f / M_PI;

//#define draw //Draw is slow turn it on only for debug purposes
#ifdef draw
static GLFWwindow* GLwindow;


// Mouse position
double lastX, lastY;

// Window position
GLint windowX, windowY;

void reshape(int width, int height) {
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)width / (double)height, 1.0, 100.0);
}


// Mouse button callback
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        glfwGetCursorPos(window, &lastX, &lastY);
    }
}

// Cursor position callback
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double deltaX = xpos - lastX;
        double deltaY = ypos - lastY;
        windowX += deltaX;
        windowY += deltaY;
        glfwSetWindowPos(window, windowX, windowY);
        lastX = xpos;
        lastY = ypos;
    }
}

void drawCube(const f angle, const f3& axis, rigidbody::SimulationContext const& context)
{
    GLfloat x = context.lengths[0] * 0.5f;
    GLfloat y = context.lengths[1] * 0.5f;
    GLfloat z = context.lengths[2] * 0.5f;

    //const f3x3 rotMat = quaternionToMatrix(orientation);
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

void drawLine(const f3& direction, const f3& origin = f3(0.f, 0.f, 0.f)) {
    glDisable(GL_DEPTH_TEST);
    glBegin(GL_LINES);
    glVertex3f(origin[0], origin[1], origin[2]); // Start point of the line
    glVertex3f(direction[0], direction[1], direction[2]);   // End point of the line
    glEnd();
    glEnable(GL_DEPTH_TEST);
}

void display(const f angle, const f3& axis, const f3& answerAxis, rigidbody::SimulationContext const& context) {
    // Scale to window size
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
    drawLine( context.initial_impulse_application_point - context.initial_impulse, context.initial_impulse_application_point);

	const f3x3 rotMat = matrixFromAxisAngle(angle, axis).transpose();

	glColor3f(1.0f, 1.0f, 0.0f);
	drawLine(rotMat[0] *(context.lengths[0] + 1.f));

    glColor3f(1.0f, 0.0f, 0.0f);
    drawLine(axis * 5.f);

    glColor3f(0.0f, 0.0f, 1.0f);
    drawLine(answerAxis);

    glfwSwapBuffers(GLwindow);
}
#endif //draw

void GlobalInit()
{
#ifdef draw
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

    // Set callbacks
    glfwSetMouseButtonCallback(GLwindow, mouseButtonCallback);
    glfwSetCursorPosCallback(GLwindow, cursorPosCallback);

    // Get the initial window position
    glfwGetWindowPos(GLwindow, &windowX, &windowY);

    /* Make the window's context current */
    glfwMakeContextCurrent(GLwindow);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);

    glfwSetFramebufferSizeCallback(GLwindow, (GLFWframebuffersizefun)reshape);
#endif
}

void GlobalTeardown()
{
#ifdef draw
    glfwDestroyWindow(GLwindow);
    glfwTerminate();
#endif
}

rigidbody::f3x3 Simulate(rigidbody::SimulationContext const& context, const f3& answerAxis)
{
    using namespace rigidbody;

    ((void)context);

    const f density = context.density;                           
    const f3& lengths = context.lengths;                          
    const f3& initial_impulse = context.initial_impulse;

    const f final_time = context.final_time;
	const quat angular_velocity = context.ComputeInitialAngularVelocity();
	const f3 axis = f3(angular_velocity.x, angular_velocity.y, angular_velocity.z);

    quat orientiation;
#ifdef draw
    f current_time = time_step;

    f angle_mag;
    f3 axisNormed;
    quaternionToAxisAngle(orientiation, angle_mag, axisNormed);
    display(angle_mag, axisNormed, answerAxis, context);
    std::cin.ignore();
	while (current_time < final_time)
	{
        orientiation.applyAngularVelocity(angular_velocity, time_step);
        quaternionToAxisAngle(orientiation, angle_mag, axisNormed);
		display(angle_mag, axisNormed, answerAxis, context);
		current_time += time_step;

	}
#endif

    //f3x3 final_orientation = matrixFromAxisAngle(finalAngle_mag, finalAxisNormed);
    orientiation.applyAngularVelocity(angular_velocity, final_time);
	f3x3 final_orientation = quaternionToMatrix(orientiation);

    return final_orientation;
}

} // namespace REC991
