#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/types-struct.h"

typedef struct {
  vec3s position, orientation, up;
  float aspectRatio, speed, sensitivity;
  mat4s mat;
} Camera;

[[nodiscard]]
Camera cameraCreate(vec3s pos, vec3s orientation, float sensitivity);

void cameraUpdate(Camera* self, float fov, float nearPlane, float farPlane, float aspectRatio, float dt);
void cameraMoveForward(Camera* self);
void cameraMoveBack(Camera* self);
void cameraMoveLeft(Camera* self);
void cameraMoveRight(Camera* self);
void cameraMoveUp(Camera* self);
void cameraMoveDown(Camera* self);
void cameraSetIncreasedSpeed(Camera* self);
void cameraSetNormalSpeed(Camera* self);
void cameraMove(Camera* self, double x, double y, int w, int h);

#endif

