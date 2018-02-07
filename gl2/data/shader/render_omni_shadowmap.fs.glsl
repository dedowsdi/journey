uniform float near_plane;
uniform float far_plane;

varying float z_distance; // in eye space

void main(void)
{
  //write linear depth
  gl_FragDepth = (z_distance - near_plane) / (far_plane - near_plane);
}
