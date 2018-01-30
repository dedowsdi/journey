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

out vec4 frag_color;

void main(void) {
  vec3 vertex = texture(g_vertex, fi.texcoord).xyz;
  vec3 normal = normalize(texture(g_normal, fi.texcoord).xyz);

  material material;

  material.emission = texture(g_emission, fi.texcoord);
  material.ambient = texture(g_ambient, fi.texcoord);
  material.diffuse = texture(g_diffuse, fi.texcoord);
  material.specular = texture(g_specular, fi.texcoord);
  material.shininess = texture(g_shininess, fi.texcoord).r * 255;

  frag_color = blinn(vertex, normal, material);
  gl_FragDepth = texture(g_depth, fi.texcoord).r;
}
