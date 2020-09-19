#version 120

// https://thebookofshaders.com/13/
// see also turbulence and fbm

#pragma include shader/ntoy.frag
#pragma include shader/snoise.frag

// Ridged multifractal
// See "Texturing & Modeling, A Procedural Approach", Chapter 12
float ridge(float h, float offset) {
    h = abs(h);     // create creases
    h = offset - h; // invert so creases are at top
    h = h * h;      // sharpen creases
    return h;
}

#define OCTAVES 2

float ridgedMF(vec2 p) {
    float lacunarity = 2.0;
    float gain = 0.5;
    float offset = 0.87;

    float sum = 0.0;
    float freq = 1.0, amp = 0.5;
    float prev = 1.0;
    for(int i=0; i < OCTAVES; i++) {
        float n = ridge(snoise(p*freq), offset);
        sum += n*amp;
        sum += n*amp*prev;  // scale by previous octave
        prev = n;
        freq *= lacunarity;
        amp *= gain;
    }
    return sum;
}

void main()
{
    vec2 st = gl_FragCoord.xy / resolution;

    float color = ridgedMF( st * 4 );

    // color = color * 0.5 + 0.5;

    gl_FragColor = vec4( color );
}
