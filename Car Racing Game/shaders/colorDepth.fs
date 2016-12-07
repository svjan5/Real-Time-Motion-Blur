in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDir_cameraspace;
in vec3 MaterialColor;
in vec3 texCoord;
in vec3 outColor;
in vec3 outTangent;
in vec2 outUV;

uniform samplerCube texCube;
uniform sampler2D texRect;

// Values that stay constant for the whole mesh.
uniform vec3 LightPos;
uniform vec3 LightColor;
uniform int colorMode;

void main() {

	float LightPower = 50.0;
	vec3 MaterialColor;

	switch(colorMode){
		case 0:
			MaterialColor = outColor;
			break;
		case 1:
			MaterialColor = texture(texCube, texCoord);
			gl_FragColor = vec4(MaterialColor,1.0);
			return;
			break;
		case 2:
			MaterialColor = texture(texRect, texCoord.xy);
			gl_FragColor = vec4(MaterialColor, 1.0);
			return;
			break;
		
	}

	vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialColor;
	vec3 MaterialSpecularColor = vec3(1.0, 1.0, 1.0);
	// float distance = length( LightPos - Position_worldspace );
	float distance = 7.0;

	vec3 n 	= normalize(Normal_cameraspace);
	vec3 l1 = normalize(LightDir_cameraspace);

	float cosTheta1 = clamp( abs(dot( n, l1 )), 0, 1 );
	vec3 E  = normalize(EyeDirection_cameraspace);

	vec3 R1 = reflect(-l1, n);
	float cosAlpha1 = clamp( dot( E, R1 ), 0, 1 );

	vec3 color = 	MaterialAmbientColor +
	        	(MaterialColor * LightColor * LightPower * cosTheta1 / (distance * distance)) +
	        	(MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha1, 32) / (distance * distance));

	gl_FragColor = vec4(color, 1.0);
}