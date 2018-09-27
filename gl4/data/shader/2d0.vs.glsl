#version 330 core

in vec4 vertex;

#ifdef INSTANCE_TRANSLATION
uniform mat4 mvp_mat
#else
uniform mat4 mvp_mat;
#endif

void main(void)
{
  gl_Position = mvp_mat * vertex;
}
