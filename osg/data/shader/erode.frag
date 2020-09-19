#version 120

varying float lightIntensity;
varying vec3 pos;

uniform sampler3D noise_texture;
uniform vec4 mortarColor = vec4(1, 1, 1, 1);
uniform vec4 brickColor = vec4(1, 0.5, 0, 1);

void main(void)
{
    vec4 noise_color = texture3D(noise_texture, pos * 2);
    if (noise_color.x < 0.22 && noise_color.y < 0.22 && noise_color.z < 0.22)
    {
        discard;
    }
    gl_FragColor = vec4(noise_color);
}
