#version 430 core

layout(quads, equal_spacing, ccw) in;

uniform mat4 mvp_mat;

// order 4 bernstein polynomial
float B(int i, float u)
{
  vec4 bc_table = vec4(1, 3, 3, 1);
  return bc_table[i] * pow(u, i) * pow(1-u, 3-i);
}

void main()
{
  gl_Position = vec4(0);
  // u order and v order is the same here, so it doesn't matter which is u or v.
  // it must be opengl convention to make u along x even for patch, this also
  // means u dir is the dir of the last second array
  // eg:
  //  data[a][b][c],  c is dimention, b is u order, c is ustride, a is v order,
  //  b*c is vstride
  float u = gl_TessCoord.x;
  float v = gl_TessCoord.y;
  for (int j = 0; j < 4; ++j) {
    for (int i = 0; i < 4; i++) {
      gl_Position += gl_in[4*j + i].gl_Position * B(i, u) * B(j, v);
    }
  }

  gl_Position = mvp_mat * gl_Position;
}
