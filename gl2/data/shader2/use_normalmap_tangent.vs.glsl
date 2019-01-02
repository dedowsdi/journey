#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

attribute vec4 vertex;
attribute vec3 normal;
attribute vec3 tangent;
attribute vec2 texcoord;

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

varying vec3 t_vertex; 
varying vec3 t_camera;
varying vec2 m_texcoord;
//varying light_source t_lights[LIGHT_COUNT];
varying vec4 t_light_positions[LIGHT_COUNT];
varying vec3 t_spot_directions[LIGHT_COUNT];

uniform light_source lights[LIGHT_COUNT]; //lights in model space
uniform vec3 m_camera;

void main(void)
{
  vec3 T = tangent;
  vec3 N = normal;
  vec3 B = normalize(cross(N, T));
  mat3 tbn = mat3(T, B, N);
  mat3 tbn_i = transpose(tbn);

  t_vertex = tbn_i * vertex.xyz;
  t_camera = tbn_i * m_camera;
  m_texcoord = texcoord;

  for (int i = 0; i < LIGHT_COUNT; i++) {
    t_light_positions[i] = mat4(tbn_i) * lights[i].position;
    t_spot_directions[i] = tbn_i * lights[i].spot_direction;
  }

  gl_Position = mvp_mat * vertex;
}
