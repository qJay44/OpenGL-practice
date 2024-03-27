#include <stdio.h>
#include <cglm/types-struct.h>

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

  GLuint brickTexture = textureCreate("../../src/textures/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_RGBA);
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
  GLfloat pyramidVertices[] = {
    // coordinates        // colors              // texture    // normals
    -0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,   0.0f, -1.0f, 0.0f, // Bottom side
    -0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,	 0.0f, 5.0f,   0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,	 5.0f, 5.0f,   0.0f, -1.0f, 0.0f, // Bottom side
     0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,   0.0f, -1.0f, 0.0f, // Bottom side

    -0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,  -0.8f, 0.5f,  0.0f, // Left Side
    -0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,  -0.8f, 0.5f,  0.0f, // Left Side
     0.0f, 0.8f,  0.0f,   0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,  -0.8f, 0.5f,  0.0f, // Left Side

    -0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,   0.0f, 0.5f, -0.8f, // Non-facing side
     0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,   0.0f, 0.5f, -0.8f, // Non-facing side
     0.0f, 0.8f,  0.0f,   0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,   0.0f, 0.5f, -0.8f, // Non-facing side

     0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,	 0.0f, 0.0f,   0.8f, 0.5f,  0.0f, // Right side
     0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,   0.8f, 0.5f,  0.0f, // Right side
     0.0f, 0.8f,  0.0f,   0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,   0.8f, 0.5f,  0.0f, // Right side

     0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f,	 5.0f, 0.0f,   0.0f, 0.5f,  0.8f, // Facing side
    -0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f, 	 0.0f, 0.0f,   0.0f, 0.5f,  0.8f, // Facing side
     0.0f, 0.8f,  0.0f,   0.92f, 0.86f, 0.76f,	 2.5f, 5.0f,   0.0f, 0.5f,  0.8f  // Facing side
  };

  // Triangles indices of vertices
  GLuint pyramidIndices[] = {
    0, 1, 2, // Bottom side
    0, 2, 3, // Bottom side
    4, 6, 5, // Left side
    7, 9, 8, // Non-facing side
    10, 12, 11, // Right side
    13, 15, 14 // Facing side
  };

  Object pyramid = {
    .vertPtr = pyramidVertices,
    .vertSize = sizeof(pyramidVertices),
    .vertCount = sizeof(pyramidVertices)/sizeof(pyramidVertices[0]),
    .indPtr = pyramidIndices,
    .indSize = sizeof(pyramidIndices),
    .indCount = sizeof(pyramidIndices)/sizeof(pyramidIndices[0]),
    .mat = GLMS_MAT4_IDENTITY_INIT
  };

  objectLoadShaders(&pyramid, "../../src/shaders/main.vert", "../../src/shaders/main.frag");
  objectBind(&pyramid);

  objectLinkAttrib(&pyramid, 0, 3, 11, 0);
  objectLinkAttrib(&pyramid, 1, 3, 11, 3);
  objectLinkAttrib(&pyramid, 2, 2, 11, 6);
  objectLinkAttrib(&pyramid, 3, 3, 11, 8);

  objectUnbind();
  textureSetUniform(pyramid.shaderProgram, "tex0", 0);

  {
    vec3s pyramidPos = {0.f, 0.f, 0.f};
    objectTranslate(&pyramid, pyramidPos);
  }

  objectSetMatrixUniform(&pyramid, "matModel");
  objectSetVec4Unifrom(&pyramid, "lightColor", lightColor);
  objectSetVec3Unifrom(&pyramid, "lightPos", lightPos);
  objectSetVec3Unifrom(&pyramid, "camPos", camera.position);

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

    cameraUpdate(&camera, 45.f, 0.1f, 100.f, (float)width / height, dt);
    cameraMove(&camera, mouseX, mouseY, width, height);

    // Working with pyramid shader
    cameraSetMatrixUniform(&camera, pyramid.shaderProgram, "matCam");
    textureBind(brickTexture, GL_TEXTURE_2D);
    objectDraw(&pyramid);

    // Working with light shader
    cameraSetMatrixUniform(&camera, light.shaderProgram, "matCam");
    objectDraw(&light);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  objectDelete(&pyramid);
  objectDelete(&light);
  textureDelete(&brickTexture, 1);

  glfwTerminate();
  return 0;
}

