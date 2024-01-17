#include <GLFW/glfw3.h>
#include <math.h>

# define M_PI 3.14159265358979323846
int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Basil Lapuz", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        GLfloat centerX = 0.0f;
        GLfloat centerY = 0.0f;

        // Set the radius of the pentagon
        GLfloat radius = 1.0f;

        // Set the number of sides (pentagon has 5 sides)
        GLint n = 5;

        float angleIncrement = 360.0f / 5;  
        angleIncrement *=   M_PI / 180.0f;    // convert degrees to radians
        glBegin(GL_TRIANGLE_FAN);
        float angle = 0.0f;
        for (int i = 0; i <= n; ++i) {
            GLfloat angle = i * 2.0f * M_PI / n;
            GLfloat x = centerX + radius * cos(angle);
            GLfloat y = centerY + radius * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}