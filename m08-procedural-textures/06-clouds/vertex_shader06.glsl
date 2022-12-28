
#version 430 core

uniform mat4 MVMatrix; 
//uniform mat4 MVPMatrix; 
uniform mat4 NormalMatrix; 
uniform vec4 LightPos; 
uniform float Scale; 
uniform mat4 p_transform;

in layout(location = 1) vec4 MCvertex;
in  layout(location = 2) vec4 MCnormal;

out float LightIntensity; 
out vec3 MCposition; 

void main() { 

	vec3 ECposition = vec3(MVMatrix * MCvertex); 
	MCposition = vec3(MCvertex) * Scale; 
	vec3 tnorm = normalize(vec3(NormalMatrix * MCnormal)); 
	LightIntensity = dot(normalize(LightPos.xyz - ECposition), tnorm); 
	LightIntensity *= 1.5; 
	
	//gl_Position = MVPMatrix * MCvertex; 
	//gl_Position = MCvertex;
	//gl_Position = MVMatrix * MCvertex;
	gl_Position = p_transform * MVMatrix * MCvertex;
}