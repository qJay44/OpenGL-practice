#ifndef VBO_H
#define VBO_H

// Vertex Buffer Object
struct VBO {
  GLuint id;
  GLsizei size;
};

struct VBO vboCreate(GLsizei size) {
  struct VBO vbo;
  glGenBuffers(size, &vbo.id);
  vbo.size = size;

  return vbo;
}

void vboBind(const struct VBO* vbo, const void* data, GLsizeiptr dataSize) {
  glBindBuffer(GL_ARRAY_BUFFER, vbo->id);
  glBufferData(GL_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
}

void vboUnbind(void) {
  glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void vboDelete(const struct VBO* vbo) {
  glDeleteBuffers(vbo->size, &vbo->id);
}

#endif

