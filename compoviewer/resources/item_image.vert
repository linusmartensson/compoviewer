#version 400
in vec4 text_position; 
out vec2 tc; 
uniform vec2 resolution; 

void main(){
	gl_Position=vec4((text_position.xy/resolution)*2.0-vec2(1.0), 0.0, 1.0); 
	tc = text_position.zw; 
}