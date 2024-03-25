#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include <math.h>

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);
}

int main() {
  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window init
  GLFWwindow* window = glfwCreateWindow(1200, 720, "LearnOpenGL", NULL, NULL);
  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);

  // GLAD init
  int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  if (!version) {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  glViewport(0, 0, 1200, 720);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

  // Triangles vertices
  GLfloat vertices[] = {
    // x, y, z,          r, g, b
    -0.5f, 0.5f , 0.f,   0.8f, 0.3f, 0.02f,
    0.5f , 0.5f , 0.f,   0.8f, 0.3f, 0.02f,
    0.5f , -0.5f, 0.f,   1.0f, 0.6f, 0.32f,
    -0.5f, -0.5f, 0.f,   0.9f, 0.5f, 0.17f
  };

  // Triangles indices of vertices
  GLuint indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  // A vertex shader reference
  GLint vertexShader = shaderLoad("../../src/shaders/main.vert", GL_VERTEX_SHADER);
  shaderCompile(vertexShader);

  // A fragment shader reference
  GLint fragmentShader = shaderLoad("../../src/shaders/main.frag", GL_FRAGMENT_SHADER);
  shaderCompile(fragmentShader);

  // Attach created shader references to a shader program and link it
  GLint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  shaderProgramLink(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Buffer objects
  struct VAO vao = vaoCreate(1);
  struct VBO vbo = vboCreate(1);
  struct EBO ebo = eboCreate(1);

  vaoBind(&vao);
  vboBind(&vbo, vertices, sizeof(vertices));
  eboBind(&ebo, indices, sizeof(indices));

  vaoLinkAttrib(0, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)0);
  vaoLinkAttrib(1, 3, GL_FLOAT, 6 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

  vboUnbind();
  vaoUnbind();
  eboUnbind();

  GLuint uniScale = glGetUniformLocation(shaderProgram, "scale");
  float scale = 1.f;

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.07f, 0.13f, 0.17f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    scale = sin(glfwGetTime());

    glUseProgram(shaderProgram);
    glUniform1f(uniScale, scale);

    glBindVertexArray(vao.id);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    processInput(window);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clear all buffer objects
  vaoDelete(&vao);
  vboDelete(&vbo);
  eboDelete(&ebo);

  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

