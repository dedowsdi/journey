in vs_out{
  vec2 texcoord;
} fi;

uniform sampler2D g_vertex;
uniform sampler2D g_normal;
uniform sampler2D g_emission;
uniform sampler2D g_ambient;
uniform sampler2D g_diffuse;
uniform sampler2D g_specular;
uniform sampler2D g_shininess;
uniform sampler2D g_depth;

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

out vec4 frag_color;

void main(void) {
  vec3 vertex = texture(g_vertex, fi.texcoord).xyz;
  vec3 normal = normalize(texture(g_normal, fi.texcoord).xyz);

  material mtl;

  mtl.emission = texture(g_emission, fi.texcoord);
  mtl.ambient = texture(g_ambient, fi.texcoord);
  mtl.diffuse = texture(g_diffuse, fi.texcoord);
  mtl.specular = texture(g_specular, fi.texcoord);
  mtl.shininess = texture(g_shininess, fi.texcoord).r * 255;

  frag_color = blinn(vertex, normal, vec3(0), mtl, lights, lm);
  gl_FragDepth = texture(g_depth, fi.texcoord).r;
}
