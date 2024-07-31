#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "cglm/struct/affine-pre.h"
#include "cglm/struct/mat4.h"
#include "cglm/types-struct.h"

#include "mesh/object.h"
#include "mesh/shader.h"
#include "mesh/model.h"
#include "mesh/texture.h"
#include "window.h"
#include "inputs.h"
#include "camera.h"
#include "utils.h"

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
  _globalWindow.width = width;
  _globalWindow.height = height;
}

int main() {
  // Change cwd to where "src" directory located (since launching the executable always from the directory where its located)
  SetCurrentDirectory("../../../");
  srand(time(NULL));

  _globalWindow.width = 1200;
  _globalWindow.height = 720;

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window init
  GLFWwindow* window = glfwCreateWindow(_globalWindow.width, _globalWindow.height, "LearnOpenGL", NULL, NULL);
  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return -1;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, _globalWindow.width * 0.5f, _globalWindow.height * 0.5f);

  // GLAD init
  int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  if (!version) {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  glViewport(0, 0, _globalWindow.width, _globalWindow.height);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

  Texture defaultTextures[] = {
    textureCreate("src/textures/brick.png", "diffuse"),
  };

  GLint mainShader = shaderCreate("src/shaders/main.vert", "src/shaders/main.frag");
  GLint framebufferShader = shaderCreate("src/shaders/framebuffer.vert", "src/shaders/framebuffer.frag");
  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);

  Model model = modelCreate("src/mesh/models/crow/");
  modelScale(&model, 0.5f);

  // ===== Illumination ===== //

  vec4s lightColor = (vec4s){1.f, 1.f, 1.f, 1.f};
  vec3s lightPos = (vec3s){0.5f, 0.5f, 0.5f};
  mat4s lightModel = GLMS_MAT4_IDENTITY_INIT;
  glms_translate(lightModel, lightPos);

  glUseProgram(mainShader);
	glUniform4f(glGetUniformLocation(mainShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(mainShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

  // ======================== //

  vec3s backgroundColor = (vec3s){0.07f, 0.13f, 0.17f};
  float nearPlane = 0.1f;
  float farPlane = 100.f;

	glUniform3f(glGetUniformLocation(mainShader, "background"), backgroundColor.x, backgroundColor.y, backgroundColor.z);
	glUniform1f(glGetUniformLocation(mainShader, "near"), nearPlane);
	glUniform1f(glGetUniformLocation(mainShader, "far"), farPlane);

  glUseProgram(framebufferShader);
  glUniform1i(glGetUniformLocation(framebufferShader, "screenTexture"), 0);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;
  double dt;

  u32 FBO;
  glGenFramebuffers(1, &FBO);
  glBindFramebuffer(GL_FRAMEBUFFER, FBO);

  u32 framebufferTexture;
  glGenTextures(1, &framebufferTexture);
  glBindTexture(GL_TEXTURE_2D, framebufferTexture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _globalWindow.width, _globalWindow.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, framebufferTexture, 0);

  u32 RBO;
  glGenRenderbuffers(1, &RBO);
  glBindRenderbuffer(GL_RENDERBUFFER, RBO);
  glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, _globalWindow.width, _globalWindow.height);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);

  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer error, status: %d (0x%.4x)\n", fboStatus, fboStatus);

  float rectangleVertices[] = {
    // Coords      // texCoords
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,

     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f
  };

  u32 rectVAO, rectVBO;
	glGenVertexArrays(1, &rectVAO);
	glGenBuffers(1, &rectVBO);
	glBindVertexArray(rectVAO);
	glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), &rectangleVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static int width, height;
    static double mouseX, mouseY;

    glfwGetWindowSize(window, &width, &height);
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, width * 0.5f, height * 0.5f);

    currTime = glfwGetTime();
    dt = currTime - prevTime;
    prevTime = currTime;

    glUniform1i(glGetUniformLocation(framebufferShader, "winWidth"), width);
    glUniform1i(glGetUniformLocation(framebufferShader, "winHeight"), height);
    glUniform1f(glGetUniformLocation(framebufferShader, "time"), currTime);

    // Update window title every 0.3 seconds
    if (glfwGetTime() - titleTimer >= 0.3) {
      char fpsStr[256];
      char dtStr[256];
      char title[256];
      u16 fps = 1. / dt;
      sprintf(fpsStr, "%d", fps);
      sprintf(dtStr, "%f", dt);
      concat("FPS: ", fpsStr, title, 256);
      concat(title, " / ", title, 256);
      concat(title, dtStr, title, 256);
      concat(title, " ms", title, 256);
      glfwSetWindowTitle(window, title);
      titleTimer = glfwGetTime();
    }

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    processInput(window, width, height, &camera);

    cameraMove(&camera, mouseX, mouseY, width, height);
    cameraUpdate(&camera, 45.f, nearPlane, farPlane, (float)width / height, dt);

    modelDraw(&model, &camera, mainShader);

    glDisable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUseProgram(framebufferShader);
    glBindVertexArray(rectVAO);
    glDisable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, framebufferTexture);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glEnable(GL_CULL_FACE);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  modelDelete(&model);
  glDeleteProgram(mainShader);

  glfwTerminate();

  printf("Done\n");

  return 0;
}

