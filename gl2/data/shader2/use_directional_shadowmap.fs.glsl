#extension GL_EXT_gpu_shader4 : require

varying vec3 v_normal;
varying vec3 v_vertex;
varying vec4 l_vertex;  // bias, scaled wnd v_vertex of light view

uniform sampler2D depth_map;
// sample the shadowmap once, the hardware will in fact also sample the
// neighboring
// texels, do the comparison for all of them, and return a float in [0,1] with a
// bilinear filtering of the comparison results.
uniform sampler2DShadow shadow_map;
uniform float bias = 0.005;
uniform bool use_sampler2_d_shadow = true;

uniform material mtl;
uniform light_model lm;
uniform light_source lights[LIGHT_COUNT];

/*
 * as depth map is discrete, different fragments with different depth might
 * sample the same texel, this could cause shadow acne, bias is used to
 * eliminite shaodw acne.
 */
float shadow(float bias) {
  float visibility = 1.0;

  if (use_sampler2_d_shadow) {
    visibility *= shadow2DProj(shadow_map,
                    vec4(l_vertex.xy, l_vertex.z - bias, l_vertex.w)).z;
  } else {
    visibility = 0.0;
    vec2 texel_size = 1.0 / textureSize2D(depth_map, 0);
    for (int i = -1; i < 2; i++) {
      for (int j = -1; j < 2; ++j) {
        float pcf_depth =
          texture2D(depth_map, l_vertex.xy + texel_size * vec2(i, j)).z;
        visibility += (l_vertex.z - bias > pcf_depth) ? 0 : 1;
      }
    }
    visibility /= 9.0;
  }

  return visibility;
}

void main(void) {
  vec3 l = normalize(lights[0].position.xyz - v_vertex);
  vec3 n = normalize(v_normal);
  float ndotl = clamp(dot(n, l), 0.0, 1.0);

  float shadow_bias = clamp(bias * tan(acos(ndotl)), 0, 0.1);
  float visibility = shadow(shadow_bias);

  gl_FragColor = blinn(v_vertex, v_normal, vec3(0), mtl, lights, lm);
  gl_FragColor *= visibility;
}
