#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define _USE_MATH_DEFINES
#include "tiny_obj_loader.h"
#include "stb_image.h"

GLFWwindow* window;

float x_mod = 0;
float y_mod = 0;
float theta = 90;

// FPS Camera variables
glm::vec3 cameraPosition = glm::vec3(0.0f, 0.0f, 5.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
float yaw = -90.0f;
float pitch = 0.0f;
float fov = 45.0f;
float cameraSpeed = 1.f;
float cameraSensitivity = 0.05f;
float lastX = 400, lastY = 300;
bool firstMouse = true;
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// Model3D class
class Model3D {
public:
    glm::vec3 position;  // Position of the model in 3D space
    glm::vec3 rotation;  // Rotation angles (in degrees) around x, y, z axes
    glm::vec3 scale;     // Scaling factors along x, y, z axes

    Model3D(glm::vec3 pos, glm::vec3 rot, glm::vec3 scl)
        : position(pos), rotation(rot), scale(scl) {}

    // Calculate the model matrix based on position, rotation, and scale
    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);  // Translate to the specified position
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));  // Rotate around x-axis
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));  // Rotate around y-axis
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));  // Rotate around z-axis
        model = glm::scale(model, scale);  // Scale the model
        return model;
    }
};

// Create a new model with default rotation and scale, positioned at 'position'
Model3D spawnNewModel(const glm::vec3& position) {
    return Model3D(position, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
}

// Mouse callback function for camera control
void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;  // Initialize last X coordinate for mouse movement
        lastY = ypos;  // Initialize last Y coordinate for mouse movement
        firstMouse = false;  // Set to false after the first mouse movement
    }

    // Calculate the offset of mouse movement
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    // Apply sensitivity to mouse movement
    xoffset *= cameraSensitivity;
    yoffset *= cameraSensitivity;

    // Update yaw (horizontal rotation) and pitch (vertical rotation) based on mouse movement
    yaw += xoffset;
    pitch += yoffset;

    // Clamp pitch to prevent flipping
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    // Calculate the new front vector based on yaw and pitch
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(front);  // Normalize the front vector
}

// Scroll callback function for camera control
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Adjust field of view (FOV) based on scroll input
    fov -= (float)yoffset;

    // Clamp FOV to reasonable values
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}


// Process input function for camera control
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        cameraPosition += cameraSpeed * cameraFront * deltaTime;  // Move forward
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        cameraPosition -= cameraSpeed * cameraFront * deltaTime;  // Move backward
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        cameraPosition -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;  // Strafe left
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        cameraPosition += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed * deltaTime;  // Strafe right
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

    std::fstream vertSkySrc("Shaders/betlog.vert");
    std::stringstream vertSkyBuff;

    vertSkyBuff << vertSkySrc.rdbuf();

    std::string vertSky = vertSkyBuff.str();
    const char* v_sky = vertSky.c_str();

    std::fstream fragSkySrc("Shaders/betlog.frag");
    std::stringstream fragSkyBuff;

    fragSkyBuff << fragSkySrc.rdbuf();

    std::string fragSky = fragSkyBuff.str();
    const char* f_sky = fragSky.c_str();



    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    gladLoadGL();

    int img_width,
        img_height,
        colorChannels;

    stbi_set_flip_vertically_on_load(true);

    unsigned char* tex_bytes =
        stbi_load("3D/partenza.jpg",
            &img_width,
            &img_height,
            &colorChannels,
            0);

    //glViewport(0, 0, 300, 600); // MIn x, y, width , height


    // Set up camera control callbacks
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);


    // Create a Model3D instance for spawned models
    std::vector<Model3D> models;
    Model3D spawnedModel(glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.1f, 0.1f, 0.1f));
    double lastSpawnTime = glfwGetTime();

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

    GLuint vertexSkyboxShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexSkyboxShader, 1, &v_sky, NULL);
    glCompileShader(vertexSkyboxShader);

    GLuint fragSkyboxShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragSkyboxShader, 1, &f_sky, NULL);
    glCompileShader(fragSkyboxShader);

    GLuint skyboxShaderProg = glCreateProgram();
    glAttachShader(skyboxShaderProg, vertexSkyboxShader);
    glAttachShader(skyboxShaderProg, fragSkyboxShader);

    glLinkProgram(skyboxShaderProg);

    std::string path = "3D/djSword.obj";
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
    std::vector<GLfloat> fullVertexData;

    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        tinyobj::index_t vData = shapes[0].mesh.indices[i];
        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3)]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.vertices[(vData.vertex_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3)]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 1]
        );

        fullVertexData.push_back(
            attributes.normals[(vData.normal_index * 3) + 2]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2)]
        );

        fullVertexData.push_back(
            attributes.texcoords[(vData.texcoord_index * 2) + 1]
        );



    }

    std::vector<GLuint> mesh_indices;
    for (int i = 0; i < shapes[0].mesh.indices.size(); i++) {
        mesh_indices.push_back(
            shapes[0].mesh.indices[i].vertex_index
        );
    }

    float skyboxVertices[]{
        -1.f, -1.f, 1.f, //0
        1.f, -1.f, 1.f,  //1
        1.f, -1.f, -1.f, //2
        -1.f, -1.f, -1.f,//3
        -1.f, 1.f, 1.f,  //4
        1.f, 1.f, 1.f,   //5
        1.f, 1.f, -1.f,  //6
        -1.f, 1.f, -1.f  //7
    };

    //Skybox Indices
    unsigned int skyboxIndices[]{
        1,2,6,
        6,5,1,

        0,4,7,
        7,3,0,

        4,5,6,
        6,7,4,

        0,3,2,
        2,1,0,

        0,1,5,
        5,4,0,

        3,7,6,
        6,2,3
    };





    GLfloat vertices[]{
        //x   y    z
        0.f, 0.5f, 0.f,
        -0.5f, -0.5f, 0.f,
        0.5f, -0.5f, 0.f
    };

    GLuint indices[] = {
        0,1,2
    };

    GLfloat UV[]{
        0.f, 1.f,
        0.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        1.f, 1.f,
        1.f, 0.f,
        0.f, 1.f,
        0.f, 0.f
    };

    unsigned int skyboxVAO, skyboxVBO, skyboxEBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glGenBuffers(1, &skyboxEBO);
    glBindVertexArray(skyboxVAO);


    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(skyboxVertices),
        &skyboxVertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GL_INT) * 36,
        &skyboxIndices,
        GL_STATIC_DRAW
    );

    glEnableVertexAttribArray(0);

    std::string facesSkybox[]{
        "Skybox/rainbow_rt.png",
        "Skybox/rainbow_lf.png",
        "Skybox/rainbow_up.png",
        "Skybox/rainbow_dn.png",
        "Skybox/rainbow_ft.png",
        "Skybox/rainbow_bk.png",
    };


    GLuint VAO, VBO, EBO, VBO_UV;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * fullVertexData.size(),
        fullVertexData.data(),
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    GLintptr normalPtr = 3 * sizeof(float);

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)normalPtr
    );

    glEnableVertexAttribArray(1);

    GLintptr uvPtr = 6 * sizeof(float);

    glVertexAttribPointer(
        2,
        2,
        GL_FLOAT,
        GL_FALSE,
        8 * sizeof(float),
        (void*)uvPtr
    );


    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);



    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    glm::mat4 identity_matrix = glm::mat4(1.0f);

    unsigned int skyboxTex;

    glGenTextures(1, &skyboxTex);
    glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        int w, h, skyCChannel;
        stbi_set_flip_vertically_on_load(false);
        unsigned char* data = stbi_load(facesSkybox[i].c_str(), &w, &h, &skyCChannel, 0);

        if (data) {
            glTexImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                0,
                GL_RGB,
                w,
                h,
                0,
                GL_RGB,
                GL_UNSIGNED_BYTE,
                data
            );

            stbi_image_free(data);
        }
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D,
        0,
        GL_RGB,
        img_width,
        img_height,
        0,
        GL_RGB,
        GL_UNSIGNED_BYTE,
        tex_bytes);

    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(tex_bytes);

    glEnable(GL_DEPTH_TEST);





    glm::mat4 projectionMatrix = glm::perspective(
        glm::radians(60.f),
        window_height / window_width,
        0.1f,
        100.f
    );

    glm::vec3 cameraPos = glm::vec3(0, 0, 10.0f);

    glm::mat4 cameraPositionMatrix =
        glm::translate(glm::mat4(1.0f),
            cameraPos * -1.0f);
    glm::vec3 WorldUp = glm::vec3(0, 1.0f, 0);
    glm::vec3 Center = glm::vec3(0, 3.f, 0);

    glm::vec3 F = glm::vec3(Center - cameraPos);

    F = glm::normalize(F);

    glm::vec3 R = glm::normalize(glm::cross(F, WorldUp));

    glm::vec3 U = glm::normalize(glm::cross(R, F));


    glm::vec3 lightPos = glm::vec3(-10, 5, 7);
    glm::vec3 lightColor = glm::vec3(0.941, 0.608, 0.122);
    glm::mat4 viewMatrix = glm::lookAt(cameraPos, Center, WorldUp);
    float ambientStr = 0.1f;
    glm::vec3 ambientColor = lightColor;
    float specStr = 0.5f;
    float specPhong = 16;




    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glDepthMask(GL_FALSE);
        glDepthFunc(GL_LEQUAL);
        glUseProgram(skyboxShaderProg);

        glm::mat4 sky_view = glm::mat4(1.f);
        sky_view = glm::mat4(glm::mat3(viewMatrix));
        glm::mat4 projection = glm::perspective(glm::radians(fov), window_width / window_height, 0.1f, 100.0f);


        unsigned int skyboxViewLoc = glGetUniformLocation(skyboxShaderProg, "view");
        glUniformMatrix4fv(skyboxViewLoc,
            1,
            GL_FALSE,
            glm::value_ptr(sky_view));

        unsigned int skyboxProjLoc = glGetUniformLocation(skyboxShaderProg, "projection");
        glUniformMatrix4fv(skyboxProjLoc,
            1,
            GL_FALSE,
            glm::value_ptr(projection));

        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);
        glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        glDepthMask(GL_TRUE);
        glDepthFunc(GL_LESS);


        glUseProgram(shaderProg);

        //Keep track of time to use for cooldown
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
        unsigned int viewLoc = glGetUniformLocation(shaderProg, "view");
        glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));

        unsigned int projectionLoc = glGetUniformLocation(shaderProg, "projection");

        glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

        //Keep listening for inputs from the function processInput()
        processInput(window);



        // Spawn a new model with a 3-second cooldown
        double currentTime = glfwGetTime();
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && currentTime - lastSpawnTime > 1.0) {
            // Create a new model and add it to the models vector
            models.push_back(spawnNewModel(cameraPosition + 2.0f * cameraFront));
            lastSpawnTime = currentTime;

        }



        for (const auto& model : models) {
            // Render the spawned model
            glm::mat4 transform = model.getModelMatrix();

            unsigned int transformLoc = glGetUniformLocation(shaderProg, "transform");
            glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(transform));

            // Use the model's transformation directly, no need for fixed transformations
            GLuint tex0Address = glGetUniformLocation(shaderProg, "tex0");

            glBindTexture(GL_TEXTURE_2D, texture);

            GLuint brightnessAddress = glGetUniformLocation(shaderProg, "brightness");
            glUniform1f(brightnessAddress, 1.0f);
            GLuint lightAddress = glGetUniformLocation(shaderProg, "lightPos");
            glUniform3fv(lightAddress, 1, glm::value_ptr(lightPos));
            GLuint lightColorAddress = glGetUniformLocation(shaderProg, "lightColor");
            glUniform3fv(lightColorAddress, 1, glm::value_ptr(lightColor));
            GLuint ambientStrAddress = glGetUniformLocation(shaderProg, "ambientStr");
            glUniform1f(ambientStrAddress, ambientStr);
            GLuint ambientColorAddress = glGetUniformLocation(shaderProg, "ambientColor");
            glUniform3fv(ambientColorAddress, 1, glm::value_ptr(ambientColor));
            GLuint cameraPosAddress = glGetUniformLocation(shaderProg, "cameraPos");
            glUniform3fv(cameraPosAddress, 1, glm::value_ptr(cameraPos));
            GLuint specStrAddress = glGetUniformLocation(shaderProg, "specStr");
            glUniform1f(specStrAddress, specStr);
            GLuint specPhongAddress = glGetUniformLocation(shaderProg, "specPhong");
            glUniform1f(specPhongAddress, specPhong);


            glUniform1i(tex0Address, 0);

            glBindVertexArray(VAO);

            glDrawArrays(GL_TRIANGLES, 0, fullVertexData.size() / 8);
        }





        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VAO);
    // glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}