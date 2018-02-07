varying vec3 v_vertex; // view space
varying vec3 v_normal; // view space
#ifdef WITH_TEX
varying vec2 m_texcoord;
#endif

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

#ifdef WITH_TEX
uniform sampler2D diffuse_map;
#endif


void main(void) {
  vec3 normal = normalize(v_normal);
  gl_FragColor = blinn(v_vertex, normal, vec3(0), mtl, lights, lm);
#ifdef WITH_TEX
  gl_FragColor.xyz *= texture2D(diffuse_map, m_texcoord).xyz;
#endif
}
