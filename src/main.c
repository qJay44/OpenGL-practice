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
#include "inputs.h"
#include "camera.h"
#include "utils.h"

#define NUM_WINDOWS 100

// The information about the windows
vec3s positionsWin[NUM_WINDOWS];
float rotationsWin[NUM_WINDOWS];

// The order of drawing the windows
u32 orderDraw[NUM_WINDOWS];
float distanceCamera[NUM_WINDOWS];

int compare(const void* a, const void* b) {
  double diff = distanceCamera[*(int*)b] - distanceCamera[*(int*)a];
  return (0. < diff) - (diff < 0.);
}

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
}

int main() {
  // Change cwd to where "src" directory located (since launching the executable always from the directory where its located)
  SetCurrentDirectory("../../../");
  srand(time(NULL));

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
  glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
  glfwSetCursorPos(window, 1200 * 0.5f, 720 * 0.5f);

  // GLAD init
  int version = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
  if (!version) {
    printf("Failed to initialize GLAD\n");
    return -1;
  }

  glViewport(0, 0, 1200, 720);
  glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

  Texture defaultTextures[] = {
    textureCreate("src/textures/brick.png", "diffuse"),
  };

  GLint mainShader = shaderCreate("src/shaders/main.vert", "src/shaders/main.frag");
  GLint grassShader = shaderCreate("src/shaders/main.vert", "src/shaders/grass.frag");
  GLint windowsShader = shaderCreate("src/shaders/main.vert", "src/shaders/windows.frag");
  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);

  Model ground = modelCreate("src/mesh/models/ground/");
  Model grass = modelCreate("src/mesh/models/grass/");
  Model windows = modelCreate("src/mesh/models/windows/");
  modelScale(&ground, 0.5f);
  modelScale(&grass, 0.5f);
  modelScale(&windows, 0.5f);

  Object pyramid = objectCreateTestPyramid();
  objectAddTexture(&pyramid, &defaultTextures[0]);

  // ===== Illumination ===== //

  vec4s lightColor = (vec4s){1.f, 1.f, 1.f, 1.f};
  vec3s lightPos = (vec3s){0.5f, 0.5f, 0.5f};
  mat4s lightModel = GLMS_MAT4_IDENTITY_INIT;
  glms_translate(lightModel, lightPos);

  glUseProgram(mainShader);
	glUniform4f(glGetUniformLocation(mainShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(mainShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);
  glUseProgram(grassShader);
	glUniform4f(glGetUniformLocation(grassShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

  // ======================== //

  vec3s backgroundColor = (vec3s){0.07f, 0.13f, 0.17f};
  float nearPlane = 0.1f;
  float farPlane = 100.f;

	glUniform3f(glGetUniformLocation(mainShader, "background"), backgroundColor.x, backgroundColor.y, backgroundColor.z);
	glUniform1f(glGetUniformLocation(mainShader, "near"), nearPlane);
	glUniform1f(glGetUniformLocation(mainShader, "far"), farPlane);

  glUseProgram(grassShader);
	glUniform4f(glGetUniformLocation(grassShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

  glUseProgram(grassShader);
	glUniform4f(glGetUniformLocation(grassShader, "lightColor"), lightColor.x, lightColor.y, lightColor.z, lightColor.w);
	glUniform3f(glGetUniformLocation(grassShader, "lightPos"), lightPos.x, lightPos.y, lightPos.z);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Windows init
  for (int i = 0; i < NUM_WINDOWS; i++) {
    positionsWin[i] = (vec3s) {
      -15.f + rand() / (RAND_MAX / 30.f),
      1.f + rand() / (RAND_MAX / 3.f),
      -15.f + rand() / (RAND_MAX / 30.f)
    };
    rotationsWin[i] = (float)rand() / RAND_MAX;
    orderDraw[i] = i;
  }

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
      titleTimer = glfwGetTime();
    }

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    processInput(window, width, height, &camera);

    cameraMove(&camera, mouseX, mouseY, width, height);
    cameraUpdate(&camera, 45.f, nearPlane, farPlane, (float)width / height, dt);

    // Get distance from each window to the camera
    for (int i = 0; i < NUM_WINDOWS; i++)
      distanceCamera[i] = glms_vec3_norm(glms_vec3_sub(camera.position, positionsWin[i]));

    qsort(orderDraw, NUM_WINDOWS, sizeof(u32), compare);

    modelDraw(&ground, &camera, mainShader);

    // Disable cull face so the grass and windows have both faces
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    modelDraw(&grass, &camera, grassShader);
    for (int i = 0; i < NUM_WINDOWS; i++) {
      modelDrawTRC(
        &windows,
        &camera,
        windowsShader,
        positionsWin[orderDraw[i]],
        (versors){0.f, 0.f, rotationsWin[orderDraw[i]], 1.f},
        (vec3s){1.f, 1.f, 1.f}
      );
    }
    // Use blending only for windows
    glDisable(GL_BLEND);
    glEnable(GL_CULL_FACE);

    /* { */
    /*   mat4s mat = GLMS_MAT4_IDENTITY_INIT; */
    /*   vec3s trans = (vec3s){0.f, 0.f, 0.f}; */
    /*   versors rot = (versors){0.f, 0.f, 0.f, 1.f}; */
    /*   vec3s sca = (vec3s){1.f, 1.f, 1.f}; */
    /*   objectDraw(&pyramid, &camera, mat, trans, rot, sca); */
    /* } */

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  modelDelete(&ground);
  modelDelete(&grass);
  modelDelete(&windows);
  glDeleteProgram(mainShader);
  glDeleteProgram(grassShader);
  glDeleteProgram(windowsShader);

  glfwTerminate();

  printf("Done\n");

  return 0;
}

