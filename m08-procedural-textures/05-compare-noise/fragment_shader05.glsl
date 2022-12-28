#version 430 core

out vec4 fColor;

//in vec4 vColor;
in vec2 vTextureCoord;

uniform layout(location = 2) sampler2D  sampler;

uniform layout(location = 3) int texture_unit;

void main()
{

    //fColor = vec4(vTextureCoord.s, vTextureCoord.s, vTextureCoord.t,1.0);
    //fColor = vColor;
    vec4 val  = texture(sampler, vTextureCoord);
    float g = 0.5*val.x + 0.25*val.y + 0.125*val.z + 0.0625*val.w + 0.0625*0.25;


    vec4 samples[5];
    samples[0] = vec4(val.x, val.x, val.x, 1.0);
    samples[1] = vec4(val.y, val.y, val.y, 1.0);
    samples[2] = vec4(val.z, val.z, val.z, 1.0);
    samples[3] = vec4(val.w, val.w, val.w, 1.0);
    samples[4] = vec4(g,g,g, 1.0);

    fColor = samples[texture_unit];
    //fColor = vec4(1.0, 1.0, 0.0, 1.0);
}
