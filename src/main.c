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

  GLuint brickTexture = textureCreate("../../src/textures/brick.png", GL_TEXTURE_2D, GL_TEXTURE0, GL_RGBA, GL_RGBA);

  //================= Pyramid =================//

  // z+ towards us, z- away from us
  GLfloat pyramidVertices[] = {
    // coordinates        // colors              // texture cordinates
    -0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f,   0.0f, 0.f,
    -0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,   5.0f, 0.f,
     0.5f, 0.0f, -0.5f,   0.83f, 0.70f, 0.44f,   0.0f, 0.f,
     0.5f, 0.0f,  0.5f,   0.83f, 0.70f, 0.44f,   5.0f, 0.f,
     0.0f, 0.8f,  0.0f,   0.92f, 0.86f, 0.76f,   2.5f, 5.f,
  };

  // Triangles indices of vertices
  GLuint pyramidIndices[] = {
    0, 1, 2, // Bottom 1
    0, 2, 3, // Bottom 2
    0, 1, 4, // Left face
    1, 2, 4, // Rear face
    2, 3, 4, // Right face
    3, 0, 4  // Front face
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

  objectLinkAttrib(&pyramid, 0, 3, 8, 0);
  objectLinkAttrib(&pyramid, 1, 3, 8, 3);
  objectLinkAttrib(&pyramid, 2, 2, 8, 6);

  objectUnbind();
  textureSetUniform(pyramid.shaderProgram, "tex0", 0);

  {
    vec3s pyramidPos = {0.f, 0.f, 0.f};
    objectTranslate(&pyramid, pyramidPos);
    objectSetMatrixUniform(&pyramid, "matModel");
  }
  //=============================================//

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

  {
    vec3s lightPos = {0.5f, 0.5f, 0.5f};
    objectTranslate(&light, lightPos);
    objectSetMatrixUniform(&light, "matModel");
  }

  //=======================================//

  glEnable(GL_DEPTH_TEST);

  Camera camera = {
    .position = {0.f, 0.f, 2.f},
    .orientation = {0.f, 0.f, -1.f},
    .up = {0.f, 1.f, 0.f},
    .aspectRatio = 1.f,
    .speed = 100.f,
    .sensitivity = 100.f,
    .firstClick = true
  };

  double prevTime = glfwGetTime();

  // Render loop
  while (!glfwWindowShouldClose(window)) {
    static int width, height;
    glfwGetWindowSize(window, &width, &height);

    double currTime = glfwGetTime();
    double dt = currTime - prevTime;
    prevTime = currTime;

    glClearColor(0.07f, 0.13f, 0.17f, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    processInput(window, width, height, &camera);

    cameraUpdate(&camera, 45.f, 0.1f, 100.f, (float)width / height, dt);

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
  textureDelete(&brickTexture, 1);

  glfwTerminate();
  return 0;
}

