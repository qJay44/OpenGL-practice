#include <stdio.h>

#include "mesh/shader.h"
#include "mesh/object.h"
#include "inputs.h"
#include "camera.h"

// Called when the window resized
void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
  glViewport(0, 0, width, height);
  glfwSetCursorPos(window, width * 0.5f, height * 0.5f);
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

  Texture planksTex = textureCreate("../../src/textures/planks.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_RGBA);
  Texture planksSpecTex = textureCreate("../../src/textures/planksSpec.png", GL_TEXTURE_2D, GL_TEXTURE1, GL_RED, GL_RGBA);
  vec4s lightColor = {1.f, 1.f, 1.f, 1.f};

  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);

  //========== Illumination cube ==========//

  // z+ towards us, z- away from us
  GLfloat lightVertices[] = {
    // coordinates           // colors          // texture    // normals
    -0.1f, -0.1f,  0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
    -0.1f, -0.1f, -0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
     0.1f, -0.1f, -0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
     0.1f, -0.1f,  0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
    -0.1f,  0.1f,  0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
    -0.1f,  0.1f, -0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
     0.1f,  0.1f, -0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
     0.1f,  0.1f,  0.1f,     1.f, 1.f, 1.f,     0.f, 0.f,     0.f, 0.f, 0.f,
  };

  // Triangles indices of vertices
  GLuint lightIndices[] = {
    0, 1, 2,
    0, 2, 3,
    0, 4, 7,
    0, 7, 3,
    3, 7, 6,
    3, 6, 2,
    2, 6, 5,
    2, 5, 1,
    1, 5, 4,
    1, 4, 0,
    4, 5, 6,
    4, 6, 7
  };

  GLint lightShader = shaderCreate("../../src/shaders/light.vert", "../../src/shaders/light.frag");
  Object light = objectCreate(lightVertices, sizeof(lightVertices), lightIndices, sizeof(lightIndices), &lightShader);
  vec3s lightPos = {0.5f, 0.5f, 0.5f};

  objectTranslate(&light, lightPos);

  //====== Main object ====================//

  GLfloat mainVertices[] = {
    -1.f, 0.f,  1.f,     0.f, 0.f, 0.f,     0.f, 0.f,     0.f, 1.f, 0.f,
    -1.f, 0.f, -1.f,     0.f, 0.f, 0.f,     0.f, 1.f,     0.f, 1.f, 0.f,
     1.f, 0.f, -1.f,     0.f, 0.f, 0.f,     1.f, 1.f,     0.f, 1.f, 0.f,
     1.f, 0.f,  1.f,     0.f, 0.f, 0.f,     1.f, 0.f,     0.f, 1.f, 0.f,
  };

  // Triangles indices of vertices
  GLuint mainIndices[] = {
    0, 1, 2,
    0, 2, 3
  };

  GLint mainShader = shaderCreate("../../src/shaders/main.vert", "../../src/shaders/main.frag");
  Object main = objectCreate(mainVertices, sizeof(mainVertices), mainIndices, sizeof(mainIndices), &mainShader);
  objectAddTexture(&main, &planksTex);
  objectAddTexture(&main, &planksSpecTex);

  //===== Uniforms ==============================//

  objectSetMatrixUniform(&light, "matModel");

  objectSetMatrixUniform(&main, "matModel");
  objectSetVec4Unifrom(&main, "lightColor", lightColor);
  objectSetVec3Unifrom(&main, "lightPos", lightPos);
  objectSetTextureUnifrom(&main, "tex0", 0);
  objectSetTextureUnifrom(&main, "tex1", 1);

  //=============================================//

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
    objectSetVec3Unifrom(&main, "camPos", camera.position);

    // Working with pyramid shader
    objectSetCameraMatrixUnifrom(&main, (const GLfloat*)camera.mat.raw, "matCam");
    objectDraw(&main);

    // Working with light shader
    objectSetCameraMatrixUnifrom(&light, (const GLfloat*)camera.mat.raw, "matCam");
    objectDraw(&light);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  objectDelete(&main);
  objectDelete(&light);
  textureDelete(&planksTex, 1);

  glfwTerminate();
  return 0;
}

