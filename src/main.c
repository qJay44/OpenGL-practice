#include <stdio.h>
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
    textureCreate("src/textures/brick.png", "diffuse"),
    textureCreate("src/textures/planks.png", "diffuse"),
    textureCreate("src/textures/planksSpeck.png", "diffuse"),
  };

  GLint mainShader = shaderCreate("src/shaders/main.vert", "src/shaders/main.frag");
  GLint outlineShader = shaderCreate("src/shaders/outline.vert", "src/shaders/outline.frag");
  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);

  Model crow = modelCreate("src/mesh/models/crow/");
  Model crowOutline = modelCreate("src/mesh/models/crow-outline/");
  modelScale(&crow, 0.25f);
  modelScale(&crowOutline, 0.25f);

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

  // ======================== //

  vec3s backgroundColor = (vec3s){0.07f, 0.13f, 0.17f};
  float nearPlane = 0.1f;
  float farPlane = 100.f;

	glUniform3f(glGetUniformLocation(mainShader, "background"), backgroundColor.x, backgroundColor.y, backgroundColor.z);
	glUniform1f(glGetUniformLocation(mainShader, "near"), nearPlane);
	glUniform1f(glGetUniformLocation(mainShader, "far"), farPlane);

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);
  glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

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

    glClearColor(backgroundColor.x, backgroundColor.y, backgroundColor.z, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    processInput(window, width, height, &camera);

    cameraMove(&camera, mouseX, mouseY, width, height);
    cameraUpdate(&camera, 45.f, nearPlane, farPlane, (float)width / height, dt);

    glStencilFunc(GL_ALWAYS, 1, 0xff);
    glStencilMask(0xff);
    modelDraw(&crow, &camera, mainShader);

    glStencilFunc(GL_NOTEQUAL, 1, 0xff);
    glStencilMask(0x0);
    glDisable(GL_DEPTH_TEST);
    modelDraw(&crowOutline, &camera, outlineShader);

    glStencilMask(0xff);
    glStencilFunc(GL_ALWAYS, 0, 0xff);
    glEnable(GL_DEPTH_TEST);

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

  modelDelete(&crow);
  glDeleteProgram(mainShader);
  glfwTerminate();

  printf("Done\n");

  return 0;
}

