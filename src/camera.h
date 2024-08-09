#ifndef CAMERA_H
#define CAMERA_H

#include "cglm/types-struct.h"

typedef struct {
  vec3s position, orientation, up;
  float speed, sensitivity, fov;
  mat4s mat;
} Camera;

[[nodiscard]]
Camera cameraCreate(vec3s pos, vec3s orientation, float sensitivity);

void cameraUpdate(Camera* self, float dt);
void cameraMoveForward(Camera* self);
void cameraMoveBack(Camera* self);
void cameraMoveLeft(Camera* self);
void cameraMoveRight(Camera* self);
void cameraMoveUp(Camera* self);
void cameraMoveDown(Camera* self);
void cameraSetIncreasedSpeed(Camera* self);
void cameraSetNormalSpeed(Camera* self);
void cameraMove(Camera* self, double x, double y);

#endif

