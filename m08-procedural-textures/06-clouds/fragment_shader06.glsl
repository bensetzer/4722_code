#version 430 core

uniform sampler3D Noise; 
uniform vec4 SkyColor; // (0.0, 0.0, 0.8) 
uniform vec4 CloudColor; // (0.8, 0.8, 0.8) 

in float LightIntensity; 
in vec3 MCposition; 

out vec4 FragColor; 

void main() { 

	vec4 noisevec = texture(Noise, MCposition); 
	float intensity = (noisevec[0] + noisevec[1] + noisevec[2] + noisevec[3] + 0.03125) * 1.5; 
	vec4 color = mix(SkyColor, CloudColor, intensity) * LightIntensity; 
	FragColor = vec4(color.rgb, 1.0);
//    FragColor = vec4(normalize(noisevec.rgb), 1.0);

//	FragColor = SkyColor;
//	FragColor = CloudColor;

//	FragColor = vec4(abs(normalize(MCposition)), 1.0);
	//FragColor = vec4(abs(normalize(noisevec.xyz)), 1.0);

	//FragColor = vec4(1.0,0.0,1.0,1.0);
} 