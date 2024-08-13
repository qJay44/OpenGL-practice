#include "framebuffer.h"

#include <assert.h>

#include "fbo.h"
#include "texture.h"

Framebuffer framebufferCreate(enum FramebufferEnum fbType) {
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

void framebufferBindReadDraw(const Framebuffer* read, const Framebuffer* draw) {
  assert(fbMultiSample->type == FRAMEBUFFER_MS);

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

