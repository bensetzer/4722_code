#version 430 core

out vec4 fColor;

in vec2 vTextureCoord;

uniform sampler3D  sampler;
uniform sampler2D sampler2;

void main()
{
    //fColor = vec4(vTextureCoord, 0.0, 1.0);
  vec4 s = texture(sampler, vec3(vTextureCoord, 0.5));
//  vec4 s = texture(sampler2, vTextureCoord);
  //float ls = length(s.xyz);
  float ls  = s.x + s.y + s.z + s.a;
  ls = s.x;
  fColor = vec4(ls,ls,ls,1.0);
//  fColor = vec4(1,1,0,1);
}
