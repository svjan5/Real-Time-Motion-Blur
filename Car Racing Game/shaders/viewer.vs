#version 330 core

in vec3 position;
out vec2 texCoord;

void main() {
	gl_Position = vec4(position.xy, 0.0, 1.0);
	texCoord = (position.xy + vec2(1.0,1.0))/2.0;
}