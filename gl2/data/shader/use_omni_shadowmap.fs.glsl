#extension GL_EXT_gpu_shader4 : require

uniform vec3 w_light_pos;
uniform samplerCube depth_cube_map;
uniform float bias = 0.05;
uniform float near_plane; //light near
uniform float far_plane;  //light far

varying vec3 v_vertex;
varying vec3 w_vertex;
varying vec3 v_normal;  // local

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

float shadow(float bias){
  vec3 l2v = w_vertex - w_light_pos;
  float current_depth = max(abs(l2v.x), max(abs(l2v.y), abs(l2v.z)));

  float visibility = 0.0;
  float offset = 0.1;
  float num_samples = 4.0;
  //l2v = normalize(l2v);
  for (float x = -offset; x < offset; x+= 2 * offset/num_samples) {
    for (float y = -offset; y < offset; y+= 2 * offset/num_samples) {
      for (float z = -offset; z < offset; z+= 2 * offset/num_samples) {
        float pcf_depth = textureCube(depth_cube_map, l2v + vec3(x, y, z)).x;
        pcf_depth = pcf_depth * (far_plane - near_plane) + near_plane;
        visibility += (current_depth - bias > pcf_depth) ? 0 : 1;
      }
    }
  }
  visibility /= (num_samples * num_samples * num_samples);
  return visibility;
}


void main(void) {
  // simple blinn-phone for point light
  vec3 l = normalize(lights[0].position.xyz - v_vertex);
  vec3 n = normalize(v_normal);
  float ndotl = clamp(dot(n, l), 0.0, 1.0);

  float shadow_bias = clamp(bias * tan(acos(ndotl)), 0, 5);
  float visibility = shadow(shadow_bias);

  gl_FragColor = blinn(v_vertex, v_normal, vec3(0), mtl, lights, lm);
  gl_FragColor *= visibility;

}
