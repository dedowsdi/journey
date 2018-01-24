in VS_OUT{
  vec3 viewVertex; // view space
  vec3 viewNormal; // view space
#ifdef WITH_TEX
  vec2 texcoord;
#endif
} fs_in;

#ifdef WITH_TEX
uniform sampler2D diffuseMap;
#endif

out vec4 fragColor;

void main(void) {
  vec3 normal = normalize(fs_in.viewNormal);
  fragColor = blinn(fs_in.viewVertex, normal);
#ifdef WITH_TEX
  fragColor.xyz *= texture(diffuseMap, fs_in.texcoord).xyz;
#endif
}
