#version 420

uniform float iGlobalTime;
uniform vec2 iResolution;



vec3 COLOR1 = vec3(0.0, 0.0, 0.3);
vec3 COLOR2 = vec3(0.5, 0.0, 0.0);
float BLOCK_WIDTH = 0.01;

out vec4 c;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	uv = uv*2.0-1.0;


	c = vec4(uv.xy,1.0,1.0);

}