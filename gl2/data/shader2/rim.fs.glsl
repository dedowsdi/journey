varying vec3 v_vertex; // view space
varying vec3 v_normal; // view space

uniform vec3 rim_color;
uniform float rim_power;

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

vec3 rim(vec3 v, vec3 n){
  float f = 1.0 - dot(v, n);
  return pow(smoothstep(0.0, 1.0, f), rim_power) * rim_color;
}

void main(void)
{
  gl_FragColor = blinn(v_vertex, v_normal, vec3(0), mtl, lights, lm);

  vec3 n = normalize(v_normal);
  vec3 v = normalize(-v_vertex);
  gl_FragColor.xyz = gl_FragColor.xyz + rim(n, v);
}
