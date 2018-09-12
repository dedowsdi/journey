#version 430 core

uniform sampler2D diffuse_map;
out vec4 frag_color;

void main(void)
{
  frag_color = texture(diffuse_map, gl_PointCoord);
}
