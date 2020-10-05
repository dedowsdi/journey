#version 330

in layout(location = 0) vec4 vertex;

void main(void)
{
    gl_Position = vertex;
}
