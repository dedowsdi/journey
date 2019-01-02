//#version 430 core
in vs_out{
  vec3 v_vertex;
  vec2 texcoord;
  mat3 tbn;
} fi;

uniform mat4 mv_mat_it;
uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];
uniform sampler2D normal_map;

out vec4 frag_color;

void main(void)
{
  vec3 normal = texture(normal_map, fi.texcoord).xyz * 2 - 1; 
  // tbn is an orthogonal basis
  normal = normalize(mat3(mv_mat_it) * (fi.tbn * normal));

  frag_color = blinn(fi.v_vertex, normal, vec3(0), mtl, lights, lm);
}
