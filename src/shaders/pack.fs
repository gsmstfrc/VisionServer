//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

precision highp float;

uniform sampler2D inTex;
uniform float increment;
varying vec2 intTexCoord;

vec4 coffhi = vec4(0.50196, 0.25098, 0.12549, 0.06275);
vec4 cofflo = vec4(0.03137, 0.01569, 0.00784, 0.00392);

void main()
{
    float acc = intTexCoord.x;
    float y = intTexCoord.y; 
    vec4 sample;
    vec4 sample2;
    for (int i = 0; i < 4; i++) {
        sample.x = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        sample.y = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        sample.z = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        sample.w = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        sample2.x = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        sample2.y = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        sample2.z = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        sample2.w = texture2D(inTex, vec2(acc, y)).x;
        acc += increment;
        gl_FragColor[i] = dot(sample, coffhi) + dot(sample2, cofflo); 
    }
}



