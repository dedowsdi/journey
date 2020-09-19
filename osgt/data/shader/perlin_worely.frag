#version 120

#pragma include shader/snoise.frag

uniform vec2 resolution;
uniform float osg_SimulationTime;

vec2 random2(vec2 p)
{
    return fract(
        sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3)))) * 43758.5453123);
}

float cnoise(vec2 st)
{
    vec2 ist = floor(st);
    vec2 fst = fract(st);

    float min_dist = 1.0;

    // check 3x3 cells for distance filed
    for (int x = -1; x < 2; x++)
    {
        for (int y = -1; y < 2; y++)
        {
            vec2 neighbor = vec2(x, y);

            // get random point by tile index
            vec2 point = random2(vec2(ist + neighbor));

            // animate the point
            point = 0.5 + 0.5 * sin(point * 6 + osg_SimulationTime);

            // get distance2 to this point
            vec2 toPoint = neighbor + point - fst;
            min_dist = min(min_dist, dot(toPoint, toPoint));
        }
    }

    return sqrt(min_dist);
}

float cnoise_fbm( vec2 x, in float H )
{
    float gain = exp2(-H);
    float frequence = 1.0;
    float amplitude = 1.0;
    float total = 0.0;
    for (int i = 0; i < 3; i++)
    {
        total += amplitude * cnoise(frequence * x);
        frequence *= 2.0;
        amplitude *= gain;
    }
    return total;
}


void main()
{
    vec2 st = gl_FragCoord.xy / resolution;
    // st *= 8;

    float f = cnoise_fbm(st * 16, 2.23);
    f = 1- f;

    // float f = 1 - cnoise(st);


    vec4 color = vec4(f);

    // draw cell center
    // color += step(min_dist, 0.02);

    // draw grid
    // color.x += step(0.98, fst.x) + step(0.98, fst.y);

    gl_FragColor = vec4(color);
}
