//#version 330 core

in vec4 vertex; // x y s t
in vec2 texcoord;
in vec3 translation; // instance translation

#ifdef SCALE
in vec2 size;
#endif

#ifdef ROTATE
in float angle;
#endif

// texpack offset
#ifdef TEX_OFFSET
in vec2 texoffset;
#endif

out vs_out{
  vec2 texcoord;
}vo;

uniform mat4 vp_mat;
uniform vec3 camera_pos;

#ifdef USE_CAMERA_UP
uniform vec3 camera_up;
#endif

void main() {
  vec3 ev = camera_pos - translation;
  ev = normalize(ev);

#ifdef USE_CAMERA_UP
  vec3 up = camera_up;
  vec3 side = normalize(cross(up, ev));
  up = cross(ev, side);

  mat4 m_mat = mat4(mat3(side, up, ev));
  m_mat[3].xyz = translation;
#else
  // yaw z to xz proj of ev
  float c0 = ev.z;
  float s0 = ev.x;
  float c1 = sqrt(c0 * c0 + s0 * s0);
  float s1 = -ev.y;
  if(c1 == 0){
    c0 = 1;
  }else{
    c0 /= c1;
    s0 /= c1;
  }

  float l = sqrt(c1 * c1 + s1 * s1);
  if(l == 0) {
    c1 = 1;
  }else{
    c1 /= l;
    s1 /= l;
  }

  mat4 m_mat = mat4(
      c0,            0,             -s0,           0,
      s0 * s1,       c1,            c0*s1,         0,
      s0*c1,         -s1,           c0*c1,         0,
      translation.x, translation.y, translation.z, 1
      );
#endif

#ifdef TEX_OFFSET
  vo.texcoord = texcoord + texoffset;
#else
  vo.texcoord = texcoord;
#endif

  vec4 v = vertex;

#ifdef SCALE
  v.xy *= size.xy;
#endif

#ifdef ROTATE
  float c = cos(angle);
  float s = sin(angle);
  float x = dot(vec2(c, s), v.xy);
  float y = dot(vec2(-s, c), v.xy);
  v.xy = vec2(x, y);
#endif

  gl_Position = vp_mat * m_mat * v;

}
