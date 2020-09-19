#version 120

#pragma include ntoy.frag
#pragma include cnoise.frag

void main( void )
{
    vec2 st = gl_FragCoord.xy / resolution;
    st *= 16;

    float f = pnoise(st, vec2(6.99));
    float c = f * 0.5 + 0.5;

    gl_FragColor = vec4(c);
}
