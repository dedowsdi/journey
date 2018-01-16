#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec2 texcoord;

uniform mat4 modelViewProjMatrix;

struct LightSource {
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  vec4 position;  
  vec3 spotDirection;
  float spotExponent;
  float spotCutoff;
  float spotCosCutoff;
  float constantAttenuation;
  float linearAttenuation;
  float quadraticAttenuation;
};

out VS_OUT{
  vec3 tangentVertex; 
  vec3 tangentCamera;
  vec2 texcoord;
  LightSource tangentLights[LIGHT_COUNT];
} vs_out;

uniform LightSource lights[LIGHT_COUNT]; //lights in model space
uniform vec3 modelCamera;

void main(void)
{
  vec3 T = tangent;
  vec3 N = normal;
  vec3 B = normalize(cross(N, T));
  mat3 tbn = mat3(T, B, N);
  mat3 tbnInverse = transpose(tbn);

  vs_out.tangentVertex = (mat4(tbnInverse) * vertex).xyz;
  vs_out.tangentCamera = tbnInverse * modelCamera;
  vs_out.texcoord = texcoord;

  vs_out.tangentLights = lights;
  for (int i = 0; i < LIGHT_COUNT; i++) {
    vs_out.tangentLights[i].position = mat4(tbnInverse) *
      vs_out.tangentLights[i].position;
    vs_out.tangentLights[i].spotDirection = tbnInverse *
      vs_out.tangentLights[i].spotDirection;
  }

  gl_Position = modelViewProjMatrix * vertex;
}
