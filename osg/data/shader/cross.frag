#version 120

#pragma include shader/ntoy.frag
#pragma include shader/snoise.frag


void main()
{
    vec2 st = gl_FragCoord.xy / resolution * 2 - 1;
    st *= 8;

    st = abs(st);

    // to highlight 1/x, we must get distance from current point to normal or 1/x.
    // derivative of 1/x is -1/(x^2), so tangent of it's normal is x^2
    float tan_theta2 = st.x * st.x * st.x * st.x;
    float sin_theta = sqrt( tan_theta2 / ( 1 + tan_theta2 ) );

    float f = 1.0 / (  abs( st.y - 1 / st.x ) * sin_theta );

    vec3 color = vec3(f);

    gl_FragColor = vec4(color, 1);
}
