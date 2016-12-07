in vec3 position;
in vec3 normal;
in vec2 UV;
in vec3 color;
in vec3 tangent;

// Output data ; will be interpolated for each fragment.
out vec3 texCoord;
out vec3 outColor;
out vec3 outTangent;
out vec2 outUV;
out vec3 Position_worldspace;
out vec3 Normal_cameraspace;
out vec3 EyeDirection_cameraspace;
out vec3 LightDir_cameraspace;

// Values that stay constant for the whole mesh.
uniform mat4 MVP;
uniform mat4 V;
uniform mat4 M;
uniform vec3 LightPos;

void main() {
	vec4 newPosition 		= vec4(position, 1.0);
	texCoord 			= position;
	gl_Position 			= MVP * newPosition;
	Position_worldspace 		= (M * newPosition).xyz;
	vec3 vertexPosition_cameraspace = ( V * M * newPosition).xyz;
	EyeDirection_cameraspace 	= vec3(0, 0, 0) - vertexPosition_cameraspace;
	vec3 Light1Pos_cameraspace 	= ( V * vec4(LightPos, 1)).xyz;
	LightDir_cameraspace 		= Light1Pos_cameraspace + EyeDirection_cameraspace;
	Normal_cameraspace 		= ( V * M * vec4(normal, 0)).xyz;

	outUV = UV;
	outColor = color;
	outTangent = tangent;
}