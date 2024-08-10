#ifndef FBO_H
#define FBO_H

struct FBO {
  GLuint id;
  GLsizei size;
};

[[nodiscard]]
struct FBO fboCreate(GLsizei size);

void fboBind(GLenum target,const struct FBO* self);
void fboUnbind(void);
void fboDelete(const struct FBO* self);

#endif

