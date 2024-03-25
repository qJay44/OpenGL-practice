#include <stdio.h>
#include <cglm/cglm.h>

#include "cglm/affine-pre.h"
#include "cglm/mat4.h"
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "texture.h"

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

  float angle = 0.f;

  glEnable(GL_DEPTH_TEST);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.07f, 0.13f, 0.17f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    angle += 0.01f;

    glUseProgram(shaderProgram);

    static int width, height;
    glfwGetWindowSize(window, &width, &height);

    mat4 model = GLM_MAT4_IDENTITY_INIT;
    mat4 view = GLM_MAT4_IDENTITY_INIT;
    mat4 proj = GLM_MAT4_IDENTITY_INIT;

    vec3 vecTranslate = {0.f, -0.5f, -2.0f};
    glm_rotate_y(model, glm_rad(angle), model);
    glm_translate(view, vecTranslate);
    glm_perspective(glm_rad(45.f), (float)width / height, 0.1f, 100.f, proj);

    GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);

    GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);

    GLint projLoc = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, &proj[0][0]);

    textureBind(texture, GL_TEXTURE_2D);

    glBindVertexArray(vao.id);

    glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, 0);

    processInput(window);

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

