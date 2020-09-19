#version 120

#pragma include shader/snoise.frag

// ball is really a stupid name, TODO find a better name
#pragma import_defines(NUM_BALLS)
#ifndef NUM_BALLS
#    define NUM_BALLS 1
#endif  // ifndef NUM_BALLS

// It shuld be small relative to world size (200 * 200), it's used to approximate normal.
#define EPSILON 0.16

uniform vec3 balls[NUM_BALLS];
uniform float osg_SimulationTime;
#define time osg_SimulationTime

// tweakable
const float dampen_scale = 0.19f;
const float wave_power = 2.36;
const float max_radius = 50;
const float shape_noise_st_scale = 0.06;
const float shape_noise_scale = 0.07;
const float wave_animation_scale = 1.0f;

varying vec3 normal;
varying vec3 vertex;

float compute_height(vec2 xy)
{
    float amplitude = 4;
    float omega = 0.6;
    float phi = 10;

    vec2 pos = 15 * vec2(cos(time), sin(time));

    vec2 ball2st = xy - pos;

    vec2 vel = 15 * vec2(-sin(time), cos(time));

    // TODO squeeze it
    // float cos_with_intensity = dot(vel, ball2st);
    // ball2st *= 1 + pow(1.1, cos_with_intensity * 0.325);

    float ball2st_len = length(ball2st);
    if (ball2st_len > max_radius)
    {
        return 0;
    }

    // todo squeeze it
    float dampened_amplitude = amplitude * exp(-ball2st_len * dampen_scale);

    // change shape
    ball2st_len *= 1 + shape_noise_scale * snoise(ball2st * shape_noise_st_scale +
                                                  osg_SimulationTime * 0.3f);
    float inner = ball2st_len * omega - phi * time * wave_animation_scale;
    float sharped_wave = pow((sin(inner) + 1) * 0.5, wave_power);
    return dampened_amplitude * sharped_wave;
}

vec3 approximate_normal(vec2 xy)
{
    // compute normal for the most simple case
    // float common_factor = amplitude * cos(inner) * omega * 0.5 / ball2vertex_length * 2;
    // float phx = common_factor * ball2vertex.x; // partial derivative of h to x
    // float phy = common_factor * ball2vertex.y;
    // normal = gl_NormalMatrix * vec3(-phx, -phy, 1);
    // vertex = vec3(gl_Vertex.xy, ball2vertex_length/37);

    // It's hard to compute precise normal when there are so many factors, I will
    // approximate it instead.
    float l = length(xy);
    float epsilon = EPSILON;
    float delta_change = epsilon * 2;

    float phx = (compute_height(vec2(xy.x + epsilon, xy.y)) -
                    compute_height(vec2(xy.x - epsilon, xy.y))) /
                delta_change;
    float phy = (compute_height(vec2(xy.x, xy.y + epsilon)) -
                    compute_height(vec2(xy.x, xy.y - epsilon))) /
                delta_change;
    return normalize(vec3(-phx, -phy, 1));
}

void main()
{
    normal = approximate_normal(gl_Vertex.xy);
    float h = compute_height(gl_Vertex.xy * 0);
    vertex = vec3(gl_Vertex.xy, h);
    gl_Position = gl_ModelViewProjectionMatrix * vec4(gl_Vertex.xy, h, 1);
}
