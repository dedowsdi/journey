/*
 * Align billboard rotation to camera rotation(not facing camera).
 */
#version 330 core

in vec4 vertex; // x y s t
in vec3 translation; // instance translation

out VS_OUT{
  vec2 texcoord;
}vs_out;

uniform mat4 viewMatrix;
uniform mat4 viewProjMatrix;

void main() {
  vec3 worldCameraAxisX = vec3(viewMatrix[0][0], viewMatrix[1][0], viewMatrix[2][0]);
  vec3 worldCameraAxisY = vec3(viewMatrix[0][1], viewMatrix[1][1], viewMatrix[2][1]);

  vec3 v = translation + worldCameraAxisX * vertex.x + worldCameraAxisY * vertex.y;
  vs_out.texcoord = vertex.zw;

  gl_Position = viewProjMatrix * vec4(v, 1);
}
