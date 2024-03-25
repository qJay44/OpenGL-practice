#ifndef EBO_H
#define EBO_H

// Element Buffer Object
struct EBO {
  GLuint id;
  GLsizei size;
};

struct EBO eboCreate(GLsizei size) {
  struct EBO ebo;
  glGenBuffers(size, &ebo.id);
  ebo.size = size;

  return ebo;
}

void eboBind(const struct EBO* ebo, const void* data, GLsizeiptr dataSize) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo->id);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data, GL_STATIC_DRAW);
}

void eboUnbind(void) {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void eboDelete(const struct EBO* ebo) {
  glDeleteBuffers(ebo->size, &ebo->id);
}

#endif

