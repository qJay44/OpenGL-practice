#include <stdio.h>
#include <cglm/types-struct.h>

#include "GLFW/glfw3.h"
#include "inputs.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "texture.h"
#include "camera.h"

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
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

  // z+ towards us, z- away from us
  GLfloat vertices[] = {
    // coordinates        // colors              // texture cordinates
    -0.5f, 0.0f, 0.5f ,   0.83f, 0.70f, 0.44f,   0.0f, 0.f,
    -0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,   5.0f, 0.f,
    0.5f , 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,   0.0f, 0.f,
    0.5f , 0.0f, 0.5f ,   0.83f, 0.70f, 0.44f,   5.0f, 0.f,
    0.0f , 0.8f, 0.0f ,   0.92f, 0.86f, 0.76f,   2.5f, 5.f,
  };

  // Triangles indices of vertices
  GLuint indices[] = {
    0, 1, 2, // Bottom 1
    0, 2, 3, // Bottom 2
    0, 1, 4, // Left face
    1, 2, 4, // Rear face
    2, 3, 4, // Right face
    3, 0, 4  // Front face
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

  vaoLinkAttrib(0, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)0);
  vaoLinkAttrib(1, 3, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
  vaoLinkAttrib(2, 2, GL_FLOAT, 8 * sizeof(GLfloat), (void*)(6 * sizeof(GLfloat)));

  vboUnbind();
  vaoUnbind();
  eboUnbind();

  // Texture
  GLuint texture = textureCreate("../../src/textures/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_RGBA);
  textureSetUniform(shaderProgram, "tex0", 0);

  glEnable(GL_DEPTH_TEST);

  Camera camera = {
    .position = {0.f, 0.f, 2.f},
    .orientation = {0.f, 0.f, -1.f},
    .up = {0.f, 1.f, 0.f},
    .aspectRatio = 1.f,
    .speed = 100.f,
    .sensitivity = 100.f,
    .firstClick = true
  };

  double prevTime = glfwGetTime();

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    double currTime = glfwGetTime();
    double dt = currTime - prevTime;
    prevTime = currTime;

    glClearColor(0.07f, 0.13f, 0.17f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    static int width, height;
    glfwGetWindowSize(window, &width, &height);
    camera.aspectRatio = (float)width / height;

    camera.speed *= dt;
    processInput(window, width, height, &camera);

    cameraSetMatrixUniform(&camera, 45.f, 0.1f, 100.f, shaderProgram, "camMatrix");
    textureBind(texture, GL_TEXTURE_2D);

    glBindVertexArray(vao.id);
    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  // Clear all buffer objects
  vaoDelete(&vao);
  vboDelete(&vbo);
  eboDelete(&ebo);

  textureDelete(&texture, 1);
  glDeleteProgram(shaderProgram);

  glfwTerminate();
  return 0;
}

