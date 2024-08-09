#ifndef UTILS_H
#define UTILS_H

#include "cglm/types-struct.h"
#include "mesh/object.h"

[[nodiscard]] char* readFile(const char* path, bool printContent);
[[nodiscard]] byte* readFileBytes(const char* path, size_t* outSize);
[[nodiscard]] char* getFileNameFromPath(const char* path);
[[nodiscard]] char* getExtensionFromFileName(const char* path);
[[nodiscard]] int randBetween(int min, int max);

bool listDirectoryContents(const char *sDir);

void arrResizeFloat(float** arr, size_t oldSize, size_t* outNewSize);
void arrResizeUint(u32** arr, size_t oldSize, size_t* outNewSize);
void arrResizeVec2s(vec2s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeVec3s(vec3s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeVec4s(vec4s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeVersors(versors** arr, size_t oldSize, size_t* outNewSize);
void arrResizeMat4s(mat4s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeObject(Object** arr, size_t oldSize, size_t* outNewSize);
void arrResizeChar(char** arr, size_t oldSize, size_t* outNewSize);
void arrResizeByte(byte** arr, size_t oldSize, size_t* outNewSize);

#endif

