#version 420
out vec2 tc; 

in vec2 transition_position;

void main(){
	gl_Position=vec4(transition_position.xy*2.0-1.0, 0.5, 1.0);
	tc = vec2(transition_position); 
}