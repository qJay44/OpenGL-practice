#include <assert.h>
#include <stdio.h>
#include <windows.h>

#include "cglm/struct/mat4.h"
#include "cglm/types-struct.h"
#include "mesh/object.h"
#include "mesh/shader.h"
#include "mesh/model.h"
#include "inputs.h"
#include "camera.h"
#include "mesh/texture.h"

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
}

int main() {
  // Change cwd where "src" directory located
  SetCurrentDirectory("../../../");

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

  Texture defaultTextures[4] = {
    textureCreate("src/textures/brick.png", "diffuse", 0),
    textureCreate("src/textures/grass_block.png", "diffuse", 1),
    textureCreate("src/textures/planks.png", "diffuse", 2),
    textureCreate("src/textures/planksSpeck.png", "diffuse", 3),
  };

  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);
  GLint mainShader = shaderCreate("src/shaders/main.vert", "src/shaders/main.frag");
  Model model = modelCreate("src/mesh/models/sword/", &mainShader);

  Object pyramid = objectCreatePyramid(&mainShader);
  objectAddTexture(&pyramid, &defaultTextures[0]);

  glEnable(GL_DEPTH_TEST);

  double prevTime = glfwGetTime();

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static int width, height;
    static double mouseX, mouseY;

    glfwGetWindowSize(window, &width, &height);
    glfwGetCursorPos(window, &mouseX, &mouseY);
    glfwSetCursorPos(window, width * 0.5f, height * 0.5f);

    double currTime = glfwGetTime();
    double dt = currTime - prevTime;
    prevTime = currTime;

    glClearColor(0.07f, 0.13f, 0.17f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput(window, width, height, &camera);

    cameraMove(&camera, mouseX, mouseY, width, height);
    cameraUpdate(&camera, 45.f, 0.1f, 100.f, (float)width / height, dt);

    modelDraw(&model, &camera);

    {
      mat4s mat = GLMS_MAT4_IDENTITY_INIT;
      vec3s trans = (vec3s){0.f, 0.f, 0.f};
      versors rot = (versors){0.f, 0.f, 0.f, 1.f};
      vec3s sca = (vec3s){1.f, 1.f, 1.f};
      objectDraw(&pyramid, &camera, mat, trans, rot, sca);
    }

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  modelDelete(&model);

  glfwTerminate();
  printf("Done\n");

  return 0;
}

