in vs_out{
  vec3 vertex; 
  vec2 texcoord;
  mat3 tbn;
} fi;

uniform vec3 m_camera;
uniform sampler2D normal_map;

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

out vec4 frag_color;

void main(void)
{
  vec3 normal = texture(normal_map, fi.texcoord).xyz * 2 - 1;
  normal = normalize(fi.tbn * normal);
  frag_color = blinn(fi.vertex, normal, m_camera, mtl, lights, lm);
}
