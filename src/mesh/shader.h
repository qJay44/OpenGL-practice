#ifndef SHADER_H
#define SHADER_H

[[nodiscard]]
GLint shaderLoad(const char* path, int type);
void shaderCompile(GLuint shader);
void shaderProgramLink(GLuint program);

#endif

