#version 150
uniform sampler2D diffuseTex;
uniform samplerCube cubeTex;

uniform sampler2D reflectionTex;
uniform sampler2D refractionTex;


uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;


in Vertex {
	vec4 colour;
	vec4 clipSpace;
	vec3 normal;
	vec3 worldPos;
	vec3 toCam;
} IN;

out vec4 FragColor;

void main(void){

	//// REFLECTION/REFRACION /////////

	vec2 ndc = (IN.clipSpace.xy/IN.clipSpace.w)/2.0 + 0.5;
	vec2 refractTexCoord = vec2(ndc.x,ndc.y);
	vec2 reflectTexCoord = vec2(ndc.x,-ndc.y);
	
	vec4 reflection = texture(reflectionTex, reflectTexCoord);
	vec4 refraction = texture(refractionTex, refractTexCoord);
	
	vec3 view = normalize(IN.toCam);
	float refractFactor = dot(view, IN.normal);
	refractFactor = pow(refractFactor, 2.0);
	
	vec4 finalwater = mix(vec4(0,0.1,1,1),mix(reflection, refraction, refractFactor),0.9);
	
	/////////////LIGHT////////////////////
	
	
	
	vec3 incident = normalize(IN.worldPos - cameraPos);
	float lambert = max(0.0 , dot ( incident , IN.normal ));
	float dist = length(lightPos - IN.worldPos);
	float atten = 1.0 - clamp(dist / lightRadius, 0.2,1.0);
	
	vec3 viewDir = normalize( cameraPos - IN.worldPos );
	vec3 halfDir = normalize( incident + viewDir );

	float rFactor = max ( dot( halfDir , IN.normal ), 0.0);
	float sFactor = pow ( rFactor , 2000.0 );
	
	vec4 specHighlights = lightColour * sFactor * rFactor;
	
 vec3 colour = ( finalwater.rgb * finalwater.rgb );
 //colour += ( finalwater.rgb * sFactor ) * 0.75;
 FragColor = vec4 ( colour * atten * lambert , finalwater.a );
 FragColor.rgb += ( finalwater.rgb * finalwater.rgb ) * 0.8;
 FragColor.rgb += specHighlights.rgb * 100;	
	
	//FragColor = (lightColour*finalwater*atten);
	//FragColor.a = 1;
 //FragColor = vec4(IN.normal,1);

}