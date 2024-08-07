#include <stdio.h>
#include <stdlib.h>

#include "shader.h"

static GLuint load(const char* path, int type) {
  char* shaderStr = readFile(path, false);
  const char* shaderStrPtr = shaderStr;
  GLuint shaderId = glCreateShader(type);
  glShaderSource(shaderId, 1, &shaderStrPtr, NULL);

  free(shaderStr);
  return shaderId;
}

static GLint compile(const char* path, int type) {
  GLint shaderId = load(path, type);

  GLint hasCompiled;
  char infoLog[1024];

  glCompileShader(shaderId);
  glGetShaderiv(shaderId, GL_COMPILE_STATUS, &hasCompiled);

  // if GL_FALSE
  if (!hasCompiled) {
    glGetShaderInfoLog(shaderId, 1024, NULL, infoLog);
    printf("\nShader: %s\n", path);
    printf("\n===== Shader compilation error =====\n\n%s", infoLog);
    printf("====================================\n\n");
  }

  return shaderId;
}

static void link(GLuint program) {
  GLint hasLinked;
  char infoLog[1024];

  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &hasLinked);

  // if GL_FALSE
  if (!hasLinked) {
    glGetProgramInfoLog(program, 1024, NULL, infoLog);
    printf("\n===== Shader link error =====\n\n%s", infoLog);
    printf("\n=============================\n\n");
  }
}

GLint shaderCreate(const char* vsPath, const char* fsPath, const char* gsPath) {
  GLint program = glCreateProgram();
  GLint shaders[3];
  u8 idx = 0;

  shaders[idx++] = compile(vsPath, GL_VERTEX_SHADER);
  shaders[idx++] = compile(fsPath, GL_FRAGMENT_SHADER);

  if (gsPath)
    shaders[idx++] = compile(gsPath, GL_GEOMETRY_SHADER);

  for (u8 i = 0; i < idx; i++)
    glAttachShader(program, shaders[i]);

  link(program);

  for (u8 i = 0; i < idx; i++)
    glDeleteShader(shaders[i]);

  return program;
}

