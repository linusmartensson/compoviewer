#version 400
in vec4 text_position; 
uniform vec2 resolution; 

void main(){
	gl_Position=vec4((text_position.xy)*2.0-vec2(1.0), 0.0, 1.0); 
}