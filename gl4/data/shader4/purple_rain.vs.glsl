#version 430 core

in vec4 vertex;
in vec4 posz; // position and size
//in mat4 mvp_mats;
uniform mat4 vp_mat;

void main(void)
{
  mat4 m = mat4(posz.w);
  m[3][3] = 1;
  m[3].xyz = posz.xyz;
  m = vp_mat * m ;

  //gl_Position = mvp_mats * vertex;
  gl_Position = m * vertex;
}
