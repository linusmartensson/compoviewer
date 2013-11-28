#version 420

const float speed = 10.0;
const float zoom = 10.0;
const float finalTone = 1.0;


uniform vec2 iResolution;
uniform float iGlobalTime;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

out vec4 c;


#define FIX_ASPECT
#define VIGNETTING
#define BOXSTENCIL
#define ROTATE
#define MEGADETHLAZORS
vec3 oorb(void)
{
	// uvset [0,1]
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	
	// [0,1] to [-1,1]
	
	vec2 p = 2.0 * uv - 1.0;
	p -= vec2(0.75,-0.2);
	p *= 1.5*(-texture2D(iChannel0,vec2(0.0,0.5)).r*10.0+1.5);
	
	// force 1:1 aspect ratio if defined
#ifdef FIX_ASPECT
	p.x *= iResolution.x/iResolution.y;
#endif
	float t = iGlobalTime * 10.0;
	vec3 color = vec3(0.0);
	float lp = length(p);
	

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
	color.b = clamp(abs((5.0*sin(zp.x * ofs + t))-2.0),0.0,1.0);
	color.b += clamp(abs((3.0*sin(0.9*zp.x * ofs + t*0.6))-2.0),0.0,1.0);
	
	// the inner bulbe
	color.g += (1.0 - length(p * vec2(1.0,1.5))) * -color.r;
	//color.rgb += color.r;
	//color.rgb = color.r * vec3(0.5,0.5,0.5);	
#endif
	
	// box stencil / force nova
#ifdef BOXSTENCIL
	float stencil = ceil(1.0-clamp(lp,0.0,1.0));
	float boxstencil = stencil * length(p * sin(zp2.x) * sin(0.4*t-zp2.y));
	boxstencil = clamp(abs(boxstencil),0.0,1.0);
	boxstencil += -1.4+sin(p.y+t*0.2)*2.0;
	color *= (0.6*boxstencil*10.0*(1.0-lp));
#endif
	
	
	color = vec3(color.b*0.3,color.b*0.7,color.b*3.0);
	color += length(p);
	
	// vignetting
#ifdef VIGNETTING
	color *= 1.0-clamp(abs(length(p)),0.0,1.0);
#endif
	
#ifdef MEGADETHLAZORS
	color = (color+length(p))*color.r;
#endif
	

	//clamp the color
	color = clamp(abs(color),vec3(0.0),vec3(1.0));
	
	// output final color
	return color * finalTone;
}



//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }

vec3 orb(void)
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	vec2 p = uv;
	
	p.x *= iResolution.x/iResolution.y;

	float t = iGlobalTime * 0.05+texture2D(iChannel1,vec2(0.0,0.5)).r*0.1;

	vec3 o = vec3(
		snoise(vec3(p.xy*0.5,t)+vec3(2000.0))*5.0,
		snoise(vec3(p.xy*0.5,t)+vec3(1000.0))*5.0,
		snoise(vec3(p.xy*0.5,t)+vec3(3000.0))*5.0
	);

	mat4 trans = mat4(1.0,0.0,0.0,0.0,
					  0.0,1.0,0.0,0.0,
					  0.0,0.0,1.0,0.0,
					  o.x,o.y,o.z,1.0);
	mat4 itrans = mat4(1.0,0.0,0.0,0.0,
					  0.0,1.0,0.0,0.0,
					  0.0,0.0,1.0,0.0,
					  -o.x,-o.y,-o.z, 1.0);
	vec4 color = vec4(p.xy, 1.0,1.0)*trans;
	float a = cos(length(color)+t*0.2);
	float b = sin(length(color)+t*0.2);
	mat4 rot = mat4(  a, -b,0.0,0.0,
					  b,  a,0.0,0.0,
					0.0,0.0,1.0,0.0,
					0.0,0.0,0.0,1.0)*
			   
			   mat4(  a,0.0, -b,0.0,
					0.0,1.0,0.0,0.0,
					  b,0.0,  a,0.0,
					0.0,0.0,0.0,1.0)*
					
			   mat4(1.0,0.0,0.0,0.0,
					0.0,  a, -b,0.0,
					0.0,  b,  a,0.0,
					0.0,0.0,0.0,1.0);
	color = (vec4(p.xy,0.0,1.0)*trans*rot*itrans);
		 
	// output final color
	return max(color.yyy,0.0);
}

void main(void)
{
vec3 final = vec3(0.0);

	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	vec2 p = 2.0*uv-1.0;
	float t = iGlobalTime * 3.0;
	p.x *= iResolution.x/iResolution.y;
	
	//create the border
	float border =  ceil(cos((p.y*3.0+5.0*min(0.0,sign(p.y))*texture2D(iChannel0, vec2(uv.x,0.5)))));
	float border3 =  ceil(cos((p.y*3.0+6.0*min(0.0,sign(p.y))*texture2D(iChannel0, vec2(uv.x,0.5)))));
	float border2 =  ceil(cos((p.y*3.0)));
	vec2 zp = p * 200.0;

	zp *= mat2(cos(-0.4+uv.x*0.4),-sin(-0.4+uv.x*0.4),sin(-0.4+uv.x*0.4),cos(-0.4+uv.x*0.4));

	float n2 = 0.5*(snoise(vec3(uv*vec2(0.5,0.5),iGlobalTime*0.1))+snoise(vec3(uv*vec2(0.5,0.5),iGlobalTime*0.1)));
	float tiles = pow(sin(zp.x*0.5 - t*2.0)*sin(zp.y*0.5),2.0)*2.0;
	final += tiles*0.0125*distance(uv,vec2(1.0,0.5));
	
	// beam
	float ofs = sin(p.x+t*0.5)*0.2+0.1;
	ofs += sin(t*0.3)*sin(p.y*0.1);
	float beam = 0.0;
	beam += cos((p.y*2.0)+ofs);
	beam += cos((p.y*5.0)+ofs*2.0);
	beam = beam;
	final += clamp(beam,0.0,1.0) * vec3(1.0,3.0,30.0)* 0.01;
	ofs = cos(p.x+t*0.7+50.0)*0.2+0.1;
	beam += sin((p.y*5.0)+ofs*7.0);
	beam += cos((p.y*7.0)+ofs*5.0);
	beam = clamp(beam,0.0,1.0);
	final += beam*vec3(0.1,0.3,1.0)*0.01;
	
	
//	final *= clamp(1.0-length(p*0.3),0.0,1.0);
	
	
	final += orb();
	final = 1.0-final/(final+1.0);
	final += oorb();
	final += mix(vec3(0.0),vec3(0.3,0.7,1.0)+0.45*vec3(1.0,0.5,0.3)*(pow(tiles,8.0)*0.05-sqrt(tiles)*0.2)*0.1*n2,1.0-final.b);
	
	
	float f = clamp(((1.0-border)*border2),0.0,1.0);

	final = mix(final*sin(f*3.14159+3.14159*0.5), -final*0.0*(1.0-length(p+vec2(0.5,0.0)))+1.0, step(border2, 0.0));
	
	
	
	c = vec4(final,1.0);
	
}