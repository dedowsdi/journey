#version 330 core

uniform vec4 color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

layout(location = 0) out vec4 fragColor;

void main() {
  fragColor = color;
}
