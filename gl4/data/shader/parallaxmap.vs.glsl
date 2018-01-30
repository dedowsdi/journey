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
  vec3 tangent_vertex; 
  vec3 tangent_camera;
  vec2 texcoord;
  light_source tangent_lights[LIGHT_COUNT];
} vo;

uniform light_source lights[LIGHT_COUNT]; //lights in model space
uniform vec3 model_camera;

void main(void)
{
  vec3 T = tangent;
  vec3 N = normal;
  vec3 B = normalize(cross(N, T));
  mat3 tbn = mat3(T, B, N);
  mat3 tbn_i = transpose(tbn);

  vo.tangent_vertex = (mat4(tbn_i) * vertex).xyz;
  vo.tangent_camera = tbn_i * model_camera;
  vo.texcoord = texcoord;

  vo.tangent_lights = lights;
  for (int i = 0; i < LIGHT_COUNT; i++) {
    vo.tangent_lights[i].position = mat4(tbn_i) *
      vo.tangent_lights[i].position;
    vo.tangent_lights[i].spot_direction = tbn_i *
      vo.tangent_lights[i].spot_direction;
  }

  gl_Position = mvp_mat * vertex;
}
