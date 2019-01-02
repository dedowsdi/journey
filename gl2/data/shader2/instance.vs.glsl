#version 120 

attribute vec4 vertex;
attribute vec3 normal;
// if you have too many instances, you will run out of uniform, it'd better to
// use attribute to pass instance data such as transform
attribute mat4 mvp_mat;
attribute mat4 mv_mat;
attribute mat4 mv_mat_it;

varying vec3 v_normal; // view space
varying vec3 v_vertex; // view space

void main(void)
{
  v_normal = mat3(mv_mat_it) * normal;
  v_vertex = (mv_mat * vertex).xyz;
  gl_Position = mvp_mat * vertex;
}
