#version 330

layout(location = 0) in vec4 vertex;
layout(location = 1) in vec4 color;

uniform mat4 mvp_mat;

out vertex_data
{
    vec4 color;
}vo;

void main(void)
{
    gl_Position = mvp_mat * vertex;
    vo.color = color;
}
