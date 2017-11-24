
#version 150 core
 


layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;


in Vertex {
	vec2 texCoord;
} IN[];


out Vertex {
	vec2 texCoord;
} OUT;
 
 
uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform mat4 projMatrix;
uniform mat4 textureMatrix;
 
 
uniform float time; 
 
 
 //function found online
 mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}
 
 
 
void main() {	

	for(int i = 0; i < gl_in.length(); ++i) {
		

		vec3 pointA = gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		
		vec3 pointB = gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz;
		
		mat4 mv = viewMatrix * modelMatrix;
		
		mat4 mvp = projMatrix * viewMatrix * modelMatrix;
		 
		mat4 normalMatrix = transpose(inverse(mv));  
		
		vec3 faceNormal = mat3(normalMatrix) * normalize(cross(pointA,pointB));
		//faceNormal = normalize(cross(pointA, pointB));
		
		OUT.texCoord = IN[0].texCoord;
		gl_Position = (gl_in[0].gl_Position + vec4(faceNormal * (time* time), 1));
		EmitVertex();
		
		
		OUT.texCoord = IN[1].texCoord;
		
		gl_Position = (gl_in[1].gl_Position +  vec4(faceNormal * (time* time), 1));
		EmitVertex();
		
		
		OUT.texCoord = IN[2].texCoord;
		gl_Position = (gl_in[2].gl_Position + vec4(faceNormal * (time* time), 1));
		EmitVertex();
		

		EndPrimitive();
	}
}

//That's pretty much everything for geometry shaders! If you've taken a look
//at the 'Drawing Text' tutorial, you might want to start thinking about how
//you could start using a geometry shader to render the quads used for every
//character of text. The Texture Atlas tutorial should also help in working
//out which texture coordinates to give to a vertex...