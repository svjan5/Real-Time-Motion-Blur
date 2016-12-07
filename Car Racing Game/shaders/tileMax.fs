
uniform sampler2D TEX_VL;
uniform int K;

void main(void) {

        ivec2 ivTileCorner = ivec2(gl_FragCoord.xy) * int(K);

        int startx = int(gl_FragCoord.x) * int(K);
        int starty = int(gl_FragCoord.y) * int(K);
        int endx = startx + int(K);
        int endy = starty + int(K);

        float velMax = -1.0e10;
        vec2 velMaxVec = vec2(0.5);

        for (int y = starty; y < endy; ++y) {
                for (int x = startx; x < endx; ++x) {
                        vec2 velVec = texelFetch(TEX_VL, ivec2(x, y), 0).xy;
                        float velLen = length(velVec);

                        if (velLen > velMax) {
                                velMax = velLen;
                                velMaxVec = velVec;
                        }
                }
        }

        gl_FragColor = vec4(velMaxVec, 0.0, 1.0);
}