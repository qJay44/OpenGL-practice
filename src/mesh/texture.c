#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "texture.h"

static u8 unit2D = 0;

Texture textureCreate2D(const char* path, enum TextureEnum type) {
  assert(unit2D < GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS);

  int imgWidth, imgHeight, imgColorChannels;
  stbi_set_flip_vertically_on_load(true);
  byte* imgBytes = stbi_load(path, &imgWidth, &imgHeight, &imgColorChannels, 0);

  GLuint textureId;
  glGenTextures(1, &textureId);
  glBindTexture(GL_TEXTURE_2D, textureId);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  switch (imgColorChannels) {
    case 1:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, imgWidth, imgHeight, 0, GL_RED, GL_UNSIGNED_BYTE, imgBytes);
      break;
    case 3:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, imgWidth, imgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, imgBytes);
      break;
    case 4:
      glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, imgWidth, imgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBytes);
      break;
    default:
      printf("Unhandled texture image color channels type (%d)\n", imgColorChannels);
      printf("Path: %s\n", path);
      printf("stbi_failure_reason: %s\n", stbi_failure_reason());
      exit(EXIT_FAILURE);
  }

  glGenerateMipmap(GL_TEXTURE_2D);

  Texture tex;
  tex.id = textureId;
  tex.unit = unit2D++;
  tex.glType = GL_TEXTURE_2D,
  tex.type = type;
  sprintf(tex.name, "%s", getFileNameFromPath(path));

  stbi_image_free(imgBytes);
  textureUnbind(GL_TEXTURE_2D);

  return tex;
}

Texture* textureCreateCubemap(const char* dirPath)  {
  static const char* texNames[6] = { // Order matters
    "right",
    "left",
    "top",
    "bottom",
    "front",
    "back",
  };

  GLuint cubemap;
  glGenTextures(1, &cubemap);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  // ========== Find out the extension of the textures ==========  //

  // NOTE: Assuming all files have the same extention (per folder)

  char imgPath[256];
  sprintf(imgPath, "%s/*.*", dirPath);

  WIN32_FIND_DATA fdFile;
  HANDLE hFind = NULL;
  if((hFind = FindFirstFile(imgPath, &fdFile)) == INVALID_HANDLE_VALUE)
    printf("Path not found: [%s]\n", dirPath);

  //Find first file will always return "."
  //    and ".." as the first two directories.
  FindNextFile(hFind, &fdFile);
  FindNextFile(hFind, &fdFile);
  if (fdFile.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
    printf("Warning found directory in the skybox folder: [%s]\n", fdFile.cFileName);

  FindNextFile(hFind, &fdFile);
  sprintf(imgPath, "%s/%s", dirPath, fdFile.cFileName);

  char* extension = getExtensionFromFileName(fdFile.cFileName);

  // ============================================================  //

  for (int i = 0; i < 6; i++) {
    char path[256];
    sprintf(path, "%s/%s.%s", dirPath, texNames[i], extension);

    int width, height, channels;
    byte* data = stbi_load(path, &width, &height, &channels, 0);

    if (data) {
      stbi_set_flip_vertically_on_load(false);
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    } else {
      printf("Failed to load texture: %s\n", path);
    }
    stbi_image_free(data);
  }

  FindClose(hFind);
  textureUnbind(GL_TEXTURE_CUBE_MAP);

  Texture* tex = malloc(sizeof(Texture)); // Freeing in "textureDelete"
  tex->id = cubemap;
  tex->unit = 0;
  tex->glType = GL_TEXTURE_CUBE_MAP,
  tex->type = TEXTURE_CUBEMAP;
  sprintf(tex->name, "%s", "CUBEMAP");

  return tex;
}

Texture textureCreateFramebuffer(GLenum targetType) {
  GLuint texId;
  Texture tex;

  glGenTextures(1, &texId);
  glBindTexture(targetType, texId);

  switch (targetType) {
    case GL_TEXTURE_2D_MULTISAMPLE:
      glTexImage2DMultisample(targetType, _gState.aaSamples, GL_RGB16F, _gState.winWidth, _gState.winHeight, GL_TRUE);
      break;
    case GL_TEXTURE_2D:
      glTexImage2D(targetType, 0, GL_RGB16F, _gState.winWidth, _gState.winHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
      break;
    default:
      printf("(textureCreateFramebuffer): Unhandled targetType\n");
      exit(EXIT_FAILURE);
  }

  glTexParameteri(targetType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(targetType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(targetType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(targetType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, targetType, texId, 0);

  tex.id = texId;
  tex.glType = targetType,
  tex.unit = 0;
  tex.type = TEXTURE_FRAMEBUFFER;
  sprintf(tex.name, "%s", "FRAMEBUFFER");

  textureUnbind(targetType);

  return tex;
}

Texture textureCreateShadowMap(int w, int h) {
  GLuint texId;
	glGenTextures(1, &texId);
	glBindTexture(GL_TEXTURE_2D, texId);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
  float clampColor[] = {1.f, 1.f, 1.f, 1.f};
  glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, clampColor);

  Texture tex;
  tex.id = texId;
  tex.glType = GL_TEXTURE_2D,
  tex.unit = unit2D++;
  tex.type = TEXTURE_SHADOWMAP;
  sprintf(tex.name, "%s", "SHADOWMAP");

  textureUnbind(GL_TEXTURE_2D);

  return tex;
}

void textureBind(const Texture* self) {
  glActiveTexture(GL_TEXTURE0 + self->unit);
  glBindTexture(self->glType, self->id);
}

void textureUnbind(GLenum texType) {
  glBindTexture(texType, 0);
}

void textureSetUniform(GLint shader, const char* uniform, GLuint unit) {
  glUseProgram(shader);
  glUniform1i(glGetUniformLocation(shader, uniform), unit);
}

void textureDelete(Texture* self, GLsizei num) {
  glDeleteTextures(num, &self->id);
  if (self->glType ==  GL_TEXTURE_CUBE_MAP)
    free(self);
}

