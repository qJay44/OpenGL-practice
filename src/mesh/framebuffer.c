#include "framebuffer.h"

#include <stdio.h>
#include <stdlib.h>

#include "fbo.h"
#include "texture.h"

static void status() {
  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer error: %d\n", fboStatus);
}

Framebuffer framebufferCreate() {
  Framebuffer fb;

  fb.fbo = fboCreate(1);
  fboBind(GL_FRAMEBUFFER, &fb.fbo);
  fb.glType = GL_TEXTURE_2D;
  fb.texture = textureCreateFramebuffer(fb.glType);

  status();
  fboUnbind();

  return fb;
}

Framebuffer framebufferCreateMSAA(void) {
  Framebuffer fb;

  fb.fbo = fboCreate(1);
  fboBind(GL_FRAMEBUFFER, &fb.fbo);
  fb.glType = GL_TEXTURE_2D_MULTISAMPLE;
  fb.texture = textureCreateFramebuffer(fb.glType);

  status();
  fboUnbind();

  return fb;
}

Framebuffer framebufferCreateShadowMap(enum ShadowMapEnum target, int w, int h) {
  Framebuffer fb;

  fb.fbo = fboCreate(1);

  switch (target) {
    case SHADOWMAP_DEFAULT:
      fb.texture = textureCreateShadowMap(w, h);
      textureBind(&fb.texture);
      fboBind(GL_FRAMEBUFFER, &fb.fbo);
      glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb.texture.id, 0);
      break;
    case SHADOWMAP_CUBEMAP:
      fb.texture = textureCreateShadowCubeMap(w, h);
      textureBind(&fb.texture);
      fboBind(GL_FRAMEBUFFER, &fb.fbo);
      glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, fb.texture.id, 0);
      break;
    default:
      printf("Unhandled framebuffer shadow map target\n");
      exit(EXIT_FAILURE);
  }
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  fb.glType = target;

  status();
  fboUnbind();

  return fb;
}

void framebufferBind(const Framebuffer* self) {
  fboBind(GL_FRAMEBUFFER, &self->fbo);
}

void framebufferBindReadDraw(const Framebuffer* read, const Framebuffer* draw) {
  fboBind(GL_READ_FRAMEBUFFER, &read->fbo);
  fboBind(GL_DRAW_FRAMEBUFFER, &draw->fbo);
  glBlitFramebuffer(0, 0, _gState.winWidth, _gState.winHeight, 0, 0, _gState.winWidth, _gState.winHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void framebufferUnbind(void) {
  fboUnbind();
}

void framebufferDraw(GLint shader, const struct VAO* vao, u32 vertCount) {
  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, "winWidth"), _gState.winWidth);
  glUniform1i(glGetUniformLocation(shader, "winHeight"), _gState.winHeight);
  glUniform1f(glGetUniformLocation(shader, "time"), _gState.time);
  glUniform1f(glGetUniformLocation(shader, "gamma"), _gState.gamma);
  vaoBind(vao);
  glDisable(GL_DEPTH_TEST);
  glDrawArrays(GL_TRIANGLES, 0, vertCount);
  glEnable(GL_DEPTH_TEST);
}

