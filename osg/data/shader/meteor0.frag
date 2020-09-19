#version 120

#pragma include shader/ntoy.frag

#define TURBULENCE_OCTAVES 2
#define TURBULENCE_LACUNARITY 2
#define TURBULENCE_GAIN 0.5
#pragma include shader/fbm.frag

void main()
{
    vec2 st = gl_FragCoord.xy / resolution * 2 - 1;
    st.x += 1;    // shift meteor from center to left

    // body and tail
    float f = 0.29 * pow( max( st.x, 0 ), 1.03 ) /
              abs( pow( abs( st.y * 3.06 ), 1.33 ) );

    f /= turbulence( st * 32 );

    // fade out
    f *= smoothstep( pow( st.x, 0.41 ), 0, abs(st.y) );

    vec3 color = vec3( 0.73, 0.26, 0.15 ) * f;

    gl_FragColor = vec4( color, 1 );
}
