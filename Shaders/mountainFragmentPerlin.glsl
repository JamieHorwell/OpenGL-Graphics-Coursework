 # version 150 core

 uniform sampler2D diffuseTex;
 uniform sampler2D diffuseTexBump;
 
 uniform sampler2D topTex;
 uniform sampler2D topTexBump;
 
 uniform sampler2DShadow shadowTex;

 uniform vec3 cameraPos;
 uniform vec4 lightColour;
 uniform vec3 lightPos;
 uniform float lightRadius;

 in Vertex {
 vec3 colour;
 vec2 texCoord;
 vec3 normal;
 vec3 tangent;
 vec3 binormal;
 vec3 worldPos;
 vec4 shadowProj;
 float noise;
 } IN;

 out vec4 FragColor;

 void main ( void ) {
 
 ///// BLENDING 2 TEXTURES BASED ON HEIGHT ///////////
 vec4 diffuse;
 vec4 diffuse1;
 vec4 diffuse2;
 vec4 bump;
 vec4 bump1;
 vec4 bump2;
 
diffuse1 = texture ( diffuseTex , IN.texCoord );
diffuse2 = texture (topTex, IN.texCoord);

bump1 = texture(diffuseTexBump, IN.texCoord);
bump2 = texture(topTexBump, IN.texCoord);

float yValue = (IN.worldPos.y - 700 + (IN.noise* 800)) / 150;
yValue = clamp(yValue, 0.0, 1.0);

diffuse = mix(diffuse1,diffuse2,yValue);
bump = mix(bump1,bump2, yValue);


///// BUMPMAPPING ///////

mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);

vec3 normal = normalize(TBN * (bump.rgb * 2.0 - 1.0));


////// LIGHT CALCULATIONS ///////////

 vec3 incident = normalize( lightPos - IN.worldPos );
 float lambert = max(0.0 , dot ( incident , normal ));
 float dist = length(lightPos - IN.worldPos);
 float atten = 1.0 - clamp( dist/lightRadius , 0.0 , 1.0);
 vec3 viewDir = normalize( cameraPos - IN.worldPos );
 vec3 halfDir = normalize( incident + viewDir );

 float rFactor = max (0.0 , dot( halfDir , normal ));
 float sFactor = pow ( rFactor , 50.0 );
 
 /////// SHADOW CALCULATIONS /////////
 
 float shadow = 1.0;
 
 if(IN.shadowProj.w > 0.0) {
	shadow = textureProj(shadowTex,IN.shadowProj);
 }
 
 lambert *= shadow;
 
 ///FINAL COLOUR CALCULATIONS ////////
 
 vec3 colour = ( diffuse.rgb * lightColour.rgb );
 colour += ( lightColour.rgb * sFactor ) * 0.33;
 FragColor = vec4 ( colour * atten * lambert , diffuse.a );
 FragColor.rgb += ( diffuse.rgb * lightColour.rgb ) * 0.19;
 
 
 
 /////// DEBGUGGING VALUES ////////
 
 
 //first see if any colour appears
 //FragColor = vec4(1,0,0,1);
 
 //next see if texCoord are being correctly shown
 //FragColor = vec4(1,IN.texCoord.y,IN.texCoord.x,1);
 
 //now check  if texture is correctly being applied
 //FragColor = texture ( diffuseTex , IN.texCoord );
 
 //check light components work
 //FragColor = vec4(atten,atten,atten,1);

 }

