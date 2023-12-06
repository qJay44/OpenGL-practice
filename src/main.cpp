#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "utils.hpp"

int main(void)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLfloat vertices[] = {
      -0.5f, -0.5f * float(sqrt(3)) / 3.f, 0.f,
       0.5f, -0.5f * float(sqrt(3)) / 3.f, 0.f,
       0.f ,  0.5f * float(sqrt(3)) * 2 / 3.f, 0.f,
    };

    GLFWwindow* window = glfwCreateWindow(1200, 720, "Test title", NULL, NULL);
    glfwMakeContextCurrent(window);

    gladLoadGL();

    glViewport(0, 0, 800, 800);

    // Load vertex shader
    const std::string vertShaderString = readFromFile("../../src/shaders/vert.vert");
    const char* vertShaderCharPtr = vertShaderString.c_str();
    GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertShader, 1, &vertShaderCharPtr, NULL);
    glCompileShader(vertShader);

    // Load fragment shader
    const std::string fragShaderString = readFromFile("../../src/shaders/frag.frag");
    const char* fragShaderCharPtr = fragShaderString.c_str();
    GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragShader, 1, &fragShaderCharPtr, NULL);
    glCompileShader(fragShader);

    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertShader);
    glAttachShader(shaderProgram, fragShader);
    glLinkProgram(shaderProgram);

    // Since shaders were loaded we are no longer need them
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);

    GLuint VAO, VBO; // Not an array because stores only one object

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    while (!glfwWindowShouldClose(window)) {
      glfwPollEvents();

      glClearColor(0.07f, 0.13f, 0.17f, 1.0f);
      glClear(GL_COLOR_BUFFER_BIT);
      glUseProgram(shaderProgram);
      glBindVertexArray(VAO);
      glDrawArrays(GL_TRIANGLES, 0, 3);

      glfwSwapBuffers(window);
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

