
// Interpolated values from the vertex shaders
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDir_cameraspace;
in vec3 MaterialColor;
in vec4 finalPos;
in vec3 texCoord;

uniform samplerCube TEX;

// Ouput data
out vec4 o_vFragColor;

// Values that stay constant for the whole mesh.
uniform vec3 LightPos;
uniform vec3 LightColor;

void Normalize(in vec3 v1, out vec3 v2){
	float length = v1.x*v1.x + v1.y*v1.y + v1.z*v1.z;
	length = sqrt(length);
	v2.x = v1.x/length;
	v2.y = v1.y/length;
	v2.z = v1.z/length;
}

void main() {

	float LightPower = 50.0;

	vec3 MaterialColor = texture(TEX, texCoord);

	vec3 MaterialAmbientColor = vec3(0.1, 0.1, 0.1) * MaterialColor;
	vec3 MaterialSpecularColor = vec3(1.0, 1.0, 1.0);
	float distance = length( LightPos - Position_worldspace );
	vec3 n; Normalize(Normal_cameraspace, n);
	vec3 l1, l2; Normalize(LightDir_cameraspace, l1);
	float cosTheta1 = clamp( abs(dot( n, l1 )), 0, 1 );
	vec3 E; Normalize(EyeDirection_cameraspace, E);
	vec3 R1 = reflect(-l1, n);
	float cosAlpha1 = clamp( dot( E, R1 ), 0, 1 );

	vec3 color = 	MaterialAmbientColor +
	        	(MaterialColor * LightColor * LightPower * cosTheta1 / (distance * distance)) +
	        	(MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha1, 32) / (distance * distance));

	gl_FragColor = vec4(color, 1.0);
}