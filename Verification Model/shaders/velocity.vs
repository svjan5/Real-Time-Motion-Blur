in vec3 position; // In BaseShader
out vec4 currPos;
out vec4 prevPos;

uniform mat4 currMVP;
uniform mat4 prevMVP;
uniform mat4 proj;

void main(){
	vec4 pos = vec4(position, 1.0);
	currPos = currMVP * pos;
	prevPos = prevMVP * pos;

	gl_Position = proj * currPos;
}
