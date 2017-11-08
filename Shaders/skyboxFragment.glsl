#version 150


uniform samplerCube cubeTex;
uniform vec3 cameraPos;


in Vertex {
	vec3 normal;
} IN;

out vec4 FragColor;

void main(void){

	FragColor = texture(cubeTex, normalize(IN.normal));
	//FragColor = vec4(1,0,1,1);
}