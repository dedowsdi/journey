#version 430 core

layout(location = 0) out vec3 gVertex;
layout(location = 1) out vec3 gNormal;
layout(location = 2) out vec4 gEmission;
layout(location = 3) out vec4 gAmbient;
layout(location = 4) out vec4 gDiffuse;
layout(location = 5) out vec4 gSpecular;
layout(location = 6) out float gShininess;

in VS_OUT{
  vec3 viewVertex; // view space
  vec3 viewNormal; // view space
} fs_in;

struct Material {
  vec4 emission;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
uniform Material material;

void main(void)
{
  gVertex = fs_in.viewVertex;
  gNormal = fs_in.viewNormal;
  gEmission = material.emission;
  gAmbient = material.ambient;
  gDiffuse = material.diffuse;
  gSpecular = material.specular;
  gShininess = material.shininess / 255;
}
