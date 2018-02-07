/*
 * read_image.c
 *  A helper function to read the binary images included with this
 *  program distribution.
 */

#include "glad/glad.h"
#include <stdio.h>
#include <stdlib.h>
#include "common.h"

// from big endian to small endian
static unsigned int byte_to_u_int32(const unsigned char buf[4]) {
  return (buf[3] & 0x000000ff) | ((buf[2] << 8) & 0x0000ff00) |
         ((buf[1] << 16) & 0x00ff0000) | (buf[0] << 24);
}

GLubyte* read_image(const char* filename, GLsizei* width, GLsizei* height) {
  int n;
  GLubyte* pixels;

  FILE* infile = fopen(filename, "rb");

  if (!infile) {
    fprintf(stderr, "unable to open file '%s'\n", filename);
    return NULL;
  }

  //image file of redbook is in big endian format, be careful
  fread(width, sizeof(GLsizei), 1, infile);
  fread(height, sizeof(GLsizei), 1, infile);
  *width = byte_to_u_int32((unsigned char*)width);
  *height = byte_to_u_int32((unsigned char*)height);

  n = 3 * (*width) * (*height);

  pixels = (GLubyte*)malloc(n * sizeof(GLubyte));
  if (!pixels) {
    fprintf(stderr, "unable to malloc() bytes for pixels\n");
    return NULL;
  }

  fread(pixels, sizeof(GLubyte), n, infile);

  fclose(infile);

  return pixels;
}
