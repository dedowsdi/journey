/*
 * Just a demostration of two pass gaussian blur, you should precaculate weights
 * if you want to use it.
 */
#extension GL_EXT_gpu_shader4 : require

uniform float mean;
uniform float deviation;
uniform bool horizontal = true;
uniform float gaussianStep;

uniform sampler2D colorMap;

#ifndef radius
#define radius 4
#endif /* ifndef radius */

const float pi = 3.14159265358979323846264338327950288419716939937510;
const float e = 2.71828182845904523536028747135266249775724709369995;
float weights[radius+1];

void main(void) {
  float r = 1.0 / sqrt(2.0 * pi);
  vec2 texelStep = 1.0 / textureSize2D(colorMap, 0);
  if (horizontal) {
    texelStep.y = 0.0;
  } else {
    texelStep.x = 0.0;
  }
  float rd = 1.0 / deviation;

  // calculate weights
  for (int i = 0; i <= radius; i++) {
    weights[i] =
      pow(e, -0.5 * pow((gaussianStep * i - mean) * rd, 2.0)) * rd * r;
  }
  // normalize weights
  float rw = weights[0];
  for (int i = 1; i <= radius; i++) {
    rw += weights[i] * 2;
  }
  rw = 1.0 / rw;

  vec4 color = weights[0] * rw * texture2D(colorMap, gl_TexCoord[0].xy);
  for (int i = 1; i <= radius; i++) {
    color +=
      weights[i] * rw * texture2D(colorMap, gl_TexCoord[0].xy + texelStep * i);
    color +=
      weights[i] * rw * texture2D(colorMap, gl_TexCoord[0].xy - texelStep * i);
  }
  gl_FragColor = color;
}
