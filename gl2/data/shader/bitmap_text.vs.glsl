attribute vec4 vertex;
uniform mat4 mvp_mat;

varying vec2 texcoord;

void main(void)
{
  gl_Position = mvp_mat * vec4(vertex.xy, 0, 1);
  texcoord = vertex.zw;
}
