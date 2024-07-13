#ifndef UTILS_H
#define UTILS_H

#include "cglm/types-struct.h"
#include "mesh/object.h"

[[nodiscard]] char* readFile(const char* path, bool printContent);
[[nodiscard]] byte* readFileBytes(const char* path);
void int2str(int value, char* result, int base);

void arrResizeFloat(float** arr, size_t oldSize, size_t* outNewSize);
void arrResizeUint(u32** arr, size_t oldSize, size_t* outNewSize);
void arrResizeVec2s(vec2s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeVec3s(vec3s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeVec4s(vec4s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeMat4s(mat4s** arr, size_t oldSize, size_t* outNewSize);
void arrResizeObject(Object** arr, size_t oldSize, size_t* outNewSize);
void arrResizeTexture(Texture** arr, size_t oldSize, size_t* outNewSize);
void arrResizeChar(char** arr, size_t oldSize, size_t* outNewSize);
void arrResizeCharPtr(char*** arr, size_t oldSize, size_t* outNewSize);
void arrResizeByte(byte** arr, size_t oldSize, size_t* outNewSize);
void concat(const char* s1, const char* s2, char* out, rsize_t outSize);

#endif

