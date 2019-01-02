// only works for directional light

attribute vec4 vertex;

uniform mat4 mvp_mat;

void main(void) {
  gl_Position = mvp_mat * vertex;
}
