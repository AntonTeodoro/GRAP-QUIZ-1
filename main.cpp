#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

glm::mat4 base_matrix = glm::mat4(1.0f);

float posX = 0.f, posY = 0.f, posZ = 0.f;
float scaleX = 2.f, scaleY = 2.f, scaleZ = 2.f;
float rotAxisX = 0.f, rotAxisY = 1.f, rotAxisZ = 0.f;

float rotateThetaX = 0.f;
float rotateThetaY = 0.f;

void HandleKeyInput(GLFWwindow* window,
    int key,
    int scancode,
    int action,
    int mods) {
    if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
        posX += 0.2f;
    }

    if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
        posX -= 0.2f;
    }

    if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
        posY += 0.2f;
    }

    if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
        posY -= 0.2f;
    }

    if (key == GLFW_KEY_RIGHT && action != GLFW_RELEASE) {
        rotateThetaY += 5.f;
    }

    if (key == GLFW_KEY_LEFT && action != GLFW_RELEASE) {
        rotateThetaY -= 5.f;
    }

    if (key == GLFW_KEY_UP && action != GLFW_RELEASE) {
        rotateThetaX += 5.f;
    }

    if (key == GLFW_KEY_DOWN && action != GLFW_RELEASE) {
        rotateThetaX -= 5.f;
    }

    if (key == GLFW_KEY_E && action != GLFW_RELEASE) {
        scaleX += 0.1f;
        scaleY += 0.1f;
        scaleZ += 0.1f;
    }

    if (key == GLFW_KEY_Q && action != GLFW_RELEASE) {
        scaleX -= 0.2f;
        scaleY -= 0.2f;
        scaleZ -= 0.2f;
    }
}

int main(void) {
    GLFWwindow* window;

    if (!glfwInit()) return -1;

    window = glfwCreateWindow(640, 640, "Louis Anton Teodoro", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    glfwSetKeyCallback(window, HandleKeyInput);

    std::fstream vertexShaderFile("Shaders/sample.vert");
    std::stringstream vertexBuffer;

    vertexBuffer << vertexShaderFile.rdbuf();

    std::string vertexSource = vertexBuffer.str();
    const char* vertexShaderSource = vertexSource.c_str();

    std::fstream fragmentShaderFile("Shaders/sample.frag");
    std::stringstream fragmentBuffer;

    fragmentBuffer << fragmentShaderFile.rdbuf();

    std::string fragmentSource = fragmentBuffer.str();
    const char* fragmentShaderSource = fragmentSource.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);

    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    glCompileShader(fragmentShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);

    glLinkProgram(shaderProgram);

    std::string modelPath = "3D/bunny.obj";
    std::vector<tinyobj::shape_t> objShapes;
    std::vector<tinyobj::material_t> objMaterials;
    std::string objWarning, objError;

    tinyobj::attrib_t objAttributes;

    bool loadSuccess = tinyobj::LoadObj(
        &objAttributes,
        &objShapes,
        &objMaterials,
        &objWarning,
        &objError,
        modelPath.c_str()
    );

    std::vector<GLuint> indices;
    for (int i = 0; i < objShapes[0].mesh.indices.size(); i++) {
        indices.push_back(objShapes[0].mesh.indices[i].vertex_index);
    }

    GLuint vaoID, vboID, eboID;
    glGenVertexArrays(1, &vaoID);
    glGenBuffers(1, &vboID);
    glGenBuffers(1, &eboID);

    glBindVertexArray(vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(GLfloat) * objAttributes.vertices.size(),
        &objAttributes.vertices[0],
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(GLfloat),
        (void*)0
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
        sizeof(GLuint) * indices.size(),
        indices.data(),
        GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 transformMatrix = glm::translate(
            base_matrix,
            glm::vec3(posX, posY, posZ)
        );

        transformMatrix = glm::scale(
            transformMatrix,
            glm::vec3(scaleX, scaleY, scaleZ)
        );

        transformMatrix = glm::rotate(
            transformMatrix,
            glm::radians(rotateThetaY),
            glm::normalize(glm::vec3(rotAxisX, rotAxisY, rotAxisZ))
        );

        transformMatrix = glm::rotate(
            transformMatrix,
            glm::radians(rotateThetaX),
            glm::normalize(glm::vec3(rotAxisX + 1.f, rotAxisY - 1.f, rotAxisZ))
        );

        unsigned int transformUniformLoc = glGetUniformLocation(shaderProgram, "transform");

        glUniformMatrix4fv(transformUniformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(transformMatrix));

        glUseProgram(shaderProgram);
        glBindVertexArray(vaoID);

        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);

        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vaoID);
    glDeleteBuffers(1, &vboID);
    glDeleteBuffers(1, &eboID);

    glfwTerminate();
    return 0;
}
