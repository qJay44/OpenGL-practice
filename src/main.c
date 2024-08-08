#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "cglm/types-struct.h"

#include "mesh/object.h"
#include "mesh/shader.h"
#include "mesh/model.h"
#include "window.h"
#include "inputs.h"
#include "camera.h"
#include "utils.h"

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
  _gWindow.width = width;
  _gWindow.height = height;
}

int main() {
  // Change cwd to where "src" directory located (since launching the executable always from the directory where its located)
  SetCurrentDirectory("../../../src");
  srand(time(NULL));

  _gWindow.width = 1200;
  _gWindow.height = 720;

  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Window init
  GLFWwindow* window = glfwCreateWindow(_gWindow.width, _gWindow.height, "LearnOpenGL", NULL, NULL);
  if (!window) {
    printf("Failed to create GFLW window\n");
    glfwTerminate();
    return EXIT_FAILURE;
  }
  glfwMakeContextCurrent(window);
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, _gWindow.width * 0.5f, _gWindow.height * 0.5f);

  // GLAD init
  int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  if (!version) {
    printf("Failed to initialize GLAD\n");
    return EXIT_FAILURE;
  }

  glViewport(0, 0, _gWindow.width, _gWindow.height);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

  GLint mainShader = shaderCreate("shaders/main.vert", "shaders/main.frag", "shaders/main.geom");
  GLint normalsShader = shaderCreate("shaders/main.vert", "shaders/normals.frag", "shaders/normals.geom");
  GLint lightShader = shaderCreate("shaders/light.vert", "shaders/light.frag", NULL);
  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);

  Model model = modelCreate("mesh/models/airplane/");
  modelScale(&model, 0.5f);

  // ===== Illumination ===== //

  vec4s lightColor = (vec4s){1.f, 1.f, 1.f, 1.f};
  vec3s lightPos = (vec3s){0.5f, 1.5f, 0.5f};
  Object lightCube = objectCreateTestLight((vec3s){lightColor.x, lightColor.y, lightColor.z});
  objectTranslate(&lightCube, lightPos);

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

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;
  double dt;

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
      titleTimer = currTime;
    }

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    processInput(window, width, height, &camera);

    cameraMove(&camera, mouseX, mouseY, width, height);
    cameraUpdate(&camera, 45.f, nearPlane, farPlane, (float)width / height, dt);

    modelDraw(&model, &camera, mainShader);
    modelDraw(&model, &camera, normalsShader);

    glDisable(GL_CULL_FACE);
    objectDraw(&lightCube, &camera, lightShader);
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

