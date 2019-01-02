in vs_out{
  vec3 v_vertex; // view space
  vec3 v_normal; // view space
#ifdef WITH_TEX
  vec2 texcoord;
#endif
} fi;

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

#ifdef WITH_TEX
uniform sampler2D diffuse_map;
#endif

out vec4 frag_color;

void main(void) {
  vec3 normal = normalize(fi.v_normal);
  frag_color = blinn(fi.v_vertex, normal, vec3(0), mtl, lights, lm);
#ifdef WITH_TEX
  frag_color.xyz *= texture(diffuse_map, fi.texcoord).xyz;
#endif
}
