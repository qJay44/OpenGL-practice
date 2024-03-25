#ifndef VAO_H
#define VAO_H

// Vertex Array Object
struct VAO {
  GLuint id;
  GLsizei size;
};

struct VAO vaoCreate(GLsizei size) {
  struct VAO vao;
  glGenVertexArrays(size, &vao.id);
  vao.size = size;

  return vao;
}

void vaoBind(const struct VAO* vao) {
  glBindVertexArray(vao->id);
}

void vaoLink(const struct VAO* vao, GLuint layout) {
  glVertexAttribPointer(layout, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (void*)0);
  glEnableVertexAttribArray(layout);
}

void vaoUnbind(void) {
  glBindVertexArray(0);
}

void vaoDelete(const struct VAO* vao) {
  glDeleteVertexArrays(vao->size, &vao->id);
}

#endif

