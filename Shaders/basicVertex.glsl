#version 150 core

uniform mat4 viewProjMatrix;


in  vec3 pos;
in  vec3 colour;

out Vertex {
	vec4 colour;
} OUT;

void main(void)	{
	gl_Position	  = vec4(pos, 1.0);
	OUT.colour    = vec4(colour,1);
}