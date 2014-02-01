//
// FRC Vision Server
// FRC 2014
// FRC Team 3318
// Written by Ian Ewell <ianewellgmsmt@gmail.com>
// (C) 2014 GSMST Robotics
//

precision highp float;

attribute vec4 position;
attribute vec2 texCoord;

varying vec2 intTexCoord;

void main()
{
    intTexCoord = texCoord;
    gl_Position = position;
}
