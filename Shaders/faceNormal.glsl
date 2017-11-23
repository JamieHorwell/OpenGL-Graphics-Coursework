
#version 150 core
 


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in Vertex {
	vec4 colour;
	vec4 clipSpace;
	vec3 normal;
	vec3 newSpace;
	vec3 worldPos;
	vec3 toCam;
} IN[];


out Vertex {
	vec4 colour;
	vec4 clipSpace;
	vec3 normal;
	vec3 worldPos;
	vec3 toCam;
} OUT;
 
 
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
 
 
void main() {	
//gl_in is another GLSL keyword. It'll give us
//how many primitives we're accepting from
//the vertex shader. as we're taking in points
//our number of primitives is equal to our
//number of vertices in the bound VBO
	for(int i = 0; i < gl_in.length(); ++i) {
		//So, for every iteration of this loop (ie for every
		//point in our VBO, we are going to output 4 vertices

		vec3 pointA = IN[2].newSpace.xyz - IN[0].newSpace.xyz;
		
		vec3 pointB = IN[1].newSpace.xyz - IN[0].newSpace.xyz;
		
		mat4 mv = viewMatrix * modelMatrix;
		
		mat4 mvp = projMatrix * viewMatrix * modelMatrix;
		 
		mat4 normalMatrix = transpose(inverse(mv));  
		
		vec3 faceNormal = mat3(normalMatrix) * normalize(cross(pointA,pointB));
		//faceNormal = normalize(cross(pointA, pointB));
		
		OUT.colour = IN[0].colour;
		OUT.clipSpace = IN[0].clipSpace;
		OUT.normal = faceNormal;
		OUT.worldPos = IN[0].worldPos;
		OUT.toCam = IN[0].toCam;
		gl_Position = gl_in[0].gl_Position;
		EmitVertex();
		
		
		OUT.colour = IN[1].colour;
		OUT.clipSpace = IN[1].clipSpace;
		OUT.normal = faceNormal;
		OUT.worldPos = IN[1].worldPos;
		OUT.toCam = IN[1].toCam;
		gl_Position = gl_in[1].gl_Position;
		EmitVertex();
		
		OUT.colour = IN[2].colour;
		OUT.clipSpace = IN[2].clipSpace;
		OUT.normal = faceNormal;
		OUT.worldPos = IN[2].worldPos;
		OUT.toCam = IN[2].toCam;
		gl_Position = gl_in[2].gl_Position;
		EmitVertex();
		

		EndPrimitive();
	}
}

//That's pretty much everything for geometry shaders! If you've taken a look
//at the 'Drawing Text' tutorial, you might want to start thinking about how
//you could start using a geometry shader to render the quads used for every
//character of text. The Texture Atlas tutorial should also help in working
//out which texture coordinates to give to a vertex...