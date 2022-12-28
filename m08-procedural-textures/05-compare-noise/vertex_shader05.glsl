#version 430 core

in layout(location = 1) vec4 bPosition;
//in vec4 bColor;
in layout(location = 2) vec2 bTextureCoord;

uniform layout(location = 1) mat4 transform;

//out vec4 vColor;
out vec2 vTextureCoord;

void
main()
{
    //vColor = bColor;
    vTextureCoord = bTextureCoord;
    //gl_PointSize = 25.0;
    gl_Position = transform * bPosition;
}
