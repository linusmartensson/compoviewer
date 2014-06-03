#version 400

const float speed = 10.0;
const float zoom = 10.0;
const float finalTone = 1.0;


uniform vec2 iResolution;
uniform float iGlobalTime;


uniform vec4 audiolevel;

out vec4 c;
void main(void)
{
	vec3 final = vec3(0.0);

	vec3 ccc = vec3(0.98039215686,0.34901960784,0.0);

	vec2 uv = gl_FragCoord.xy / iResolution.yy;
	vec2 p = 2.0*uv-1.0;
	float t = iGlobalTime * 0.01;
	p.x *= iResolution.x/iResolution.y;
	

	float m = t*0.5;
	float cr = cos(m);
	float sr = sin(m);
	uv-=0.5;
	vec2 r = vec2(cr*uv.x-sr*uv.y, sr*uv.x+cr*uv.y);
	vec2 r2 = round((uv+0.3)*10.0)/10.0;
	uv+=0.5;
	r+=0.5;
	float pos = sqrt(r2.x*r2.x+r2.y*r2.y)-t*3.0;	 
	float pos2 = sqrt(r2.x*r2.x+r2.y*r2.y)-t*3.0;
	final = vec3(0.85)-0.1*max(0.0,sign(cos((r.x+r.y+t)*3.14159*10.0)));
	final = mix(final, vec3(0.45), max(sin(34.0*pos)*cos(3.0*pos2+sin(r2.x*10.0)),0.0));

	c = vec4(final,1.0);
	
}