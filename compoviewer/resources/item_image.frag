#version 400  
uniform sampler2D tex;
uniform vec2 image_resolution;
uniform vec2 resolution;
out vec4 c; 
void main(){

	vec2 p = (gl_FragCoord.xy*vec2(1.0,-1.0) + vec2(0.0,resolution.y) - resolution/2.0 + image_resolution/2.0)/ image_resolution;

//	vec2 p = (tc.xy*resolution - resolution/2.0+image_resolution/2.0)/image_resolution;
	c.rgba = 
		(p.y*image_resolution.y+2.0)/image_resolution.y<0.0
		||(p.x*image_resolution.x+2.0)/image_resolution.x<0.0||(p.y*image_resolution.y-2.0)/image_resolution.y>1.0||(p.x*image_resolution.x-2.0)/image_resolution.x>1.0?vec4(0.0):vec4(0.0);
	c.rgba = p.x<0.0||p.y<0.0||p.x>1.0||p.y>1.0?c.rgba:texture2D(tex, p);
	
	c.a = 1.0;
}