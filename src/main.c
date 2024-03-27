#include <stdio.h>

#include "GLFW/glfw3.h"
#include "inputs.h"
#include "object.h"
#include "camera.h"
#include "texture.h"

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

  Camera camera = {
    .position = {-1.f, 1.f, 2.f},
    .orientation = {0.5f, -0.3f, -1.f},
    .up = {0.f, 1.f, 0.f},
    .aspectRatio = 1.f,
    .speed = 100.f,
    .sensitivity = 100.f,
  };

  //========== Illumination cube ==========//

  // z+ towards us, z- away from us
  GLfloat lightVertices[] = {
    -0.1f, -0.1f,  0.1f,
    -0.1f, -0.1f, -0.1f,
     0.1f, -0.1f, -0.1f,
     0.1f, -0.1f,  0.1f,
    -0.1f,  0.1f,  0.1f,
    -0.1f,  0.1f, -0.1f,
     0.1f,  0.1f, -0.1f,
     0.1f,  0.1f,  0.1f
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

  Object light = {
    .vertPtr = lightVertices,
    .vertSize = sizeof(lightVertices),
    .vertCount = sizeof(lightVertices)/sizeof(lightVertices[0]),
    .indPtr = lightIndices,
    .indSize = sizeof(lightIndices),
    .indCount = sizeof(lightIndices)/sizeof(lightIndices[0]),
    .mat = GLMS_MAT4_IDENTITY_INIT
  };

  objectLoadShaders(&light, "../../src/shaders/light.vert", "../../src/shaders/light.frag");
  objectBind(&light);
  objectLinkAttrib(&light, 0, 3, 3, 0);
  objectUnbind();

  vec3s lightPos = {0.5f, 0.5f, 0.5f};
  objectTranslate(&light, lightPos);

  objectSetMatrixUniform(&light, "matModel");
  objectSetVec4Unifrom(&light, "lightColor", lightColor);

  //=======================================//

  //================= Pyramid =================//

  // z+ towards us, z- away from us
  GLfloat mainVertices[] = {
    // coordinates       // colors          // texture    // normals
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

  Object main = {
    .vertPtr = mainVertices,
    .vertSize = sizeof(mainVertices),
    .vertCount = sizeof(mainVertices)/sizeof(mainVertices[0]),
    .indPtr = mainIndices,
    .indSize = sizeof(mainIndices),
    .indCount = sizeof(mainIndices)/sizeof(mainIndices[0]),
    .mat = GLMS_MAT4_IDENTITY_INIT
  };

  objectLoadShaders(&main, "../../src/shaders/main.vert", "../../src/shaders/main.frag");
  objectBind(&main);

  objectLinkAttrib(&main, 0, 3, 11, 0);
  objectLinkAttrib(&main, 1, 3, 11, 3);
  objectLinkAttrib(&main, 2, 2, 11, 6);
  objectLinkAttrib(&main, 3, 3, 11, 8);

  objectUnbind();
  textureSetUniform(main.shaderProgram, "tex0", 0);
  textureSetUniform(main.shaderProgram, "tex1", 1);

  {
    vec3s pyramidPos = {0.f, 0.f, 0.f};
    objectTranslate(&main, pyramidPos);
  }

  objectSetMatrixUniform(&main, "matModel");
  objectSetVec4Unifrom(&main, "lightColor", lightColor);
  objectSetVec3Unifrom(&main, "lightPos", lightPos);

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
    cameraSetMatrixUniform(&camera, main.shaderProgram, "matCam");
    textureBind(&planksTex, GL_TEXTURE_2D);
    textureBind(&planksSpecTex, GL_TEXTURE_2D);
    objectDraw(&main);

    // Working with light shader
    cameraSetMatrixUniform(&camera, light.shaderProgram, "matCam");
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

