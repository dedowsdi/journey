/*
 * look at camera from billboard, use camera up as up
 */
#version 330 core

in vec4 vertex; // x y s t
in vec3 translation; // instance translation

out VS_OUT{
  vec2 texcoord;
}vs_out;

uniform mat4 viewProjMatrix;
uniform vec3 cameraUp;
uniform vec3 cameraPos;

void main() {
  vec3 ev = cameraPos - translation;
  ev = normalize(ev);

  vec3 up = cameraUp;
  vec3 right = normalize(cross(up, ev));
  up = cross(ev, right);

  mat4 modelMatrix = mat4(mat3(right, up, ev));
  modelMatrix[3].xyz = translation;

  vs_out.texcoord = vertex.zw;
  gl_Position = viewProjMatrix * modelMatrix * vec4(vertex.xy, 0, 1);
}
