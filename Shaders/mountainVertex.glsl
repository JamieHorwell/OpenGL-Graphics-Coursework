#version 150 core

uniform sampler2D perlinTex;

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;

uniform vec4 clippingPlane; 

in  vec3 position;
in  vec4 colour;
in vec3 normal;
in vec3 tangent;
in vec2 texCoord;

out Vertex {
	vec4 colour;
	vec2 texCoord;
	vec3 normal;
	vec3 tangent;
	vec3 binormal;
	vec3 worldPos;
	vec4 shadowProj;
	float noise;
} OUT;


//used to smooth perlin noise
float fade(float t) {
	return t*t*t*(t*(t*6.0-15.0) + 10.0);
}

vec2 grad(vec2 p) {
	const float texture_width = 256.0;
	vec4 v = texture2D(perlinTex, vec2(p / texture_width));
	//make sure we remap our values from -1, 1 range rather than standard tex coord range
	return normalize(v.xy*2.0 - vec2(1.0));
}

float noise(vec2 p) {
	//calculate lattice points
	vec2 p0 = floor(p);
	vec2 p1 = p0 + vec2(1.0,0.0);
	vec2 p2 = p0 + vec2(0.0,1.0);
	vec2 p3 = p0 + vec2(1.0,1.0);
	
	//look up gradients at lattice points
	vec2 g0 = grad(p0);
	vec2 g1 = grad(p1);
	vec2 g2 = grad(p2);
	vec2 g3 = grad(p3);
	
	float t0 = p.x - p0.x;
	float fade_t0 = fade(t0);
	
	float t1 = p.y - p0.y;
	float fade_t1 = fade(t1);
	
	float p0p1 = (1.0 - fade_t0) * dot(g0, (p - p0)) + fade_t0 * dot(g1, (p - p1));
	float p2p3 = (1.0 - fade_t0) * dot(g0, (p - p2)) + fade_t0 * dot(g3, (p - p3));
	
	
	return (1.0 - fade_t1) * p0p1 + fade_t1 * p2p3;
}





void main(void)	{

	

	vec4 finalPos = (projMatrix * viewMatrix * modelMatrix) * vec4(position, 1.0);
	
	//noise generation
	float n = noise(vec2(position.x,position.y) * (1.0/300.0)) * 1.0 +
			  noise(vec2(position.x,position.y) * (1.0/150.0)) * 0.5;
		
	OUT.noise = n;

	OUT.colour = colour;
	OUT.texCoord = (textureMatrix * vec4(texCoord,0.0,1.0)).xy;
	
	mat3 normalMatrix = transpose(inverse(mat3(modelMatrix)));
	
	OUT.normal = normalize(normalMatrix * normalize(normal));
	OUT.tangent = normalize(normalMatrix * normalize(tangent));
	OUT.binormal = normalize(normalMatrix * normalize(cross(normal,tangent)));
	OUT.worldPos = (modelMatrix * vec4(position,1)).xyz;
	OUT.shadowProj = (textureMatrix * vec4(position+(normal*1.5),1));
	//clipping out vertexes
	gl_ClipDistance[0] = dot((modelMatrix * vec4(position,1)),clippingPlane);
	gl_Position = finalPos;
}