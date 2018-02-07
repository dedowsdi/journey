varying vec3 v_vertex;
varying vec2 m_texcoord;
varying mat3 tbn;

uniform mat4 mv_mat_it;
uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];
uniform sampler2D normal_map;

void main(void)
{
  vec3 normal = texture2D(normal_map, m_texcoord).xyz * 2 - 1; 
  // tbn is an orthogonal basis
  normal = normalize(mat3(mv_mat_it) * (tbn * normal));

  gl_FragColor = blinn(v_vertex, normal, vec3(0), mtl, lights, lm);
}
