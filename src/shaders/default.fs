//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

precision highp float;

uniform Sampler2D inTex;
varying vec2 intTexCoord;

void main()
{
    gl_FragColor = texture2D(inTex, intTexCoord);
}
