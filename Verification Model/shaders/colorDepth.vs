in vec3 Position;
in vec3 Normal;

// Output data ; will be interpolated for each fragment.
out vec3 texCoord;
out vec4 finalPos;
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
	vec4 newPosition = vec4(Position, 1.0f);

	texCoord = Position;
	gl_Position =  MVP * newPosition;

	finalPos = MVP * newPosition;
	Position_worldspace = (M * newPosition).xyz;
	vec3 vertexPosition_cameraspace = ( V * M * newPosition).xyz;
	EyeDirection_cameraspace = vec3(0, 0, 0) - vertexPosition_cameraspace;
	vec3 Light1Pos_cameraspace = ( V * vec4(LightPos, 1)).xyz;
	LightDir_cameraspace = Light1Pos_cameraspace + EyeDirection_cameraspace;
	Normal_cameraspace = ( V * M * vec4(Normal, 0)).xyz;
}