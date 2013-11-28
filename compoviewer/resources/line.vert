#version 420

uniform float time;
uniform float rv;

in float line_position; 
out vec4 lp;

mat4 rotx(float a){
	float s = sin(a);
	float c = cos(a);

	return mat4(1.0,0.0,0.0,0.0,
				0.0,  c, -s,0.0,
				0.0,  s,  c,0.0,
				0.0,0.0,0.0,1.0);
}

mat4 roty(float a){
	float s = sin(a);
	float c = cos(a);

	return mat4(  c,0.0, -s,0.0,
				0.0,1.0,0.0,0.0,
				  s,0.0,  c,0.0,
				0.0,0.0,0.0,1.0);
}

mat4 rotz(float a){
	float s = sin(a);
	float c = cos(a);

	return mat4(  c, -s,0.0,0.0,
				  s,  c,0.0,0.0,
				0.0,0.0,1.0,0.0,
				0.0,0.0,0.0,1.0);
}


mat4 trans(vec3 t){

	return mat4(1.0,0.0,0.0,0.0,
				0.0,1.0,0.0,0.0,
				0.0,0.0,1.0,0.0,
				t.x,t.y,t.z,1.0);
}


void main(){

	float ftime = time*0.1 + rv*100.0;

	float f2time = 10.0*(time/(2.0+time)) + rv*100.0;

	float p = line_position*10.0;
	vec4 l = vec4(p-5.0, -0.5, 0.5, 1.0);
	

	float a =	 (f2time*1.9+ftime+p*5.0+p*sin(rv+1.0)*5.0+cos(l.x)*0.3)*0.3;
	float b = sin(f2time*1.8+p*cos(rv+2.0)*5.0+sin(  a)*0.3)*0.3;
	float c = cos(f2time*1.7+p*sin(rv+3.0)*7.0+cos(  b)*0.3)*0.3;
	float d = sin(f2time*1.6+p*cos(rv+4.0)*6.1+sin(  c)*0.3)*0.3;
	float e = cos(f2time*1.5+p*sin(rv+5.0)*3.0+cos(  d)*0.3)*0.3;
	float f = sin(f2time*1.4+p*cos(rv+6.0)*2.0+sin(  e)*0.3)*0.3;
	float g = sin(f2time*1.3+p*sin(rv+7.0)*2.7+cos(  f)*0.3)*0.3;
	float h = cos(f2time*1.2+p*sin(rv+8.0)*1.9-sin(  g)*0.3)*0.3;
	float i = sin(f2time*1.1+p*cos(rv+9.0)*2.8+cos(  h)*0.3)*0.3;
	vec3 t = vec3(d,e,f);
	vec3 u = vec3(g,h,i);

	l = trans(-u)*roty(b)*rotz(c)*trans(u)*trans(-t)*rotx(a)*trans(t)*l;
	
	
	lp.xy=l.xy;
	lp.z = 1.0;//(sin(l.z)+2.0);
	lp.w = sin(l.x+l.z)*0.1;
}