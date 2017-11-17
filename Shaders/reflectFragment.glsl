#version 150
uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;


uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;


in Vertex {
	vec4 colour;
	vec4 clipSpace;
	vec3 normal;
	vec3 worldPos;
} IN;

out vec4 FragColor;

void main(void){

	vec2 ndc = (IN.clipSpace.xy/IN.clipSpace.w)/2.0 + 0.5;
	vec2 refractTexCoord = vec2(ndc.x,ndc.y);
	vec2 reflectTexCoord = vec2(ndc.x,-ndc.y);
	
	vec4 reflection = texture(reflectionTex, reflectTexCoord);
	vec4 refraction = texture(refractionTex, refractTexCoord);
	
	vec4 finalwater = mix(vec4(0,0,1,1),mix(reflection, refraction, 0.5),0.8);
	
	vec3 incident = normalize(IN.worldPos - cameraPos);
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.2,1.0);
	
	
	
	FragColor = (lightColour*finalwater*atten);
	FragColor.a = 1;

	//FragColor = vec4(1,1,1,1);
}