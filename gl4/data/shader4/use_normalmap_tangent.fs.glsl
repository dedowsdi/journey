in vs_out{
  vec3 t_vertex;
  vec3 t_camera;
  vec2 texcoord;
  light_source t_lights[LIGHT_COUNT];
} fi;

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];
uniform sampler2D normal_map;

out vec4 frag_color;

void main(void)
{
  vec3 t_normal = normalize(texture(normal_map, fi.texcoord).xyz * 2 - 1); 
  frag_color = blinn(fi.t_vertex, t_normal, fi.t_camera, mtl, fi.t_lights, lm);
}
