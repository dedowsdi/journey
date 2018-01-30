#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

in vs_out{
  vec3 t_vertex;
  vec3 t_camera;
  vec2 texcoord;
  light_source t_lights[LIGHT_COUNT];
} fi;

uniform sampler2D normal_map;

out vec4 frag_color;

void main(void)
{
  vec3 normal = texture(normal_map, fi.texcoord).xyz * 2 - 1; 
  frag_color = blinn(fi.t_vertex, normal, fi.t_camera, fi.t_lights);
}
