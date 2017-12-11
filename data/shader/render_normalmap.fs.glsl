varying vec3 normal;

void main(void)
{
  //render normal as color
  gl_FragColor = vec4((normalize(normal) + 1) * 0.5, 1);
}
