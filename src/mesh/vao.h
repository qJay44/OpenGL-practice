#ifndef VAO_H
#define VAO_H

// Vertex Array Object
struct VAO {
  GLuint id;
  GLsizei size;
};

[[nodiscard]]
struct VAO vaoCreate(GLsizei size);

void vaoBind(const struct VAO* self);
void vaoLinkAttrib(GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
void vaoUnbind(void);
void vaoDelete(const struct VAO* self);

#endif

