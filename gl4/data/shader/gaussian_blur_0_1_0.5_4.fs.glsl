/*
 * mean = 0
 * variance = 1
 * gaussian_step = 0.5
 * RADIUS = 4
 */
#version 330 core

#define RADIUS 4

in vs_out
{
 vec2 texcoord;
}fi;

uniform bool horizontal = true;
uniform sampler2D quad_map;

out vec4 frag_color;

const float weights[5] = float[5](0.204164, 0.180174, 0.123832, 0.0662822, 0.0276306);

void main(void) {
  vec2 tex_step = 1.0 / textureSize(quad_map, 0);
  if (horizontal) {
    tex_step.y = 0.0;
  } else {
    tex_step.x = 0.0;
  }

  vec4 color = weights[0] * texture2D(quad_map, fi.texcoord);
  for (int i = 1; i <= RADIUS; i++) {
    color += weights[i] * texture2D(quad_map, fi.texcoord + tex_step * i);
    color += weights[i] * texture2D(quad_map, fi.texcoord - tex_step * i);
  }
  frag_color = color;
}
