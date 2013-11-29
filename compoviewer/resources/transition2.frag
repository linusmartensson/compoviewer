#version 400 

uniform sampler2D texture_pre;
uniform sampler2D texture_post;

uniform float time;

in vec2 tc; 
out vec4 c; 

void main(){
	c = mix(texture2D(texture_pre, tc), 
			texture2D(texture_post, tc), 
			clamp(time*5.0+sin(tc.x*12.0)+cos(tc.y*643.0)-2.0,0.0,1.0));
	c.a = 1.0;
}
