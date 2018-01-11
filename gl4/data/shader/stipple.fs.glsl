#version 430 core

noperspective in float stippleCount;

// there is not ushort in glsl, only lowerest 16 bits are used
uniform uint pattern = 0xFFFF;
uniform int factor = 1;
uniform vec4 color = vec4(0.0f, 0.0f, 0.0f, 1.0f);


out vec4 fragColor;

void main() {

  uint bit = uint(round(stippleCount/factor)) & 0xf;
  if ((pattern & (1U << bit)) == 0U) discard;

  fragColor = color;
}
