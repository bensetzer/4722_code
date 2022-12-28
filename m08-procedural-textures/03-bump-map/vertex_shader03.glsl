#version 430 core

in layout(location = 1) vec4 bPosition;
in layout(location = 2) vec4 bNormal;
in layout(location = 3) vec4 bTangent;
in layout(location = 4) vec2 bTextureCoord;

uniform layout(location = 1) mat4 mv_transform;
uniform layout(location = 2) mat4 v_transform;
uniform layout(location = 3) mat4 p_transform;
uniform layout(location = 4) mat4 normal_transform;
uniform layout(location = 5) vec4 light_position;  // in view coordinates




out vec4 vNormal;
out vec4 vPosition;


out vec3 LightDir;
out vec3 EyeDir;
out vec2 TexCoord;


void
main()
{
//    vNormal = normal_transform * bNormal;
//    vPosition = mv_transform * bPosition;
//    gl_Position =  p_transform * vPosition;

    mat4 mvp_transform = p_transform * mv_transform;

    EyeDir = vec3(mv_transform * bPosition);
    TexCoord = bTextureCoord.st;
    vec3 n = normalize((normal_transform * bNormal).xyz);
    vec3 t = normalize((normal_transform * bTangent).xyz);
    vec3 b = cross(n, t);
    vec3 v;
    vec4 LightPosition = light_position;
    v.x = dot(LightPosition.xyz, t);
    v.y = dot(LightPosition.xyz, b);
    v.z = dot(LightPosition.xyz, n);
    LightDir = normalize(v);
    v.x = dot(EyeDir, t);
    v.y = dot(EyeDir, b);
    v.z = dot(EyeDir, n);
    EyeDir = normalize(v);


    gl_Position = mvp_transform * bPosition;


}
