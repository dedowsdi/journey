/*
 * just a demostration of two pass gaussian blur, you should precaculate weights
 * if you want to use it.
 */
#extension GL_EXT_gpu_shader4 : require

varying vec2 m_texcoord;

uniform float mean;
uniform float deviation;
uniform bool horizontal = true;
uniform float gaussian_step;

uniform sampler2D quad_map;

#ifndef radius
#define radius 4
#endif /* ifndef radius */

const float pi = 3.14159265358979323846264338327950288419716939937510;
const float e = 2.71828182845904523536028747135266249775724709369995;
float weights[radius+1];

void main(void) {
  float r = 1.0 / sqrt(2.0 * pi);
  vec2 texel_step = 1.0 / textureSize2D(quad_map, 0);
  if (horizontal) {
    texel_step.y = 0.0;
  } else {
    texel_step.x = 0.0;
  }
  float rd = 1.0 / deviation;

  // calculate weights
  for (int i = 0; i <= radius; i++) {
    weights[i] =
      pow(e, -0.5 * pow((gaussian_step * i - mean) * rd, 2.0)) * rd * r;
  }
  // normalize weights
  float rw = weights[0];
  for (int i = 1; i <= radius; i++) {
    rw += weights[i] * 2;
  }
  rw = 1.0 / rw;

  vec4 color = weights[0] * rw * texture2D(quad_map, m_texcoord.xy);
  for (int i = 1; i <= radius; i++) {
    color +=
      weights[i] * rw * texture2D(quad_map, m_texcoord.xy + texel_step * i);
    color +=
      weights[i] * rw * texture2D(quad_map, m_texcoord.xy - texel_step * i);
  }
  gl_FragColor = color;
}
