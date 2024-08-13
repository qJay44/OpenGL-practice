#include "framebuffer.h"

#include <stdio.h>

#include "fbo.h"
#include "texture.h"

static void status() {
  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer error: %d\n", fboStatus);
}

Framebuffer framebufferCreate(void) {
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

Framebuffer framebufferCreateShadowMap(int w, int h) {
  Framebuffer fb;

  fb.fbo = fboCreate(1);
  fb.texture = textureCreateShadowMap(w, h);
  textureBind(&fb.texture);
  fboBind(GL_FRAMEBUFFER, &fb.fbo);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, fb.texture.id, 0);
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);
  fb.glType = GL_TEXTURE_2D;

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

void framebufferDraw(const Framebuffer* self, GLint shader, const struct VAO* vao, u32 vertCount) {
  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, "winWidth"), _gState.winWidth);
  glUniform1i(glGetUniformLocation(shader, "winHeight"), _gState.winHeight);
  glUniform1f(glGetUniformLocation(shader, "time"), _gState.time);
  glUniform1f(glGetUniformLocation(shader, "gamma"), _gState.gamma);
  vaoBind(vao);
  glDisable(GL_DEPTH_TEST);
  textureBind(&self->texture);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_DEPTH_TEST);
}

