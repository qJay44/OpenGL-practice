#ifndef EBO_H
#define EBO_H

// Element Buffer Object
struct EBO {
  GLuint id;
  GLsizei size;
};

struct EBO eboCreate(GLsizei size);
void eboBind(const struct EBO* self, const void* data, GLsizeiptr dataSize);
void eboUnbind(void);
void eboDelete(const struct EBO* self);

#endif

