#ifndef UTILS_H
#define UTILS_H

#include "cglm/types-struct.h"

#define DEFAULT_BUFFER_ITEMS 0xff

[[nodiscard]]
char* readFile(const char* path, bool printContent);

void arrResizeChar(char** arr, u32 newSize);
void arrResizeFloat(float** arr, u32 newSize);
void arrResizeUint(uint** arr, u32 newSize);
void arrResizeVec2s(vec2s** arr, u32 newSize);
void arrResizeVec3s(vec3s** arr, u32 newSize);
void arrResizeVec4s(vec4s** arr, u32 newSize);
void concat(const char* s1, const char* s2, char* out, rsize_t outSize);

#endif

