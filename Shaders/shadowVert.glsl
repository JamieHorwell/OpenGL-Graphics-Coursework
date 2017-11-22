#version 150

uniform vec3 cameraPos;


uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


in vec3 position;

//we only need position for shadows

void main(void) {
	vec4 offsetPos = modelMatrix * vec4(position, 1.0);
	vec3 Direc =  cameraPos - offsetPos.xyz;
	Direc = normalize(Direc);
	offsetPos.xyz = offsetPos.xyz - (Direc * 120);
	gl_Position = (projMatrix * viewMatrix) * offsetPos;
}