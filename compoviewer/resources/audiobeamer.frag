#version 400

const float speed = 10.0;
const float zoom = 10.0;
const float finalTone = 1.0;


uniform vec2 iResolution;
uniform float iGlobalTime;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D logo;
uniform vec2 logores;


uniform vec4 audiolevel;

out vec4 c;
void main(void)
{
	vec3 final = vec3(0.0);

	vec3 ccc = vec3(0.98039215686,0.34901960784,0.0);

	vec2 uv = gl_FragCoord.xy / iResolution.yy;
	vec2 p = 2.0*uv-1.0;
	float t = iGlobalTime * 0.02;
	p.x *= iResolution.x/iResolution.y;
	

	float m = 0.4;
	float cr = cos(m);
	float sr = sin(m);
	uv-=0.5;
	vec2 r = vec2(cr*uv.x-sr*uv.y, sr*uv.x+cr*uv.y);
	vec2 r2 = uv+0.3;
	uv+=0.5;
	r+=0.5;
	float pos = sqrt(r2.x*r2.x+r2.y*r2.y)*0.5-texture2D(iChannel1, vec2(0.1,0.0),8.0).r*0.07;
	r2.x = iResolution.x/iResolution.y-uv.x-0.3*iResolution.y/iResolution.x;
	float pos2 = sqrt(r2.x*r2.x+r2.y*r2.y)*0.5-texture2D(iChannel1, vec2(0.1,1.0),8.0).r*0.07;
	final = vec3(0.85)-0.1*max(0.0,sign(cos((r.y+texture2D(iChannel0,vec2(0.5,(r.x+1.0)*0.125)).r*-10.0+t)*3.14159*10.0)));
	final += mix(vec3(0.0), ccc, clamp(texture2D(iChannel0,vec2(0.5,(r.x-r.y+1.0)*0.125)).r*10.0,0.0,1.0));
	final = mix(final, ccc, 
		max(
			sign(-0.05
				+abs(texture2D(iChannel0, vec2(0.0,mod(pos,1.0))).r)
				+abs(texture2D(iChannel0, vec2(1.0,mod(pos2,1.0))).r)
			),0.0));

			
	
	final = mix(vec3(0.85), final, max(-sign(uv.y-0.8),0.0)*max(-sign(0.2-uv.y),0.0));

	p = (gl_FragCoord.xy*vec2(1.0,-1.0) - iResolution/2.0 + logores/2.0) + vec2(-iResolution.x*0.45,iResolution.y*0.8)-logores*vec2(0.5,1.0);
	m = sin(t*100.0)*0.05;
	cr = cos(m);
	sr = sin(m);
	p = vec2(p.x*cr-p.y*sr,p.x*sr+p.y*cr);
	p += logores*vec2(0.5,1.0);
	p = (p)/logores;
	
	
	vec4 lg = texture2D(logo, p);
	if((p.x) < 1.0 && (p.y) < 1.0 && p.x > 0.0 && p.y > 0.0)
		final = mix(final, lg.rgb, lg.a);

	
	c = vec4(final,1.0);


	
}