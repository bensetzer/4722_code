#version 430 core

in vec4 bPosition;
in vec2 bTextureCoord;

out vec2 vTextureCoord;

void
main()
{
    vTextureCoord = bTextureCoord;
    gl_Position = bPosition;
}
