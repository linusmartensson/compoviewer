#version 420

layout(lines) in;
layout(triangle_strip, max_vertices=32) out;

uniform float time;

in vec4 lp[]; 

out float tx;

out vec4 p;

void main(){

	tx = 0.0;
	gl_Position=vec4(lp[0].x+lp[0].w,lp[0].y-0.3*lp[0].z, 0.5, 1.0); 
	p = gl_Position;
	EmitVertex();
	tx = 0.0;
	gl_Position=vec4(lp[1].x+lp[1].w,lp[1].y-0.3*lp[1].z, 0.5, 1.0); 
	p = gl_Position;
	EmitVertex();
	tx = 1.0;
	gl_Position=vec4(lp[0].x-lp[0].w,lp[0].y+0.3*lp[0].z, 0.5, 1.0); 
	p = gl_Position;
	EmitVertex();
	tx = 1.0;
	gl_Position=vec4(lp[1].x-lp[1].w,lp[1].y+0.3*lp[1].z, 0.5, 1.0); 
	p = gl_Position;
	EmitVertex();
	EndPrimitive();
}