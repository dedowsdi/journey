/*
 * mean = 0
 * stand deviation = 1
 * gaussian_step = 0.5
 * radius = 4
 */
#version 120
#extension GL_EXT_gpu_shader4 : require

#define radius 4

varying vec2 m_texcoord;

uniform bool horizontal = true;
uniform sampler2D quad_map;

const float weights[5] = float[5](0.204164, 0.180174, 0.123832, 0.0662822, 0.0276306);

void main(void) {
  vec2 tex_step = 1.0 / textureSize2D(quad_map, 0);
  if (horizontal) {
    tex_step.y = 0.0;
  } else {
    tex_step.x = 0.0;
  }

  vec4 color = weights[0] * texture2D(quad_map, m_texcoord.xy);
  for (int i = 1; i <= radius; i++) {
    color += weights[i] * texture2D(quad_map, m_texcoord.xy + tex_step * i);
    color += weights[i] * texture2D(quad_map, m_texcoord.xy - tex_step * i);
  }
  gl_FragColor = color;
}
