#version 430 core

out vec4 fColor;

in vec2 vTextureCoord;

uniform layout(location = 2) sampler2D  sampler;

void main()
{
    fColor = texture(sampler, vTextureCoord);
//    fColor = vec4(255.0/255.0, 128.0/255.0, 0, 1);
}
