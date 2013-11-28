#version 420 

out vec4 c; 

in float tx;


in vec4 p;

uniform float time;

float saturate(float f){
	return clamp(f, 0.0,1.0);
}

void main(){ 

	float edge = 1.0 - distance(tx,0.5)*2.0;

	c = vec4(0.5,1.0,1.0,1.0)*edge;

	c *= clamp(-p.x-5.0+time*5.0,0.0,2.0);


	c = clamp(c, vec4(0.0), vec4(1.0));
}
