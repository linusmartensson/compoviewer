#version 400  
uniform sampler2D tex;
uniform vec2 image_resolution;
uniform float pitchscale;
uniform vec2 resolution;
in vec2 tc; 
out vec4 c; 
void main(){

	vec2 p = (tc.xy*resolution - resolution/2.0+image_resolution/2.0)/image_resolution;
	c.rgba = vec4(0.0);
	c.rgba = p.x<0.0||p.y<0.0||p.x>1.0||p.y>1.0?c.rgba:texture2D(tex, p*vec2(pitchscale,1.0));
	
	c.a = 1.0;
}