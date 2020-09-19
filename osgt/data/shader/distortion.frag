#version 120

#pragma import_defines(NUM_MOUSE_POINTS)
#ifndef NUM_MOUSE_POINTS
#    define NUM_MOUSE_POINTS 1
#endif  // ifndef NUM_MOUSE_POINTS

// a fragment might be distorted by multiple mouse points
// TODO find a better blend method

// known issue: if you drag too fast, you will see artifial effect around mouse points
#define BLEND_ADD 1
// known issue: break image continuity
#define BLEND_MAX_ABS 2
#define BLEND_FUNC BLEND_ADD

// uncomment this to debug
// #define DEBUG_DISTORTION

uniform sampler2D diffuse_map;
uniform vec2 resolution;
// normalized by divide smaller extent
uniform vec2 mouse_points[NUM_MOUSE_POINTS];

// tweakable
const float max_radius = 0.08;
const float distortion_scale = 0.5;

// When you drag mouse from A to B, three areas will be affected:
//  1.  half circle around A
//  2.  half circle around B
//  3.  rect between A and B
// In short, it's a capsule.
vec2 distort(vec2 st, vec2 start, vec2 end)
{

    vec2 start2end = end - start;
    float start2end_length = length(start2end);
    vec2 dir = start2end / start2end_length;
    vec2 start2st = st - start;

    // test projection
    float projection = dot(start2st, dir);
    if (projection < -max_radius || projection > start2end_length + max_radius)
    {
        return vec2(0);
    }

    // test point line distance
    // Some times, arg of sqrt is negative, might be a precision problem, max is used to
    // deail with it
    float by_projection =
        sqrt(max(dot(start2st, start2st) - projection * projection, 0.0f));
    if (by_projection > max_radius)
    {
        return vec2(0);
    }

    float strength = 0;
    if (projection < 0)
    {
        // half circle around A
        float d = distance(st, start);
        if (d >= max_radius)
        {
            return vec2(0);
        }
        strength = pow(1 - d / max_radius, 1);
    }
    else if (projection > start2end_length)
    {
        // half circle around B
        float d = distance(st, end);
        if (d >= max_radius)
        {
            return vec2(0);
        }
        strength = pow(1 - d / max_radius, 1);
    }
    else
    {
        // center rect
        strength = 1 - by_projection / max_radius;
    }

    // We are distorting st, must apply -
    return -strength * distortion_scale * start2end;
}

void main(void)
{
    // normalized st with smaller extent
    vec2 st = gl_TexCoord[0].xy;
    st *= resolution.x > resolution.y ? vec2(resolution.x / resolution.y, 1)
                                      : vec2(1, resolution.y / resolution.x);
    vec2 distortion = vec2(0);
    for (int i = 0; i < NUM_MOUSE_POINTS - 1; i++)
    {
        vec2 start = mouse_points[i];
        vec2 end = mouse_points[i + 1];
        if (start == end)
        {
            continue;
        }

        // apply force, scale it by index, the last one is the newest
        float scale = smoothstep(0.0f, 1.0f, (i + 1.0f) / NUM_MOUSE_POINTS);
        vec2 new_distortion = distort(st, start, end) * scale;

#if BLEND_FUNC == BLEND_ADD
        distortion += new_distortion;
#endif

#if BLEND_FUNC == BLEND_MAX_ABS
        distortion.x =
            abs(distortion.x) > abs(new_distortion.x) ? distortion.x : new_distortion.x;
        distortion.y =
            abs(distortion.y) > abs(new_distortion.y) ? distortion.y : new_distortion.y;
#endif
    }

#ifdef DEBUG_DISTORTION
    gl_FragColor = vec4(0, 0, 0, 1);
    // if(distortion != 0)
    // {
        vec2 distortion_color = -distortion * 11 * 0.5 + 0.5;
        gl_FragColor = vec4(distortion_color, 0, 1);
    // }

    // draw mouse points as small red dot
    for (int i = 0; i < NUM_MOUSE_POINTS - 1; i++)
    {
        if (distance(st, mouse_points[i]) < 0.005)
        {
            gl_FragColor = vec4(1, 0, 0, 1);
        }
    }
    return;
#endif  // ifdef DEBUG_DISTORTION

    vec4 color = texture2D(diffuse_map, gl_TexCoord[0].xy + distortion);
    gl_FragColor = color;
}
