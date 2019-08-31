#ifndef LIGHT_COUNT
#define LIGHT_COUNT 8
#endif

in vec4 vertex;
in vec3 normal;
in vec3 tangent;
in vec2 texcoord;

uniform mat4 mvp_mat;

out vs_out{
  vec3 vertex; 
  vec2 texcoord;
  mat3 tbn;
} vo;

void main(void)
{
  vo.vertex = vertex.xyz / vertex.w;
  vo.texcoord = texcoord;

  vec3 B = cross(normal, tangent);
  vo.tbn = mat3(tangent, B, normal);

  gl_Position = mvp_mat * vertex;
}
