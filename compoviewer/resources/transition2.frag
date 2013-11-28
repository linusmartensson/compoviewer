#version 420 

uniform sampler2D texture_pre;
uniform sampler2D texture_post;

uniform float time;

in vec2 tc; 
out vec4 c; 

void main(){
	c = mix(texture2D(texture_pre, tc), texture2D(texture_post, tc), time);
	c.a = 1.0;
}
