out vec4 color;

uniform sampler2D TEX_C;
uniform sampler2D TEX_Z;
uniform sampler2D TEX_VL;
uniform sampler2D TEX_NM;
uniform float cameraNear;
uniform float cameraFar;
uniform int K;
uniform int S;
uniform float fps;

const float exposureTime = 0.1;
const float SOFT_Z_EXTENT = 0.1;

float readDepth(ivec2 iX){
        float z, f = cameraFar, n = cameraNear;
        z = texelFetch(TEX_Z, iX, 0).r;
        return ( 2 * n) / (f + n - z * (f - n));
}

float rand(vec2 co) {
        return fract(sin(dot(co.xy , vec2(12.9898, 78.233))) * 43758.5453);
}

float softDepthCompare(float za, float zb) {
        return clamp(1.0 - ((za - zb) / SOFT_Z_EXTENT), 0.0001, 1.0);
}

float cone(vec2 x, vec2 y, vec2 v) {
        return clamp(1.0 - length(x - y) / length(v), 0.001, 1.0);
}

float cylinder(vec2 x, vec2 y, vec2 v) {
        return 1.0 - smoothstep(0.95 * length(v), 1.05 * length(v), length(x - y));
}

vec2 amplifyVelocity(vec2 vel){
        float length = clamp(exposureTime * fps * length(vel), 0.0, K * 3.0);
        vel = normalize(vel) * length;
        return vel;
}

void main() {
        vec2  X       = gl_FragCoord.xy;
        ivec2 iX      = ivec2(X);
        ivec2 iXByk   = iX / K;

        vec2 vnX        = texelFetch(TEX_NM, iXByk, 0).xy;   /* NeighBor Max Velocity */
        vec2 vX         = texelFetch(TEX_VL, iX, 0).xy;      /* Pixel Velocity */
        vec3 cX         = texelFetch(TEX_C,  iX, 0).rgb;      /* Pixel Color */
        float zX        = readDepth(iX);

        if(length(vnX) < 0.005){
                color = texelFetch(TEX_C, iX, 0); /* No BLUR */
                return;
        }

        vnX             = amplifyVelocity(vnX);
        vX              = amplifyVelocity(vX);
        float lv        = clamp(length(vX), 1.0, K);

        float weight    = 1.0 / lv; 
        vec3 sum        = cX * weight;
        float j         = rand(X) - 0.5;

        for(int i = 0; i < S; i++){
                if(i == (S - 1)/2) continue;
                
                float t  = mix(-1.0, 1.0, (float(i)+1.0+j) / float(S+1));
                vec2 Y   = floor(X + vnX * t + vec2(0.5));
                ivec2 iY = ivec2(Y);

                float zY = readDepth(iY);
                vec2  vY = texelFetch(TEX_VL, iY, 0).xy;

                float f  = softDepthCompare(zX, zY);
                float b  = softDepthCompare(zY, zX);

                float aY =      f * cone(Y, X, vY) +
                                b * cone(X, Y, vX) +
                                cylinder(Y, X, vY) * cylinder(X, Y, vX) * 2.0;

                vec3 cY  = texelFetch(TEX_C, iY, 0).rgb;

                weight += aY;
                sum += aY * cY;
        }

        color = vec4(sum / weight, 1.0);
};
