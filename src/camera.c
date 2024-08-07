#include "cglm/struct/cam.h"
#include "cglm/struct/mat4.h"

#include "camera.h"

Camera cameraCreate(vec3s pos, vec3s orientation, float sensitivity) {
  Camera camera = {
    .position = pos,
    .orientation = orientation,
    .up = {0.f, 1.f, 0.f},
    .speed = 100.f,
    .sensitivity = sensitivity,
    .fov = 45.f,
  };

  return camera;
}

void cameraUpdate(Camera* self, float dt) {
  mat4s view = GLMS_MAT4_IDENTITY_INIT;
  mat4s proj = GLMS_MAT4_IDENTITY_INIT;

  float aspectRatio = (float)_gState.winWidth / _gState.winHeight;
  self->speed *= dt;

  vec3s lookPos = glms_vec3_add(self->position, self->orientation);

  view = glms_lookat(self->position, lookPos, self->up);
  proj = glms_perspective(glm_rad(self->fov), aspectRatio, _gState.nearPlane, _gState.farPlane);

  self->mat = glms_mat4_mul(proj, view);
}

void cameraMoveForward(Camera* self) {
  vec3s quotient = glms_vec3_scale(self->orientation, self->speed);
  self->position = glms_vec3_add(self->position, quotient);
}

void cameraMoveBack(Camera* self) {
  vec3s quotient = glms_vec3_scale(self->orientation, -self->speed);
  self->position = glms_vec3_add(self->position, quotient);
}

void cameraMoveLeft(Camera* self) {
  vec3s normCross = glms_normalize(glms_cross(self->orientation, self->up));
  vec3s quotient = glms_vec3_scale(normCross, -self->speed);
  self->position = glms_vec3_add(self->position, quotient);
}

void cameraMoveRight(Camera* self) {
  vec3s normCross = glms_normalize(glms_cross(self->orientation, self->up));
  vec3s quotient = glms_vec3_scale(normCross, self->speed);
  self->position = glms_vec3_add(self->position, quotient);
}

void cameraMoveUp(Camera* self) {
  vec3s quotient = glms_vec3_scale(self->up, self->speed);
  self->position = glms_vec3_add(self->position, quotient);
}

void cameraMoveDown(Camera* self) {
  vec3s quotient = glms_vec3_scale(self->up, -self->speed);
  self->position = glms_vec3_add(self->position, quotient);
}

void cameraSetIncreasedSpeed(Camera* self) {
  self->speed = 4.f;
}

void cameraSetNormalSpeed(Camera* self) {
  self->speed = 3.f;
}

void cameraMove(Camera* self, double x, double y) {
  // Normalizes and shifts the coordinates of the cursor such that they begin in the middle of the screen
  // and then "transforms" them into degrees
  float rotX = self->sensitivity * (y - _gState.winHeight * 0.5f) / _gState.winHeight;
  float rotY = self->sensitivity * (x - _gState.winWidth * 0.5f) / _gState.winWidth;

  // Calculates upcoming vertical change in the Orientation
  vec3s newOrientation = glms_vec3_rotate(self->orientation, glm_rad(-rotX), glms_normalize(glms_cross(self->orientation, self->up)));
  vec3s upOpposite = glms_vec3_scale(self->up, -1.f);

  // Decides whether or not the next vertical Orientation is legal or not
  if (!(glms_vec3_angle(newOrientation, self->up) <= glm_rad(5.f) || glms_vec3_angle(newOrientation, upOpposite) <= glm_rad(5.f)))
    self->orientation = newOrientation;

  // Rotates the Orientation left and right
  self->orientation = glms_vec3_rotate(self->orientation, glm_rad(-rotY), self->up);
}

