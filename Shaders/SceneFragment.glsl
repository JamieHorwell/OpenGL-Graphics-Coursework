#version 150 core

uniform sampler2D diffuseTex;
uniform int useTexture;

in Vertex	{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 FragColor;

void main(void)	{
	//FragColor = vec4(1,1,0,1);
	//FragColor = IN.colour;
	
		FragColor = texture(diffuseTex, IN.texCoord);
		FragColor.a = 1;

}