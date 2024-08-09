#include "vbo.h"

struct VBO vboCreate(GLsizei size, const void* data, GLsizeiptr dataSize) {
  struct VBO vbo;
  glGenBuffers(size, &vbo.id);
  vbo.size = size;
  vboBind(&vbo);
  glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);

  return vbo;
}

void vboBind(const struct VBO* self) {
  glBindBuffer(GL_ARRAY_BUFFER, self->id);
}

void vboUnbind(void) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vboDelete(const struct VBO* self) {
  glDeleteBuffers(self->size, &self->id);
}

