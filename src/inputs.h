#ifndef INPUTS_H
#define INPUTS_H

#include "camera.h"

void processInput(GLFWwindow* window, Camera* camera) {
  if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, GLFW_TRUE);

  if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraMoveForward(camera);
  if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraMoveLeft(camera);
  if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraMoveBack(camera);
  if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraMoveRight(camera);

  if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraMoveUp(camera);
  if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) cameraMoveDown(camera);

  if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) cameraSetIncreasedSpeed(camera);
  else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) cameraSetNormalSpeed(camera);
}

#endif

