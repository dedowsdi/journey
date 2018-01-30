/*
 * Align billboard rotation to camera rotation(not facing camera).
 */
#version 330 core

in vec4 vertex; // x y s t
in vec3 translation; // instance translation

out vs_out{
  vec2 texcoord;
}vo;

uniform mat4 v_mat;
uniform mat4 vp_mat;

void main() {
  vec3 w_xa = vec3(v_mat[0][0], v_mat[1][0], v_mat[2][0]);
  vec3 w_xy = vec3(v_mat[0][1], v_mat[1][1], v_mat[2][1]);

  vec3 v = translation + w_xa * vertex.x + w_xy * vertex.y;
  vo.texcoord = vertex.zw;

  gl_Position = vp_mat * vec4(v, 1);
}
