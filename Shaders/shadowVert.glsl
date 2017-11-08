#version 150

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;


in vec3 position;

//we only need position for shadows

void main(void) {
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
}