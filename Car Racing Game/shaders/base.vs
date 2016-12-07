#version 330 core

in vec3 position;
out vec2 texCoord;

void main() {
	gl_Position = vec4(position, 1.0);

	/* Texture coordinates are in [0,1]x[0,1] */
	texCoord = (position.xy + vec2(1.0))/2.0; 
}