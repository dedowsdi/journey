
attribute vec4 vertex;

uniform mat4 mvp_mat;
uniform float near_plane;
uniform float far_plane;

varying float z_distance; // in eye space

void main(void)
{
  gl_Position = mvp_mat * vertex;
  
  // get z distance from ndc, must faster than a mat4*vec4
  float zn = gl_Position.z/gl_Position.w;
  z_distance = -2.0 * near_plane * far_plane / (zn * (far_plane - near_plane) -
      far_plane - near_plane );
}
