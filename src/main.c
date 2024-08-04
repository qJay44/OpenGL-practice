#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "cglm/struct/affine-pre.h"
#include "cglm/struct/cam.h"
#include "cglm/struct/mat4.h"
#include "cglm/types-struct.h"

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

  GLint mainShader = shaderCreate("src/shaders/main.vert", "src/shaders/main.frag");
  GLint skyboxShader = shaderCreate("src/shaders/skybox.vert", "src/shaders/skybox.frag");
  Camera camera = cameraCreate((vec3s){-1.f, 1.f, 2.f}, (vec3s){0.5f, -0.3f, -1.f}, 100.f);

  Model model = modelCreate("src/mesh/models/airplane/");
  /* modelScale(&model, 0.5f); */

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

  glUseProgram(mainShader);
	glUniform1i(glGetUniformLocation(skyboxShader, "skybox"), 0);

  glEnable(GL_DEPTH_TEST);

  glEnable(GL_CULL_FACE);
  glCullFace(GL_FRONT);
  glFrontFace(GL_CW);

  double titleTimer = glfwGetTime();
  double prevTime = titleTimer;
  double currTime = prevTime;
  double dt;

  float skyboxVertices[] = {
    //   Coordinates
    -1.0f, -1.0f,  1.0f,//        7--------6
     1.0f, -1.0f,  1.0f,//       /|       /|
     1.0f, -1.0f, -1.0f,//      4--------5 |
    -1.0f, -1.0f, -1.0f,//      | |      | |
    -1.0f,  1.0f,  1.0f,//      | 3------|-2
     1.0f,  1.0f,  1.0f,//      |/       |/
     1.0f,  1.0f, -1.0f,//      0--------1
    -1.0f,  1.0f, -1.0f
  };

  unsigned int skyboxIndices[] = {
    // Right
    1, 2, 6,
    6, 5, 1,
    // Left
    0, 4, 7,
    7, 3, 0,
    // Top
    4, 5, 6,
    6, 7, 4,
    // Bottom
    0, 3, 2,
    2, 1, 0,
    // Back
    0, 1, 5,
    5, 4, 0,
    // Front
    3, 7, 6,
    6, 2, 3
  };

	u32 skyboxVAO, skyboxVBO, skyboxEBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glGenBuffers(1, &skyboxEBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, skyboxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(skyboxIndices), &skyboxIndices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

  const char* cubemaps[6] = {
    "src/textures/skybox/right.jpg",
    "src/textures/skybox/left.jpg",
    "src/textures/skybox/top.jpg",
    "src/textures/skybox/bottom.jpg",
    "src/textures/skybox/front.jpg",
    "src/textures/skybox/back.jpg",
  };

	u32 cubemapTexture;
	glGenTextures(1, &cubemapTexture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  for (unsigned int i = 0; i < 6; i++) {
		int width, height, nrChannels;
		byte* data = stbi_load(cubemaps[i], &width, &height, &nrChannels, 0);
		if (data) {
			stbi_set_flip_vertically_on_load(false);
			glTexImage2D(
				GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0,
				GL_RGB,
				width,
				height,
				0,
				GL_RGB,
				GL_UNSIGNED_BYTE,
				data
			);
			stbi_image_free(data);
		} else {
      printf("Failed to load texture: %s\n", cubemaps[i]);
			stbi_image_free(data);
		}
	}

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

    modelDraw(&model, &camera, mainShader);

    glDisable(GL_CULL_FACE);
    glDepthFunc(GL_LEQUAL);

    glUseProgram(skyboxShader);
    mat4s view = GLMS_MAT4_IDENTITY_INIT;
    mat4s proj = GLMS_MAT4_IDENTITY_INIT;

    // Add empty last row and column;
    view = glms_mat4_ins3(
      // Cut the last row and column;
      glms_mat4_pick3(
        // mat4
        glms_lookat(camera.position, glms_vec3_add(camera.position, camera.orientation), camera.up)
      ),
      view
    );

    proj = glms_perspective(glm_rad(45.f), (float)width / height, nearPlane, farPlane);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "view"), 1, GL_FALSE, (const GLfloat*)view.raw);
    glUniformMatrix4fv(glGetUniformLocation(skyboxShader, "projection"), 1, GL_FALSE, (const GLfloat*)proj.raw);

    glBindVertexArray(skyboxVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    glDepthFunc(GL_LESS);
    glEnable(GL_CULL_FACE);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  modelDelete(&model);
  glDeleteProgram(mainShader);
  glDeleteProgram(skyboxShader);

  glfwTerminate();

  printf("Done\n");

  return 0;
}

