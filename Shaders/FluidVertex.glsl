#version 150 core

uniform sampler2D perlinTex;
uniform float time;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;



in vec3 position;
in vec4 colour;
in vec3 normal;
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 worldPos;
} OUT;


//used to smooth perlin noise
float fade(float t) {
	return t*t*t*(t*(t*6.0-15.0) + 10.0);
}

vec3 grad(vec3 p) {
	const float texture_width = 256.0;
	vec4 v = texture2D(perlinTex, vec2(p / texture_width));
	//make sure we remap our values from -1, 1 range rather than standard tex coord range
	return normalize(v.xyz*2.0 - vec3(1.0));
}

float noise(vec3 p) {
	//calculate lattice points
	vec3 p0 = floor(p);
	vec3 p1 = p0 + vec3(1.0,0.0,0.0);
	vec3 p2 = p0 + vec3(0.0,1.0,0.0);
	vec3 p3 = p0 + vec3(1.0,1.0,0.0);
	vec3 p4 = p0 + vec3(0.0,0.0,1.0);
	vec3 p5 = p4 + vec3(1.0,0.0,0.0);
	vec3 p6 = p4 + vec3(0.0,1.0,0.0);
	vec3 p7 = p4 + vec3(1.0,1.0,0.0);
	
	vec3 g0 = grad(p0);
	vec3 g1 = grad(p1);
	vec3 g2 = grad(p2);
	vec3 g3 = grad(p3);
	vec3 g4 = grad(p4);
	vec3 g5 = grad(p5);
	vec3 g6 = grad(p6);
	vec3 g7 = grad(p7);
	
	float t0 = p.x - p0.x;
  float fade_t0 = fade(t0);

  float t1 = p.y - p0.y;
  float fade_t1 = fade(t1);
    
  float t2 = p.z - p0.z;
  float fade_t2 = fade(t2);

  float p0p1 = (1.0 - fade_t0) * dot(g0, (p - p0)) + fade_t0 * dot(g1, (p - p1));
  float p2p3 = (1.0 - fade_t0) * dot(g2, (p - p2)) + fade_t0 * dot(g3, (p - p3));

  float p4p5 = (1.0 - fade_t0) * dot(g4, (p - p4)) + fade_t0 * dot(g5, (p - p5));
  float p6p7 = (1.0 - fade_t0) * dot(g6, (p - p6)) + fade_t0 * dot(g7, (p - p7));

  float y1 = (1.0 - fade_t1) * p0p1 + fade_t1 * p2p3;
  float y2 = (1.0 - fade_t1) * p4p5 + fade_t1 * p6p7;

  return (1.0 - fade_t2) * y1 + fade_t2 * y2;
}



void main(void)	{
	//noise generation
	float n = noise(vec3(position.x,position.z,time));
	vec3 newPos = position;
	newPos.z += n * 5;


	OUT.colour = colour;
	OUT.texCoord = (textureMatrix * vec4(texCoord,0.0,1.0)).xy;
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	
	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.worldPos = (modelMatrix * vec4(position,1)).xyz;
	gl_Position = (projMatrix * viewMatrix * modelMatrix) * vec4(newPos, 1.0);
	
}