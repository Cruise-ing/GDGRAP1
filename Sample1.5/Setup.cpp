#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

//GLM Headers
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define _USE_MATH_DEFINES
#include <cmath>

GLFWwindow* window;


//Mod for model's x position
float x_mod = 0;
float y_mod = 0;
float x_mod2 = 0;
float y_mod2 = 0;
float x_mod3 = 0;
float y_mod3 = 0;
float theta = 90;

void Key_Callback(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods)
{
    if (key == GLFW_KEY_D)
        x_mod += 0.1f;
    if (key == GLFW_KEY_A)
        x_mod -= 0.1f;
    if (key == GLFW_KEY_W)
        theta += 1;
    if (key == GLFW_KEY_S)
        y_mod -= 0.1f;

}

int main(void)
{
    float window_width = 600;
    float window_height = 600;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(window_width, window_height, "Basil Lapuz ", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    std::fstream vertSrc("Shaders/Sample.vert");
    std::stringstream vertBuff;

    vertBuff << vertSrc.rdbuf();

    std::string vertS = vertBuff.str();
    const char* v = vertS.c_str();

    std::fstream fragSrc("Shaders/Sample1.frag");
    std::stringstream fragBuff;

    fragBuff << fragSrc.rdbuf();

    std::string fragS = fragBuff.str();
    const char* f = fragS.c_str();

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    gladLoadGL();

    //glViewport(0, 0, 300, 600); // MIn x, y, width , height

    glfwSetKeyCallback(window, Key_Callback);

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &v, NULL);
    glCompileShader(vertexShader);

    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &f, NULL);
    glCompileShader(fragShader);

    GLuint shaderProg = glCreateProgram();
    glAttachShader(shaderProg, vertexShader);
    glAttachShader(shaderProg, fragShader);

    glLinkProgram(shaderProg);

    std::string path = "3D/bunny.obj";
    std::vector <tinyobj::shape_t> shapes;
    std::vector <tinyobj::material_t> material;
    std::string warning, error;

    tinyobj::attrib_t attributes;

    bool success = tinyobj::LoadObj(
        &attributes,
        &shapes,
        &material,
        &warning,
        &error,
        path.c_str()
    );

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    GLfloat vertices[]{
        //x   y    z
        0.f, 0.5f, 0.f,
        -0.5f, -0.5f, 0.f,
        0.5f, -0.5f, 0.f
    };

    GLuint indices[] = {
        0,1,2
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    //Currently editing VAO = null
    glBindVertexArray(VAO);
    //Currently editing  VAO = VAO

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //Currently editing VBO = VBO
    //VAO <- VBO

    glBufferData(GL_ARRAY_BUFFER,
        sizeof(GL_FLOAT) * attributes.vertices.size(),
        attributes.vertices.data(),
        GL_STATIC_DRAW);

    glVertexAttribPointer(
        0, // 0 pos, 2 texture
        3, //  3 components
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    // VBO = VBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // VBO = EBO

    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * mesh_indices.size(),
        mesh_indices.data(),
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    glm::mat4 identity_matrix = glm::mat4(1.0f);

    /*
    glm::mat4 projectionMatrix = glm::ortho(
        -2.f, // L
        2.f,  // R
        -2.f, // B
        2.f,  // T
        -1.f, // Znear
        1.f   // Zfar
    );
    */

    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(60.f),
        window_height / window_width,
        0.1f,
        100.f
    );

    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(shaderProg);

        float radius = 2.0f;
        x_mod = radius * cos(glm::radians(theta));
        y_mod = radius * sin(glm::radians(theta));

        x_mod2 = radius * cos(glm::radians(theta + 120.0f));
        y_mod2 = radius * sin(glm::radians(theta + 120.0f));

        x_mod3 = radius * cos(glm::radians(theta + 240.0f));
        y_mod3 = radius * sin(glm::radians(theta + 240.0f));

        

        /*
         if (x_mod >= 1.0f) x_mod = -1.0f;
         else if (x_mod <= -1.0f) x_mod = 1.0f;
         else if (y_mod >= 1.0f) y_mod = -1.0f;
         else if (y_mod <= -1.0f) y_mod = 1.0f;

        unsigned int xLoc = glGetUniformLocation(shaderProg, "x");
        unsigned int yLoc = glGetUniformLocation(shaderProg, "y");

        glUniform1f(xLoc, x_mod);
        glUniform1f(yLoc, y_mod);
        */

        // glm::mat4 identity_matrix4 = glm::mat4(1.0f);
        // glm::mat4 identity_matrix4 = glm::translate (identity_matrix4, glm::vec3(x,y,z));
        // glm::mat4 identity_matrix4 = glm::rotate (identity_matrix4, glm::radians(theta), glm::vec3(x,y,z));

        theta += 0.01f;

        glm::mat4 transformation_matrix = glm::translate(identity_matrix, glm::vec3(x_mod, y_mod, -5.0f));
        transformation_matrix = glm::scale(transformation_matrix, glm::vec3(2.0f, 2.0f, 2.0f));
        transformation_matrix = glm::rotate(transformation_matrix, glm::radians(theta), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");
        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projectionMatrix));

        unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");

        glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transformation_matrix));

        glBindVertexArray(VAO);

        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);


        glm::mat4 transformation_matrix2 = glm::translate(identity_matrix, glm::vec3(x_mod2, y_mod2, -5.0f)); 
        transformation_matrix2 = glm::scale(transformation_matrix2, glm::vec3(2.0f, 2.0f, 2.0f)); 
        transformation_matrix2 = glm::rotate(transformation_matrix2, glm::radians(theta + 120.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))); 

        unsigned int transformLoc2 = glGetUniformLocation(shaderProg, "transform"); 
        glUniformMatrix4fv(transformLoc2, 1, GL_FALSE, glm::value_ptr(transformation_matrix2));

        glBindVertexArray(VAO); 
        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);

        glm::mat4 transformation_matrix3 = glm::translate(identity_matrix, glm::vec3(x_mod3, y_mod3, -5.0f));
        transformation_matrix3 = glm::scale(transformation_matrix3, glm::vec3(2.0f, 2.0f, 2.0f));
        transformation_matrix3 = glm::rotate(transformation_matrix3, glm::radians(theta + 240.0f), glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f)));

        unsigned int transformLoc3 = glGetUniformLocation(shaderProg, "transform");
        glUniformMatrix4fv(transformLoc3, 1, GL_FALSE, glm::value_ptr(transformation_matrix3));

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, mesh_indices.size(), GL_UNSIGNED_INT, 0);

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();


        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VAO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}