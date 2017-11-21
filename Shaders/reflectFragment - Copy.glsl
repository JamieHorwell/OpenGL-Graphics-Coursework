#version 150
uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;




uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;


in Vertex {
	vec4 colour;
	vec3 normal;
	vec2 texCoord;
	vec3 worldPos;
} IN;

out vec4 FragColor;

void main(void){

	
	
	vec4 diffuse = texture(diffuseTex, IN.texCoord);
	
	vec3 incident = normalize( lightPos - IN.worldPos );
 float lambert = max(0.0 , dot ( incident , IN.normal ));
 float dist = length(lightPos - IN.worldPos);
 float atten = 1.0 - clamp( dist/lightRadius , 0.0 , 1.0);
 vec3 viewDir = normalize( cameraPos - IN.worldPos );
 vec3 halfDir = normalize( incident + viewDir );

 float rFactor = max (0.0 , dot( halfDir , IN.normal ));
 float sFactor = pow ( rFactor , 50.0 );
 
 vec3 colour = ( diffuse.rgb * lightColour.rgb );
 colour += ( lightColour.rgb * sFactor ) * 0.33;
 FragColor = vec4 ( colour * atten * lambert , diffuse.a );
 FragColor.rgb += ( diffuse.rgb * lightColour.rgb ) * 0.15;
 
 //first see if any colour appears
 //FragColor = vec4(1,0,0,1);
 
 //next see if texCoord are being correctly shown
 //FragColor = vec4(1,IN.texCoord.y,IN.texCoord.x,1);
 
 //now check  if texture is correctly being applied
 //FragColor = texture ( diffuseTex , IN.texCoord );
 
 //check light components work
 //FragColor = vec4(atten,atten,atten,1);
	
	
	
	FragColor = (lightColour*finalwater*atten);
	FragColor.a = 1;

	//FragColor = vec4(1,1,1,1);
}