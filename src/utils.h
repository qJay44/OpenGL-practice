#include <stdio.h>
#include <string.h>

#define TRUE 1
#define FALSE 0

void readFile(const char* path, char* out, int outSize, int printContent) {
  FILE* fptr;

  fopen_s(&fptr, path, "r");

  if (fptr) {
    char ch;
    int i = 0;

    if (printContent) {
      printf("\n====== File content (%s) ======\n", path);
      while (!feof(fptr)) {
        if (i >= outSize) {
          printf("\nFile read error: File size excesses given char array\n");
          return;
        }
        ch = fgetc(fptr);
        out[i++] = ch;
        printf("%c", ch);
      }

      // Print lower border
      printf("\n=============================");
      int pathLen = strlen(path);
      while (pathLen--) printf("=");
      printf("\n\n");

    } else
      while (!feof(fptr)) {
        if (i >= outSize) {
          printf("\nFile read error: File size excesses given char array\n");
          return;
        }
        out[i++] = fgetc(fptr);
      }
  } else
    printf("File can't be opened: %s\n", path);

  fclose(fptr);
}

