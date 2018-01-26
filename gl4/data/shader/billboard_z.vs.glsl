/*
 * rotate z to ev, don't care about orientation
 */
#version 330 core

in vec4 vertex; // x y s t
in vec3 translation; // instance translation

out VS_OUT{
  vec2 texcoord;
}vs_out;

uniform mat4 viewProjMatrix;
uniform vec3 cameraPos; // world position

void main() {
  vs_out.texcoord = vertex.zw;
  vec3 ev = cameraPos - translation;

  // yaw, pitch in intrinsic
  
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

  mat4 modelMatrix = mat4(
      c0,            0,             -s0,           0,
      s0 * s1,       c1,            c0*s1,         0,
      s0*c1,         -s1,           c0*c1,         0,
      translation.x, translation.y, translation.z, 1
      );

  gl_Position = viewProjMatrix * modelMatrix * vec4(vertex.xy, 0, 1);
}
