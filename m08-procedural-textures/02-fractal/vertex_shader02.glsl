#version 430 core

precision highp float;

in vec4 bPosition;


uniform vec2 center, range;

out vec2 vPos;

void main()
{
    // gl_PointSize = 12.0;
    gl_Position = bPosition;
    
    //vPos = (bPosition.xy + 1.0) / 2.0 * range + ll;
    vPos = bPosition.xy * range + center;
}