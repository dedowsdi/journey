uniform vec3 rimColor;
uniform float rimPower;

vec3 rim(vec3 v, vec3 n){
  float f = 1.0 - dot(v, n);
  return pow(smoothstep(0.0, 1.0, f), rimPower) * rimColor;
}

void main(void)
{
  vec3 v = normalize(-vertex);
  vec3 n = normalize(normal);
  gl_FragColor = blinn(v, n);

  gl_FragColor.xyz = gl_FragColor.xyz + rim(n, v);
}
