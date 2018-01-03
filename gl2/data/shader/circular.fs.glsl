#ifdef GL_ES
precision mediump float;
#endif

uniform bool use_origin = false;
uniform float time;
uniform float time_scale = 5;
uniform float length_scale = 60;
uniform vec2 origin;
uniform vec2 resolution;

void main(void) {
  vec2 p;
  if (use_origin) {
    // vector from origin to pixel position
    p = (gl_FragCoord.xy - origin) / min(resolution.x, resolution.y);
  } else {
    p = (gl_FragCoord.xy - resolution * 0.5) / min(resolution.x, resolution.y);
  }

  float t = sin(length(p) * length_scale + time * time_scale);
  gl_FragColor = vec4(t, t, t, 1.0);
}
