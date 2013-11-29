#version 400 
uniform vec4 color; 
uniform sampler2D tex; 
uniform float time; 
in vec2 tc; 
out vec4 c; 
void main(){ 
	c = texture2D(tex, 
		tc.xy+
		(fract(sin(pow(tc.xy*3212.01241511,vec2(2.0))))-0.5)
			*0.15
			*smoothstep(1.0,0.0,time-gl_FragCoord.x*0.005)).rrrr
	*(time-gl_FragCoord.x*0.005)*color; 
}
