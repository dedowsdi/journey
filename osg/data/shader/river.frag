#version 120

#pragma include shader/ntoy.frag

#define TURBULENCE_OCTAVES 4
#define TURBULENCE_LACUNARITY 1.89
#define TURBULENCE_GAIN 0.5

#define FBM_OCTAVES 4
#define FBM_LACUNARITY 2
#define FBM_GAIN 0.5
// #ragma include shader/fbm.frag
#pragma include shader/snoise.frag

float fbm( vec2 st )
{
    float f = 0;
    float amplitude = 0.5f;
    float frequence = 1;
    for ( int i = 0; i < FBM_OCTAVES; i++ )
    {
        f += amplitude * snoise( st );
        st = st * frequence;
        frequence *= FBM_LACUNARITY;
        amplitude *= FBM_GAIN;
    }

    return f;
}

void main()
{
    vec2 ost = gl_FragCoord.xy / resolution * 2 - 1;
    vec2 st = ost;

    st.y += fbm(st ) * 0.12;

    // float f = 0.55 * exp( -abs( st.y * 2 ) );
    float f = 0.8 * smoothstep( 0.3, 0, abs( st.y ) );

    vec3 color = vec3(f);
    gl_FragColor = vec4( color, 1 );
}
