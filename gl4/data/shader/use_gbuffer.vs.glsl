#version 430 core

out vs_out{
  vec2 texcoord ;
} vo;

const vec4 vertices[6] = vec4[6](
    vec4(-1, -1, 0, 0),
    vec4(1,  -1, 1, 0),
    vec4(1,  1,  1, 1),
    vec4(-1, -1, 0, 0),
    vec4(1,  1,  1, 1),
    vec4(-1, 1,  0, 1)
    );

void main(void)
{
  gl_Position = vec4(vertices[gl_VertexID].xy, 0, 1);
  vo.texcoord = vertices[gl_VertexID].zw;
}
