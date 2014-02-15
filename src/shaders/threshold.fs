//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

precision highp float;

uniform sampler2D inTex;
varying vec2 intTexCoord;

vec4 mint = vec4(0.0, 0.0, 0.45, 0.0);
vec4 maxt = vec4(0.4, 0.6, 1.0, 1.0);

void main()
{
    vec4 color = texture2D(inTex, intTexCoord);
    vec4 minp = step(mint, color);
    vec4 maxp = step(vec4(1.0)-maxt, vec4(1.0)-color);
    float mask = step(0.98,dot(minp, maxp)*0.25);
    gl_FragColor = vec4(vec3(mask), 1.0);//vec4(greaterThanEqual(color, comp));
}


