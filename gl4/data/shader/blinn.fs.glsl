in vs_out{
  vec3 view_vertex; // view space
  vec3 view_normal; // view space
#ifdef WITH_TEX
  vec2 texcoord;
#endif
} fi;

#ifdef WITH_TEX
uniform sampler2D diffuse_map;
#endif

out vec4 frag_color;

void main(void) {
  vec3 normal = normalize(fi.view_normal);
  frag_color = blinn(fi.view_vertex, normal);
#ifdef WITH_TEX
  frag_color.xyz *= texture(diffuse_map, fi.texcoord).xyz;
#endif
}
