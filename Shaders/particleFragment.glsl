#version 150
uniform sampler2D diffuseTex;
uniform float lifetime;


in Vertex {
	vec2 texCoord;
} IN;

out vec4 FragColor;

void main(void){
	
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	
	if(diffuse.r < 0.07 && diffuse.g < 0.07 && diffuse.b < 0.07) {
		discard;
	}
	diffuse.a -= lifetime / 6000; 
	FragColor = diffuse;
	
}