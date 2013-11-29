#version 420

uniform float iGlobalTime;
uniform vec2 iResolution;


vec3 COLOR1 = vec3(0.0, 0.0, 0.3);
vec3 COLOR2 = vec3(0.5, 0.0, 0.0);
float BLOCK_WIDTH = 0.01;

out vec4 c;

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

void main(void)
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	vec2 uvi = uv;
	// To create the BG pattern
	vec3 final_color = vec3(1.0);
	vec3 wave_color = vec3(0.0);
	
	
	// To create the waves
	float wave_width = 0.05;
	uv  = -1.0 + 2.0 * uv;
	
	float a = 0.3;
	
	float s = sin(a);
	float cc = cos(a);
	
	mat2 m = mat2(cc,-s,s,cc);
	
	uv.y += 0.4;
	
	vec2 uvf = uv;
	uv = m*uv;
	
	
	for(float i = 0.0; i < 3.0; i++) {
		
		uv.y += (0.07 * sin(uv.x + (i+1.0)*uv.x+ i/7.0 + iGlobalTime/(iGlobalTime+1.0) ));
		wave_width = abs(1.0 / (50.0 * pow(uv.y,2.0)));
		wave_color += vec3(wave_width * 255./255., wave_width*255./255., wave_width*0.2);
	}
	uv = m*uv;
	final_color += 1.0/wave_color;
	wave_color = vec3(0.0);
	for(float i = 0.0; i < 3.0; i++) {
		
		uv.y += (0.07 * sin(uv.x+2.0 + (i+0.3)*uv.x+ i/12.0 + iGlobalTime ));
		wave_width = abs(1.0 / (50.0 * pow(uv.y,2.0)))*10.0;
		wave_color += vec3(wave_width * 255./255., wave_width*102./255., wave_width*0.2)*0.1;
	}
	final_color -= wave_color;


	float t = iGlobalTime;
	vec2 p = uvi*2.0-1.0;
	p.x *= iResolution.x/iResolution.y;
	vec2 zp = p * 200.0;
	

	zp *= mat2(cos(-0.4+uvi.x*0.4),-sin(-0.4+uvi.x*0.4),sin(-0.4+uvi.x*0.4),cos(-0.4+uvi.x*0.4));

	float n2 = 0.5*(snoise(vec3(uvi*vec2(0.5,0.5),iGlobalTime*0.1))+snoise(vec3(uvi*vec2(0.5,0.5),iGlobalTime*0.1)));
	float tiles = pow(sin(zp.x*0.5 - t*2.0)*sin(zp.y*0.5),2.0)*2.0;
	
	c = vec4(final_color, 1.0)*0.9-uv.x*uv.x;
	c -= 1.0-clamp(1.0-pow(distance(uvf.x,0.0),16.0), 0.0,1.0);
	c *= clamp(distance(uvf.x*uvf.x,1.0),0.0,1.0);
	c = max(c, vec4(0.0))*1.0;
	c+=0.1;
	c = c/(c+1.0)+0.075;
	c+= (pow(tiles,8.0)*0.05-sqrt(tiles)*0.2)*0.05*n2;
	c.a = 1.0;
}