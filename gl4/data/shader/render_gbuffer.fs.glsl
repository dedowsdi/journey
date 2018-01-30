#version 430 core

layout(location = 0) out vec3 g_vertex;
layout(location = 1) out vec3 g_normal;
layout(location = 2) out vec4 g_emission;
layout(location = 3) out vec4 g_ambient;
layout(location = 4) out vec4 g_diffuse;
layout(location = 5) out vec4 g_specular;
layout(location = 6) out float g_shininess;

in vs_out{
  vec3 view_vertex; // view space
  vec3 view_normal; // view space
} fi;

struct material {
  vec4 emission;
  vec4 ambient;
  vec4 diffuse;
  vec4 specular;
  float shininess;
};
uniform material mtl;

void main(void)
{
  g_vertex = fi.view_vertex;
  g_normal = fi.view_normal;
  g_emission = mtl.emission;
  g_ambient = mtl.ambient;
  g_diffuse = mtl.diffuse;
  g_specular = mtl.specular;
  g_shininess = mtl.shininess / 255;
}
