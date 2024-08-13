#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <windows.h>

#include "cglm/types-struct.h"

#include "camera.h"
#include "mesh/framebuffer.h"
#include "mesh/object.h"
#include "mesh/rbo.h"
#include "mesh/shader.h"
#include "mesh/model.h"
#include "inputs.h"
#include "mesh/vao.h"

#define NUM_ASTEROIDS 500

struct State _gState = {
  .nearPlane = 0.1f,
  .farPlane = 100.f,
  .winWidth = 1200,
  .winHeight = 720,
  .aaSamples = 8,
  .gamma = 2.2f,
  .time = 0.f,
};

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
  _gState.winWidth = width;
  _gState.winHeight = height;
}

int main() {
  // Change cwd to where "src" directory located (since launching the executable always from the directory where its located)
  SetCurrentDirectory("../../../src");
  srand(time(NULL));
  u32 samples = 8;

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_SAMPLES, samples);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window init
  GLFWwindow* window = glfwCreateWindow(_gState.winWidth, _gState.winHeight, "LearnOpenGL", NULL, NULL);
  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, _gState.winWidth * 0.5f, _gState.winHeight * 0.5f);

  // GLAD init
  int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  if (!version) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, _gState.winWidth, _gState.winHeight);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

  // ========== Shaders ========== //

  GLint mainShader = shaderCreate("shaders/main.vert", "shaders/main.frag", "shaders/main.geom");
  GLint normalsShader = shaderCreate("shaders/main.vert", "shaders/normals.frag", "shaders/normals.geom");
  GLint lightShader = shaderCreate("shaders/light.vert", "shaders/light.frag", NULL);
  GLint framebufferShader = shaderCreate("shaders/framebuffer.vert", "shaders/framebuffer.frag", NULL);

  // ========== Illumination ========== //

  vec4s lightColor = (vec4s){1.f, 1.f, 1.f, 1.f};
  vec3s lightPos = (vec3s){0.5f, 1.5f, 0.5f};
  Object lightCube = objectCreateTestLight((vec3s){lightColor.x, lightColor.y, lightColor.z});
  objectTranslate(&lightCube, lightPos);

  glUseProgram(mainShader);
	glUniform4f(glGetUniformLocation(mainShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(mainShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

  // ======================== //

  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);

  Model model = modelCreate("mesh/models/airplane");
  modelScale(&model, 0.25f);

  vec3s backgroundColor = (vec3s){0.07f, 0.13f, 0.17f};

  glUseProgram(mainShader);
	glUniform3f(glGetUniformLocation(mainShader, "background"), backgroundColor.x, backgroundColor.y, backgroundColor.z);
	glUniform1f(glGetUniformLocation(mainShader, "near"), _gState.nearPlane);
	glUniform1f(glGetUniformLocation(mainShader, "far"), _gState.farPlane);

  // ========== Rectangle for the default framebuffer ========== //

  const float rectangleVertices[] = {
    // Coords    // texCoords
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f,

     1.0f,  1.0f,  1.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
    -1.0f,  1.0f,  0.0f, 1.0f
  };

  struct VAO vaoRect = vaoCreate(1);
  struct VBO vboRect = vboCreate(1, rectangleVertices, sizeof(rectangleVertices));

  vaoBind(&vaoRect);
  vboBind(&vboRect);

  vaoLinkAttrib(0, 2, GL_FLOAT, 4 * sizeof(float), (void*)0);
  vaoLinkAttrib(1, 2, GL_FLOAT, 4 * sizeof(float), (void*)(2 * sizeof(float)));

  vaoUnbind();
  vboUnbind();

  // =========================================================== //

  // NOTE: Order matters
  Framebuffer framebufferMS = framebufferCreate(FRAMEBUFFER_MULTISAMPLE);
  framebufferBind(&framebufferMS);
  struct RBO rbo = rboCreate(1, GL_TEXTURE_2D_MULTISAMPLE);
  Framebuffer framebuffer = framebufferCreate(FRAMEBUFFER_DEFAULT);

  glUseProgram(framebufferShader);
	glUniform1i(glGetUniformLocation(framebufferShader, "screenTexture"), framebufferMS.texture.unit);
	glUniform1i(glGetUniformLocation(framebufferShader, "gamma"), _gState.gamma);

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;
  double dt;

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static double mouseX, mouseY;

    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, _gState.winWidth * 0.5f, _gState.winHeight * 0.5f);

    currTime = glfwGetTime();
    dt = currTime - prevTime;
    prevTime = currTime;
    _gState.time = currTime;

    // Update window title every 0.3 seconds
    if (glfwGetTime() - titleTimer >= 0.3) {
      char title[256];
      u16 fps = 1. / dt;
      sprintf(title, "FPS: %d / %f ms", fps, dt);
      glfwSetWindowTitle(window, title);
      titleTimer = currTime;
    }

    framebufferBind(&framebufferMS);
    glClearColor(powf(backgroundColor.x, _gState.gamma), powf(backgroundColor.y, _gState.gamma), powf(backgroundColor.z, _gState.gamma), 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput(window, &camera);

    cameraMove(&camera, mouseX, mouseY);
    cameraUpdate(&camera, dt);

    modelDraw(&model, &camera, mainShader);

    glDisable(GL_CULL_FACE);

    objectDraw(&lightCube, &camera, lightShader);

    framebufferBindReadDraw(&framebufferMS, &framebuffer);
    framebufferUnbind(); // Activating the default framebuffer
    // Draw on the default framebuffer with the texture from the framebuffer before
    framebufferDraw(&framebuffer, framebufferShader, &vaoRect, sizeof(rectangleVertices) / sizeof(rectangleVertices[0]));

    glEnable(GL_CULL_FACE);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  modelDelete(&model);

  glDeleteProgram(mainShader);
  glDeleteProgram(normalsShader);
  glDeleteProgram(lightShader);
  glDeleteProgram(framebufferShader);

  glfwTerminate();

  printf("Done\n");

  return 0;
}

