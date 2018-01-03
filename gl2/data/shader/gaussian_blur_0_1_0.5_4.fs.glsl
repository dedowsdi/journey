/*
 * mean = 0
 * stand deviation = 1
 * gaussianStep = 0.5
 * radius = 4
 */
#version 120
#extension GL_EXT_gpu_shader4 : require

#define radius 4

uniform bool horizontal = true;

uniform sampler2D colorMap;

const float weights[5] = float[5](0.204164, 0.180174, 0.123832, 0.0662822, 0.0276306);

void main(void) {
  vec2 texStep = 1.0 / textureSize2D(colorMap, 0);
  if (horizontal) {
    texStep.y = 0.0;
  } else {
    texStep.x = 0.0;
  }

  vec4 color = weights[0] * texture2D(colorMap, gl_TexCoord[0].xy);
  for (int i = 1; i <= radius; i++) {
    color += weights[i] * texture2D(colorMap, gl_TexCoord[0].xy + texStep * i);
    color += weights[i] * texture2D(colorMap, gl_TexCoord[0].xy - texStep * i);
  }
  gl_FragColor = color;
}
