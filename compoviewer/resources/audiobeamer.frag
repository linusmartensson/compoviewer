#version 400

uniform vec2 iResolution;
uniform float iGlobalTime;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;
uniform sampler2D logo;
uniform vec2 logores;
uniform vec4 audiolevel;

uniform int category;

out vec4 c;


vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0; }

float mod289(float x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0; }

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

float permute(float x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float taylorInvSqrt(float r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

vec4 grad4(float j, vec4 ip)
  {
  const vec4 ones = vec4(1.0, 1.0, 1.0, -1.0);
  vec4 p,s;

  p.xyz = floor( fract (vec3(j) * ip.xyz) * 7.0) * ip.z - 1.0;
  p.w = 1.5 - dot(abs(p.xyz), ones.xyz);
  s = vec4(lessThan(p, vec4(0.0)));
  p.xyz = p.xyz + (s.xyz*2.0 - 1.0) * s.www; 

  return p;
  }

#define F4 0.309016994374947451

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec2 mod289(vec2 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec3 permute(vec3 x) {
  return mod289(((x*34.0)+1.0)*x);
}

float snoise(vec2 v)
  {
  const vec4 C = vec4(0.211324865405187,  // (3.0-sqrt(3.0))/6.0
                      0.366025403784439,  // 0.5*(sqrt(3.0)-1.0)
                     -0.577350269189626,  // -1.0 + 2.0 * C.x
                      0.024390243902439); // 1.0 / 41.0
// First corner
  vec2 i  = floor(v + dot(v, C.yy) );
  vec2 x0 = v -   i + dot(i, C.xx);

// Other corners
  vec2 i1;
  //i1.x = step( x0.y, x0.x ); // x0.x > x0.y ? 1.0 : 0.0
  //i1.y = 1.0 - i1.x;
  i1 = (x0.x > x0.y) ? vec2(1.0, 0.0) : vec2(0.0, 1.0);
  // x0 = x0 - 0.0 + 0.0 * C.xx ;
  // x1 = x0 - i1 + 1.0 * C.xx ;
  // x2 = x0 - 1.0 + 2.0 * C.xx ;
  vec4 x12 = x0.xyxy + C.xxzz;
  x12.xy -= i1;

// Permutations
  i = mod289(i); // Avoid truncation effects in permutation
  vec3 p = permute( permute( i.y + vec3(0.0, i1.y, 1.0 ))
		+ i.x + vec3(0.0, i1.x, 1.0 ));

  vec3 m = max(0.5 - vec3(dot(x0,x0), dot(x12.xy,x12.xy), dot(x12.zw,x12.zw)), 0.0);
  m = m*m ;
  m = m*m ;

// Gradients: 41 points uniformly over a line, mapped onto a diamond.
// The ring size 17*17 = 289 is close to a multiple of 41 (41*7 = 287)

  vec3 x = 2.0 * fract(p * C.www) - 1.0;
  vec3 h = abs(x) - 0.5;
  vec3 ox = floor(x + 0.5);
  vec3 a0 = x - ox;

// Normalise gradients implicitly by scaling m
// Approximation of: m *= inversesqrt( a0*a0 + h*h );
  m *= 1.79284291400159 - 0.85373472095314 * ( a0*a0 + h*h );

// Compute final noise value at P
  vec3 g;
  g.x  = a0.x  * x0.x  + h.x  * x0.y;
  g.yz = a0.yz * x12.xz + h.yz * x12.yw;
  return 130.0 * dot(m, g);
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

  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }

float snoise(vec4 v)
  {
  const vec4  C = vec4( 0.138196601125011, 
                        0.276393202250021,
                        0.414589803375032, 
                       -0.447213595499958); 

  vec4 i  = floor(v + dot(v, vec4(F4)) );
  vec4 x0 = v -   i + dot(i, C.xxxx);
  vec4 i0;
  vec3 isX = step( x0.yzw, x0.xxx );
  vec3 isYZ = step( x0.zww, x0.yyz );
  i0.x = isX.x + isX.y + isX.z;
  i0.yzw = 1.0 - isX;
  i0.y += isYZ.x + isYZ.y;
  i0.zw += 1.0 - isYZ.xy;
  i0.z += isYZ.z;
  i0.w += 1.0 - isYZ.z;
  vec4 i3 = clamp( i0, 0.0, 1.0 );
  vec4 i2 = clamp( i0-1.0, 0.0, 1.0 );
  vec4 i1 = clamp( i0-2.0, 0.0, 1.0 );

  vec4 x1 = x0 - i1 + C.xxxx;
  vec4 x2 = x0 - i2 + C.yyyy;
  vec4 x3 = x0 - i3 + C.zzzz;
  vec4 x4 = x0 + C.wwww;

  i = mod289(i); 
  float j0 = permute( permute( permute( permute(i.w) + i.z) + i.y) + i.x);
  vec4 j1 = permute( permute( permute( permute (
             i.w + vec4(i1.w, i2.w, i3.w, 1.0 ))
           + i.z + vec4(i1.z, i2.z, i3.z, 1.0 ))
           + i.y + vec4(i1.y, i2.y, i3.y, 1.0 ))
           + i.x + vec4(i1.x, i2.x, i3.x, 1.0 ));

  vec4 ip = vec4(1.0/294.0, 1.0/49.0, 1.0/7.0, 0.0) ;

  vec4 p0 = grad4(j0,   ip);
  vec4 p1 = grad4(j1.x, ip);
  vec4 p2 = grad4(j1.y, ip);
  vec4 p3 = grad4(j1.z, ip);
  vec4 p4 = grad4(j1.w, ip);
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;
  p4 *= taylorInvSqrt(dot(p4,p4));
  vec3 m0 = max(0.6 - vec3(dot(x0,x0), dot(x1,x1), dot(x2,x2)), 0.0);
  vec2 m1 = max(0.6 - vec2(dot(x3,x3), dot(x4,x4)            ), 0.0);
  m0 = m0 * m0;
  m1 = m1 * m1;
  return 49.0 * ( dot(m0*m0, vec3( dot( p0, x0 ), dot( p1, x1 ), dot( p2, x2 )))
               + dot(m1*m1, vec2( dot( p3, x3 ), dot( p4, x4 ) ) ) ) ;

  }

vec3 ahsv2rgb(vec3 h){
 return (clamp(abs(fract(h.x+vec3(1.0,0.6666667,0.3333333)) * 6.0 - 3.0) -1.0, 0.0, 1.0)-1.0)*h.y+h.z;	
}


void rot(inout vec2 a, in vec2 sico){
	a = vec2(dot(a,vec2(sico.y,-sico.x)),dot(a, sico));
}
float rstr;

float df(in vec3 pos){

vec3 ipo=pos;

	float m = 0.25;
	
	pos.x = int(pos.x*m)/m;
	
	float sn = snoise(vec3(iGlobalTime*0.1, pos.x,length(pos.yz)));
	sn *= -sn;

	float al = audiolevel.x;
	if(category != 1 && category != 5){
		al = 1.0;
	}

	float z = min(length(pos.yz*2.0)-al*1.75, 
			   sn*5.0+5.0-al*3.0 + dot(pos.yz,pos.yz)*0.1/(1.0+al*1.0));


	return z;
}

float volume;

void near_collision(in vec3 pos, inout vec3 color, in float im){

	float mm = clamp(1.0-(max(im,0.0)+1.0)*0.1,0.0,1.0);
	color += vec3(1.0,0.5,0.1)*0.015*clamp(10.0-im*2.5,0.0,10.0)*mm*mm;
}

void main(void)
{
    
					
	vec3 final = vec3(0.0);




	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	
	vec2 p = 2.0*uv-1.0;
	p.x *= iResolution.x/iResolution.y;
	
	rstr = audiolevel.x;

	vec3 o = vec3(p.xy*10.0,-5.0);

	o.y += 2.0;


	vec3 d = (vec3(p.x,p.y, 1.0));


	vec3 rp = normalize(vec3(p.xy*0.6,1.0));
	vec3 rrp = (vec3(p.xy,1.0));

	float a = -0.2;
	vec2 sico = vec2(sin(a),cos(a));
	rot(o.xy, sico);
	rot(d.xy, sico);
	rot(rp.xy, sico);
	rot(rrp.xy, sico);

	a = -0.1;
	sico = vec2(sin(a),cos(a));
	rot(o.xz, sico);
	rot(d.xz, sico);
	rot(rp.xz,sico);
								  
	float t = audiolevel.z*0.25;
	a = (t*3.0-iGlobalTime);
	sico = vec2(sin(a*0.1),cos(a*0.1));
	
	rrp.y -= a*0.1;

	rot(o.yz, sico);
	rot(d.yz, sico);
	rot(rp.yz,sico);
	o.x += a*3.0;
	rp.x += a*0.01;


	float s = 1.0;

	volume = texture2D(iChannel0, vec2(0.5,(rp.x-a*0.01)*0.5+0.31)).r;

	final.rgb += clamp(pow(sin(rp.x*320.0*1.0)+0.01,115.0),0.0,1.0)*0.1;
	final.rgb += clamp(pow(sin(rrp.y*120.0*1.0)+0.01,115.0),0.0,1.0)*0.1;

	int i=0;
	for(;i<32;++i){
		float m = (df(o+d*s));
			near_collision(o+d*(s), final, m);
		s += 0.2;
	}


	final = mix(final, vec3(1.0), clamp(1.0-distance(final, vec3(0.0)),0.0,1.0));

	final = mix(final, vec3(1.0,0.7,0.3)*3.0*clamp(vec3(volume*14.0*clamp(1.0-distance(rrp.y+a*0.1,0.5)*(10.0-volume*10.0),0.0,1.0)),vec3(0.0),vec3(1.0)), clamp(min(audiolevel.z*0.1,1.0)-distance(rrp.y+a*0.1,0.5)*2.0,0.0,8.0));

	final.rgb  = vec3(final.r*final.r*final.r, final.g*final.g, final.b*2.0);
	
	vec3 beamercolor = vec3(1.0,1.0,1.0);
	final = mix(beamercolor, final, min(max(-sign(uv.y-0.8),0.0)*max(-sign(0.2-uv.y),0.0),1.0));
	
	p = (gl_FragCoord.xy*vec2(1.0,-1.0) + vec2(-iResolution.x*0.4,iResolution.y*0.55) - iResolution/2.0 + logores/2.0)/logores;
	vec4 lg = texture2D(logo, p);
	if((p.x) < 1.0 && (p.y) < 1.0 && p.x > 0.0 && p.y > 0.0)
		final = mix(final, lg.rgb, lg.a);

	c = vec4(final,1.0);


	
}