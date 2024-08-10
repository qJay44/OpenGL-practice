#include "framebuffer.h"

#include <assert.h>

#include "fbo.h"
#include "texture.h"

Framebuffer framebufferCreate(GLenum fbType) {
  Framebuffer fb;

  fb.fbo = fboCreate(1);
  fboBind(GL_FRAMEBUFFER, &fb.fbo);
  fb.texture = textureCreateFramebuffer(fbType);
  fb.glType = fbType;

  GLenum fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (fboStatus != GL_FRAMEBUFFER_COMPLETE)
    printf("Framebuffer error: %d\n", fboStatus);

  fboUnbind();

  return fb;
}

void framebufferBind(const Framebuffer* self) {
  fboBind(GL_FRAMEBUFFER, &self->fbo);
}

void framebufferBindMultiSample(const Framebuffer* fbMultiSample, const Framebuffer* fbDefault) {
  assert(fbMultiSample->type == FRAMEBUFFER_T_MS);

  fboBind(GL_READ_FRAMEBUFFER, &fbMultiSample->fbo);
  fboBind(GL_DRAW_FRAMEBUFFER, &fbDefault->fbo);
  glBlitFramebuffer(0, 0, _gState.winWidth, _gState.winHeight, 0, 0, _gState.winWidth, _gState.winHeight, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void framebufferUnbind(void) {
  fboUnbind();
}

void framebufferDraw(const Framebuffer* self, GLint shader, const struct VAO* vao, u32 vertCount, double time) {
  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, "winWidth"), _gState.winWidth);
  glUniform1i(glGetUniformLocation(shader, "winHeight"), _gState.winHeight);
  glUniform1f(glGetUniformLocation(shader, "time"), time);
  glDisable(GL_DEPTH_TEST);
  vaoBind(vao);
  textureBind(&self->texture);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glEnable(GL_DEPTH_TEST);
}

