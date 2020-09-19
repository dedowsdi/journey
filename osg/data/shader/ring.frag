#version 120

#pragma include ntoy.frag

float basic_sine_ring(vec2 st)
{
    float l = length(st);
    return sin((l+0.74) * 30);
}

float ring_with_hole(vec2 st)
{
    float l = length(st);
    float f = 0.5 - pow(l - 0.35, 0.1) + sin(l * 30.);
    f *= 1.2;
    return f;
}

void main()
{
    vec2 st = gl_TexCoord[0].xy * 2 - 1;

    float f = ring_with_hole(st);
    // float f = basic_sine_ring(st);

    vec3 color = f * vec3(1);

    gl_FragColor = vec4(color, 1);
}
