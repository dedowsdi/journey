#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec2 texcoord;

uniform mat4 mvp_mat;

struct light_source {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 position;  
  vec3 spot_direction;
  float spot_exponent;
  float spot_cutoff;
  float spot_cos_cutoff;
  float constant_attenuation;
  float linear_attenuation;
  float quadratic_attenuation;
};

out vs_out{
  vec3 t_vertex; 
  vec3 t_camera;
  vec2 texcoord;
  light_source t_lights[LIGHT_COUNT];
} vo;

uniform light_source lights[LIGHT_COUNT]; //lights in model space
uniform vec3 m_camera;

void main(void)
{
  vec3 T = tangent;
  vec3 N = normal;
  vec3 B = normalize(cross(N, T));
  mat3 tbn = mat3(T, B, N);
  mat3 tbn_i = transpose(tbn);

  vo.t_vertex = tbn_i * vertex.xyz;
  vo.t_camera = tbn_i * m_camera;
  vo.texcoord = texcoord;

  vo.t_lights = lights;
  for (int i = 0; i < LIGHT_COUNT; i++) {
    vo.t_lights[i].position = mat4(tbn_i) * vo.t_lights[i].position;
    vo.t_lights[i].spot_direction = tbn_i * vo.t_lights[i].spot_direction;
  }

  gl_Position = mvp_mat * vertex;
}
