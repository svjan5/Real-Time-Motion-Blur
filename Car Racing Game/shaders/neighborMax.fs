uniform sampler2D TEX_TM;

in vec2 texCoord;
out vec4 color;

void main(void) {
        int u = int(gl_FragCoord.x);
        int v = int(gl_FragCoord.y);

        float velMax = -1.0e10;
        vec2 velMaxVec = vec2(0.5);

        for (int x = u - 1; x <= u + 1; ++x) {
                for (int y = v - 1; y <= v + 1; ++y) {
                        vec2 vvec = texelFetch(TEX_TM, ivec2(x, y), 0).xy;
                        float velLen = length(vvec);
                        if (velLen > velMax) {
                                velMax = velLen;
                                velMaxVec = vvec;
                        }
                }
        }

        color = vec4(velMaxVec, 0.0, 1.0);
        // color = texture(TEX_TM, texCoord);
}