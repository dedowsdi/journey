#version 330 core

in vec4 vertex; // x y s t
in vec4 xyzs;  // instance x y z size 
in vec4 color; // instance color

out VS_OUT{
  vec4 color;
  vec2 texcoord;
}vs_out;

uniform mat4 viewProjMatrix;
uniform vec3 cameraUp;
uniform vec3 cameraPos;

void main() {
  vec3 ev = cameraPos - xyzs.xyz;
  ev = normalize(ev);

  vec3 up = cameraUp;
  vec3 right = normalize(cross(up, ev));
  up = cross(ev, right);

  mat4 modelMatrix = mat4(mat3(right, up, ev));
  modelMatrix[3].xyz = xyzs.xyz;

  vs_out.texcoord = vertex.zw;
  vs_out.color = color;
  gl_Position = viewProjMatrix * modelMatrix * vec4(vertex.xy * xyzs.w, 0, 1);
}
