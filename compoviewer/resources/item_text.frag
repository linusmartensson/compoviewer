#version 400 
uniform vec4 color; 
uniform sampler2D tex;
in vec2 tc; 
out vec4 c; 
void main(){ 
	c.rgb = color.rgb; 
	c.a = texture2D(tex, tc.xy).r*clamp(color.a,0.0,1.0);
}