#version 330 core

in vec2 texCoord;

out vec4 color;

uniform int texType;
uniform sampler2D renderedTex;
uniform int mK;

uniform float cameraFar;
uniform float cameraNear;

float readDepth(ivec2 iPos){
	float z, f = cameraFar, n = cameraNear;
	z = texelFetch(renderedTex, iPos, 0).r;
	return (2.0 * n) / (f + n - z * (f - n));
}

void main() {
	/* MAIN(0), COLOR(1), DEPTH(2), VELOCITY(3), TILE_MAX(4), NEIGHBOR_MAX(5) */
	switch(texType){

		case 0: /* MAIN */
			color = texture(renderedTex, texCoord) ;
			// color = vec4(1.0);
			break;

		case 1: /* COLOR */
			color = texture(renderedTex, texCoord) ;
			// color = vec4(1.0);
			break;
		case 2: /* DEPTH */
			float z = readDepth(ivec2(gl_FragCoord.xy));
			color = vec4(z,z,z,1.0);
			break;
		case 3: /* VELOCITY */
			vec2 vecVel = texture(renderedTex, texCoord).xy;
			// color = vec4(vecVel, 0.0, 1.0);
			if(length(vecVel) == 0)
				color = vec4(0,0,0,0);
			else
				color = vec4(vecVel + vec2(0.5), 0.0, 1.0);
			break;

		case 4: case 5: /* TILEMAX AND NEIGHBORMAX */
			ivec2 iPos = ivec2(gl_FragCoord.xy);
			vec2 vecNM = texelFetch(renderedTex, (iPos / mK), 0).rg;

			if(length(vecNM) == 0)
				color = vec4(0,0,0,0);
			else
				color = vec4(vecNM + vec2(0.5), 0.0, 0.5);

			break;

			
	}
};