#include "vao.h"
#include "vbo.h"
#include "ebo.h"

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
    -0.5f, 0.5f , 0.f,
    0.5f , 0.5f , 0.f,
    0.5f , -0.5f, 0.f,
    -0.5f, -0.5f, 0.f
  };

  // Triangles indices of vertices
  GLuint indices[] = {
    0, 1, 2,
    0, 2, 3
  };

  // A vertex shader reference
  GLint vertexShader = loadShader("../../src/shaders/main.vert", GL_VERTEX_SHADER, FALSE);
  glCompileShader(vertexShader);

  // A fragment shader reference
  GLint fragmentShader = loadShader("../../src/shaders/main.frag", GL_FRAGMENT_SHADER, FALSE);
  glCompileShader(fragmentShader);

  // Attach created shader references to a shader program and link it
  GLint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Buffer objects
  struct VAO vao = vaoCreate(1);
  struct VBO vbo = vboCreate(1);
  struct EBO ebo = eboCreate(1);

  vaoBind(&vao);
  vboBind(&vbo, vertices, sizeof(vertices));
  eboBind(&ebo, indices, sizeof(indices));

  vaoLink(&vao, 0);

  vboUnbind();
  vaoUnbind();
  eboUnbind();

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.07f, 0.13f, 0.17f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgram);
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

