in vec4 currPos;
in vec4 prevPos;

vec2 encodeVelocity(vec2 vel){  return (vel + vec2(1.0)) * vec2(0.5); }
vec2 readBiasScale(vec2 vel) {  return (vel * vec2(2.0)) - vec2(1.0); }

void main() {
        vec2 cPos = currPos.xy / currPos.w;
        vec2 pPos = prevPos.xy / prevPos.w;
        vec2 vel  = cPos - pPos;
        
        gl_FragColor = vec4(vel, 0.0, 0.8);
}