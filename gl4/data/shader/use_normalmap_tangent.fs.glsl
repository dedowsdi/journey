#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

in vs_out{
  vec3 tangent_vertex;
  vec3 tangent_camera;
  vec2 texcoord;
  light_source tangent_lights[LIGHT_COUNT];
} fi;

uniform sampler2D normal_map;

out vec4 frag_color;

void main(void)
{
  vec3 normal = texture(normal_map, fi.texcoord).xyz * 2 - 1; 
  frag_color = blinn(fi.tangent_vertex, normal, fi.tangent_camera, fi.tangent_lights);
}
