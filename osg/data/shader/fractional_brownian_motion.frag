#version 120

// https://thebookofshaders.com/13/
// see also turbulence and ridge

#pragma include shader/ntoy.frag
#pragma include shader/snoise.frag

#define OCTAVES 4
#define LACUNARITY 2
#define GAIN 0.5

float fbm( vec2 st )
{
    float f = 0;
    float amplitude = 0.5f;
    float frequence = 1;
    for ( int i = 0; i < OCTAVES; i++ )
    {
        f += amplitude * snoise( st * frequence );
        frequence *= LACUNARITY;
        amplitude *= GAIN;
    }

    return f;
}

float turbulence( vec2 st )
{
    float f = 0;
    float amplitude = 0.5f;
    float frequence = 1;
    for ( int i = 0; i < OCTAVES; i++ )
    {
        f += amplitude * abs( snoise( st * frequence ) );
        frequence *= LACUNARITY;
        amplitude *= GAIN;
    }

    return f;
}

void main()
{
    vec2 st = gl_FragCoord.xy / resolution;

    float color = turbulence( st * 4 ) ;
    // float color = fbm( st * 4 );

    color = color * 0.5 + 0.5;

    gl_FragColor = vec4( color );
}
