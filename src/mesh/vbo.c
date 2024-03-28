#include "vbo.h"

struct VBO vboCreate(GLsizei size) {
  struct VBO vbo;
  glGenBuffers(size, &vbo.id);
  vbo.size = size;

  return vbo;
}

void vboBind(const struct VBO* self, const void* data, GLsizeiptr dataSize) {
  glBindBuffer(GL_ARRAY_BUFFER, self->id);
  glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
}

void vboUnbind(void) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vboDelete(const struct VBO* self) {
  glDeleteBuffers(self->size, &self->id);
}

