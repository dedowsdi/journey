//#version 430 core

#ifndef RADIUS
#define RADIUS 4
#endif

in vs_out
{
 vec2 texcoord;
}fi;

uniform bool horizontal = true;
uniform float weights[32]; // 32 must be more than enough, only radius + 1 items
                           // will be used
uniform sampler2D quad_map;

out vec4 frag_color;

void main(void) {
  vec2 tex_step = 1.0 / textureSize(quad_map, 0);
  if (horizontal) {
    tex_step.y = 0.0;
  } else {
    tex_step.x = 0.0;
  }

  vec4 color = weights[0] * texture(quad_map, fi.texcoord);
  for (int i = 1; i <= RADIUS; i++) {
    color += weights[i] * texture(quad_map, fi.texcoord + tex_step * i);
    color += weights[i] * texture(quad_map, fi.texcoord - tex_step * i);
  }
  frag_color = color;
}
