#version 420

const float speed = 10.0;
const float zoom = 10.0;
const float finalTone = 1.0;


uniform vec2 iResolution;
uniform float iGlobalTime;

out vec4 c;


#define FIX_ASPECT
#define VIGNETTING
#define BOXSTENCIL
#define ROTATE
#define MEGADETHLAZORS
vec3 orb(void)
{
	// uvset [0,1]
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	// [0,1] to [-1,1]
	vec2 p = 2.0 * uv - 1.0;
	
	// force 1:1 aspect ratio if defined
#ifdef FIX_ASPECT
	p.x *= iResolution.x/iResolution.y;
#endif
	float t = iGlobalTime * 10.0;
	vec3 color = vec3(0.0);
	float lp = length(p);
	p.x -= 1.0;

	// performance no/no	
#ifdef ROTATE
	float angle = (t/speed)*0.5;
	p *= mat2(cos(angle),-sin(angle),sin(angle),cos(angle));
#endif

	vec2 zp = p * zoom;
	vec2 zp2 = 0.2*zp;
#if 1
	// the outer bars
	float ofs = 1.0/cos(p.y);
	color.b = clamp(1.0,0.0,(5.0*sin(zp.x * ofs + t))-2.0);
	color.b += clamp(1.0,0.0,(3.0*sin(0.9*zp.x * ofs + t*0.6))-2.0);
	
	// the inner bulbe
	color.g += (1.0 - length(p * vec2(1.0,1.5))) * -color.r;
	//color.rgb += color.r;
	//color.rgb = color.r * vec3(0.5,0.5,0.5);	
#endif
	
	// box stencil / force nova
#ifdef BOXSTENCIL
	float stencil = ceil(1.0-clamp(1.0,0.0,lp));
	float boxstencil = stencil * length(p * sin(zp2.x) * sin(0.4*t-zp2.y));
	boxstencil = clamp(1.0,0.0,boxstencil);
	boxstencil += -1.4+sin(p.y+t*0.2)*2.0;
	color *= (0.6*boxstencil*10.0*(1.0-lp));
#endif
	
	
	color = vec3(color.b*0.3,color.b*0.7,color.b*3.0);
	color += length(p);
	
	// vignetting
#ifdef VIGNETTING
	color *= 1.0-clamp(1.0,0.0,length(p));
#endif
	
#ifdef MEGADETHLAZORS
	color = (color+length(p))*color.r;
#endif
	

	//clamp the color
	color = clamp(vec3(1.0),vec3(0.0),color);
	
	// output final color
	return color * finalTone;
}

void main(void)
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	vec2 p = 2.0*uv-1.0;
	float t = iGlobalTime * 3.0;
	p.x *= iResolution.x/iResolution.y;

	vec3 final = vec3(0.0);
	vec3 black = vec3(0.0);
	vec3 orange = vec3(1.0,159.0/255.0,0.0);
	vec3 grey = vec3(179.0/255.0,175.0/255.0,178.0/255.0);
	
	
	//create the border
	float border =  ceil(cos((p.y*3.0)));
	float border2 =  ceil(cos((p.y*6.0)));
	final += border * orange;
	vec2 zp = p * 300.0;
	float tiles = sin(zp.x - t*200.0)*sin(zp.y);
	tiles = clamp(1.0,0.0,tiles);
	final += tiles*0.05;
	
	// beam
	float ofs = sin(p.x+t)*0.2;
	ofs += sin(t*0.1)*sin(p.y*0.1);
	float beam = 0.0;
	beam += cos((p.y*2.0)+ofs);
	beam += cos((p.y*6.0)+ofs*2.0);
	beam = clamp(1.0,0.0,beam);
	final += beam * grey * border;
	ofs = cos(p.x+t*1.2+50.0)*0.2;
	beam += sin((p.y*5.0)+ofs);
	beam += cos((p.y*2.0)+ofs*5.0);
	beam = clamp(1.0,0.0,beam);
	final += beam * orange;
	final *= 0.1;
	

	final = clamp(vec3(1.0),vec3(0.0),final);	
	final *= clamp(1.0,0.0,1.0-length(p*0.3));
	final = vec3(1.0) - final;
	
	final += orb();
	
	if(border > 0.0)
		final *= 0.7;
	
	//final = vec3((final.r+final.g+final.b)*0.33)*orange;
	//final = vec3(1.0) - final;
	c = vec4(final,0.0);
}