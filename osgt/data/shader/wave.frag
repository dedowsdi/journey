#version 120

// TODO improve lighting

#pragma include shader/snoise.frag

uniform vec2 resolution;
uniform float osg_SimulationTime;

#define NUM_WAVES 9
#define PI 3.1415926535897932384626433832795
// It's used to approximate wave surface normal
#define EPSILON 0.0005
#define time osg_SimulationTime

#define PURE_SINE 1
#define COSINE_POWER 2
#define WAVE_STYPE PURE_SINE

// tweakable
const float omega = 60;
const float phi = 10;
const float dampen_scale = 15.7f;
const float sharper_power = 2.4f;
const float max_radius = 0.6f;
const float shape_noise_st_scale = 2.3;
const float shape_noise_scale = 0.1f;
const float cycle_animation_scale = 0.18f;
const float amplitude = 0.08f;
const vec3 light_dir = normalize(vec3(-1, 1, 0.152));

// Squeze center along moving direction, squeeze wave along direction perpendicular to
// moving direction
#define SQUEEZE
// draw current wave center in red, unsqueezed wave center in white, intersections
// between unsqueezed circle and a ray madeof current circle center and current st (it's a
// circle)
// #define DEBUG_DRAW_SQUEEZE

#ifdef SQUEEZE
const float squeeze_center_scale = 0.76f;
#endif // ifdef SQUEEZE

#if WAVE_STYPE == COSINE_POWER
const float cosine_power = 33.0f;
const float cosine_scale = 5.95f;
#endif

struct Wave
{
    float t;  // nomalized time in cycled path
    float speed;
    vec3 a0;  // see get_position for what a0 and a1 mean
    vec3 a1;
};

// clang-format off
const Wave waves[9] = Wave[9] (
    Wave(0.1, 0.4, vec3(1,1,1), vec3(1,1,1)),
    Wave(0.2, 0.5, vec3(1,1,3), vec3(2,1,1)),
    Wave(0.3, 0.6, vec3(2,1,1), vec3(1,0,1)),
    Wave(0.4, 0.7, vec3(1,2.5,2.5), vec3(-1.2,1.1,1)),
    Wave(0.5, 0.8, vec3(1,1,1), vec3(1,1,3)),
    Wave(0.6, 0.9, vec3(1,2,1), vec3(1,1,2)),
    Wave(0.7, 1.0, vec3(1,3,1), vec3(1,1,5)),
    Wave(0.8, 1.1, vec3(1,4,1), vec3(1,1,2)),
    Wave(0.9, 1.2, vec3(1,5,1), vec3(1,1,1))
    );
// clang-format on

vec2 get_position(Wave wave, float t)
{
    t += wave.t;
    float x = sin(wave.a0.x * t + cos(wave.a0.y * t)) * cos(wave.a0.z * t);
    float y = cos(wave.a1.x * t + sin(wave.a1.y * t)) * sin(wave.a1.z * t);
    return vec2(x, y);
}

vec2 get_velocity(Wave wave, float t)
{
    // It shakes if epsilon is too small
    float epsilon = 0.016;
    return (get_position(wave, t + epsilon) - get_position(wave, t - epsilon)) /
           (epsilon * 2);
}

bool solve_quadratic(float a, float b, float c, out float x0, out float x1)
{
    float discriminant = b * b - 4 * a * c;
    if (discriminant < 0)
        return false;

    if (discriminant == 0)
        x0 = x1 = -0.5 * b / a;

    // avoid catastrophic cancellation
    float q = (b > 0) ? -0.5 * (b + sqrt(discriminant)) : -0.5 * (b - sqrt(discriminant));
    x0 = q / a;
    x1 = c / q;  // x0*x1 = c / a

    if (x0 > x1)
    {
        float temp = x0;
        x0 = x1;
        x1 = temp;
    }

    return true;
}

// dir need not to be normalized
// It solves || orig + dir * t - center || = radius * radius
bool circle_ray_intersect(vec2 center, float radius, vec2 orig, vec2 dir, out float t)
{
    float t0, t1;  // solutions for t if the ray intersects
    vec2 L = orig - center;
    float a = dot(dir, dir);
    float b = dot(dir, L) * 2;
    float c = dot(L, L) - radius*radius;

    if (!solve_quadratic(a, b, c, t0, t1))
        return false;

    // always return the bigger one
    t = t1;
    return t > 0;
}

#ifdef DEBUG_DRAW_SQUEEZE
vec3 draw_squeeze(vec2 xy)
{
    vec3 color = vec3(0);
    float cycle_time = time * cycle_animation_scale;
    for (int i = 0; i < NUM_WAVES; i++)
    {
        Wave wave = waves[i];
        vec2 pos = get_position(wave, cycle_time * wave.speed);

        color += vec3(step(distance(pos, xy), 0.015), 0, 0);

        vec2 vel = get_velocity(wave, cycle_time * wave.speed);
        vec2 wave2st = xy - pos;
        float wave2st_len = length(wave2st);
        float speed = length(vel);
        if (speed > 0)
        {
            // draw original circle center
            vec2 nvel = vel / speed;
            vec2 pos_b = pos - nvel * max_radius * (1 - exp(-speed * squeeze_center_scale));
            color += vec3(step(distance(pos_b, xy), 0.015));

            if (distance(pos_b, xy) > max_radius)
            {
                // color = vec3(0, 0, 0.5);
                continue;
            }

            // draw intersection point, it's same as original circle
            float t;
            if (circle_ray_intersect(pos_b, max_radius, pos, wave2st, t))
            {
                vec2 intersection = pos + wave2st * t;
                color += vec3(step(distance(intersection, xy), 0.005));
            }

        }
    }

    return color;
}
#endif  // ifdef DEBUG_DRAW_SQUEEZE

float compute_height(vec2 xy)
{
    float h = 0;
    float cycle_time = time * cycle_animation_scale;
    for (int i = 0; i < NUM_WAVES; i++)
    {
        Wave wave = waves[i];
        vec2 pos = get_position(wave, cycle_time * wave.speed);

        vec2 wave2st = xy - pos;

        float wave2st_len = length(wave2st);
        float dampened_amplitude = amplitude * exp(-wave2st_len * dampen_scale);

#ifdef SQUEEZE
        // squeeze works like create wave from another pos B, then squeeze center from B to
        // current one
        vec2 vel = get_velocity(wave, cycle_time * wave.speed);
        float speed = length(vel);
        if (speed > 0)
        {
            vec2 nvel = vel / speed;
            vec2 pos_b = pos - nvel * max_radius * (1 - exp(-speed * squeeze_center_scale));

            if (distance(pos_b, xy) > max_radius)
            {
                continue;
            }

            // squeeze it along perpendicular dir of nvel
            vec2 pnvel = vec2(-nvel.y, nvel.x);
            vec2 projection = dot(wave2st, nvel) * nvel;
            vec2 by_projection = wave2st - projection;
            wave2st = projection + by_projection * max(speed*0.75, 1) ;
            wave2st_len = length(wave2st);

            // when we squeeze, the points on the perimeter didn't change, our goal is to
            // get that point, and use it to get original point of current point on B

            // do ray circle test, this should always success in theory
            float t;
            vec2 nwave2st = wave2st / wave2st_len;

            // TODO This is a bit expansive, find a way to scale wave2st directly
            if (circle_ray_intersect(pos_b, max_radius, pos, nwave2st, t))
            {
                vec2 intersection = pos + nwave2st * t;
                float percent = wave2st_len / t;
                vec2 original_point = mix(pos_b, intersection, percent);
                wave2st_len = distance(pos_b, original_point);

                dampened_amplitude = amplitude * exp(-wave2st_len * dampen_scale);

            }
        }

#else
        if (wave2st_len > max_radius)
        {
            continue;
        }
#endif

        // noise shape
        wave2st_len *=
            1 + shape_noise_scale * snoise(wave2st * shape_noise_st_scale + time * 0.3f);

        // shaper wave
        float inner = wave2st_len * omega - phi * time + wave.t * 6.5;
        float sharped_wave = pow((sin(inner) + 1) * 0.5, sharper_power);

        h += dampened_amplitude * sharped_wave;

#if WAVE_STYPE != PURE_SINE
        float proj_len = wave2st_len;
#endif

#if WAVE_STYPE == COSINE_POWER
        // don't miss abs, base of pow has to be positive, otherwise you get nan.
        h += pow(abs(cos(min(proj_len, PI * 0.5f))), cosine_power) * 1;
#endif
    }
    return h;
}

vec3 approximate_normal(vec2 xy)
{
    // compute normal for the most simple case
    // float common_factor = amplitude * cos(inner) * omega * 0.5 / wave2vertex_length * 2;
    // float phx = common_factor * wave2vertex.x; // partial derivative of h to x
    // float phy = common_factor * wave2vertex.y;
    // normal = gl_NormalMatrix * vec3(-phx, -phy, 1);
    // vertex = vec3(gl_Vertex.xy, wave2vertex_length/37);

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

vec3 light(vec3 normal)
{
    vec3 n = normalize(normal);
    float ndotl = abs(dot(n, light_dir));

    vec3 color = vec3(0.2) * ndotl;

    if (ndotl > 0)
    {
        vec3 h = normalize(light_dir + normalize(vec3(0, 0, 1)));
        float ndoth = abs(dot(n, h));

        color += vec3(1) * pow(ndoth, 7.74);
    }

    return color;
}

void main(void)
{
    // normalize st by the short extent
    vec2 st = 2 * gl_FragCoord.xy / resolution - 1;
    st *= resolution.x > resolution.y ? vec2(resolution.x / resolution.y, 1)
                                      : vec2(1, resolution.y / resolution.x);
    vec3 normal = approximate_normal(st);
    vec3 color = light(normal);

#ifdef DEBUG_DRAW_SQUEEZE
    color += draw_squeeze(st);
#endif

    // gl_FragColor = vec4(normalize(normal) * 0.5 + 0.5, 1);
    gl_FragColor = vec4(color, 1);
}
