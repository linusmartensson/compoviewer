#version 400

#define STEPS	64

#define pi 3.14159265

uniform vec2 iResolution;
uniform sampler2D iChannel0;
uniform sampler2D iChannel1;

uniform float iGlobalTime;
float iFade;
vec2 spos;
out vec3 color;


////////////////
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
//////////


float sdBox( vec3 p, vec3 b )
{
  vec3 d = abs(p) - b;
  return min(max(d.x,max(d.y,d.z)),0.0) + length(max(d,0.0));
}

float smin( float a, float b, float k )
{
    float h = clamp( 0.5+0.5*(b-a)/k, 0.0, 1.0 );
    return mix( b, a, h ) - k*h*(1.0-h);
}

void rot(inout vec2 a, in vec2 sico){
	a = vec2(dot(a,vec2(sico.y,-sico.x)),dot(a, sico));
}

float obj( in vec3 p)
{
	float a,b,c,d,e,f,g;

	a = length(p + vec3(sin(iGlobalTime*1.4)*2.0,sin(iGlobalTime)*2.0,cos(-iGlobalTime*1.2))) - 1.2;
	b = length(p + vec3(cos(iGlobalTime*1.2)*2.1,-sin(iGlobalTime*1.5)*2.0,-cos(iGlobalTime*1.2))) - 1.3;
	c = length(p + vec3(-cos(iGlobalTime*1.3)*2.3,-sin(iGlobalTime*1.2)*2.0,cos(iGlobalTime*1.2))) - 1.4;
	d = length(p + vec3(-sin(iGlobalTime*1.4)*2.2,sin(iGlobalTime*1.6)*2.0,-cos(iGlobalTime*1.5))) - 1.5;
	e = length(p + vec3(-cos(iGlobalTime*1.4)*2.1,-sin(iGlobalTime*1.2)*2.0,cos(iGlobalTime*1.1))) - 1.6;

	a = smin(a,b,0.4);
	a = smin(a,c,0.4);
	a = smin(a,d,0.4);
	a = smin(a,e,0.4);

	rot(p.xy,vec2(sin(iGlobalTime*2.0),cos(iGlobalTime*2.0)));

	a = mix(sdBox(p,vec3(2.0)),a,clamp(iGlobalTime*0.4,0.0,1.0));

	return a;
}


float dicelogo( in vec3 p )
{
	float a=10000.0;
	p.x+=1.5; 
	//d
	a=min(a,sdBox(p+vec3(1.0,1.0,0.0),vec3(1.0,0.1,0.1)));
	a=min(a,sdBox(p+vec3(1.0,-1.0,0.0),vec3(1.0,0.1,0.1)));
	a=min(a,sdBox(p+vec3(0.0,0.0,0.0),vec3(0.1,1.1,0.1)));
	//i
	p.x-=0.5;
	a=min(a,sdBox(p+vec3(0.0,0.0,0.0),vec3(0.1,1.1,0.1)));
	//c
	p.x-=2.5;
	a=min(a,sdBox(p+vec3(1.0,1.0,0.0),vec3(1.0,0.1,0.1)));
	a=min(a,sdBox(p+vec3(1.0,-1.0,0.0),vec3(1.0,0.1,0.1)));
	a=min(a,sdBox(p+vec3(2.0,0.0,0.0),vec3(0.1,1.1,0.1)));
	p.x-=2.4;
	a=min(a,sdBox(p+vec3(1.0,1.0,0.0),vec3(1.0,0.1,0.1)));
	a=min(a,sdBox(p+vec3(1.0,0.0,0.0),vec3(1.0,0.1,0.1)));
	a=min(a,sdBox(p+vec3(1.0,-1.0,0.0),vec3(1.0,0.1,0.1)));
	return a;
}

float map( in vec3 p )
{
	//p.z += (-5.0) + iGlobalTime*0.5;
	vec3 q = p;
	float sc = cos(iGlobalTime)*100.0;
	sc = min(8.0,sc);
	sc = mix(200.0,sc,1.0-iFade);
	
	//q = vec3(ivec3(q*sc)/sc);
	return mix(obj(q),dicelogo(q),iFade);
}

vec3 getnormal( in vec3 pos )
{
	vec3 eps = vec3(0.5,0.0,0.0);
	vec3 nor;
	nor.x = map(pos+eps.xyy) - map(pos-eps.xyy);
	nor.y = map(pos+eps.yxy) - map(pos-eps.yxy);
	nor.z = map(pos+eps.yyx) - map(pos-eps.yyx);
	return normalize(nor);
}

vec4 march(vec3 origin, vec3 dir)
{
	float dist = 0.0;
	vec4 sum = vec4(0.0);
	vec3 sundir = vec3(-1.0,0.0,0.0);
	for(int i=0;i<STEPS;i++)
	{
		vec3 pos = origin + dir*dist;
		float d = map(pos);
		dist += mix(mix(0.2,d,0.25*sin(iGlobalTime*0.25)+0.75),d,iFade);
		//dist += d;
		//dist += mix(0.2,d,0.5*sin(iGlobalTime*0.25)+0.5);
		if(abs(d) < 0.01)
		{
			sum.rgb += map(pos+dir*0.6) - d;
			sum.rgb += map(pos-dir*0.6) - d;
			sum.a = clamp(sum.r,0.0,1.0);
			sum.rgb = getnormal(pos);
			// raycast shadows
			float m=10000.0;
			vec3 tosun = normalize(vec3(50.0,100.0,0.0) - pos);
			for(int j=1;j<10;j++)
			{
				m=min(m,map(pos+tosun*float(j)*0.9));
				if(m>0.001) break;
			}
			sum.a = min(sum.a,m);
			break;
		}

		/*	
		float diff = d - map(pos+sundir*0.4);
		diff = clamp(diff/0.6,0.0,1.0);
		vec3 lin = vec3(0.79,0.75,0.9)*1.35 + 0.45*vec3(1.5, 0.6, 0.7)*diff;
		sum.a *= lin.r;
		d *= 0.9;
		sum.a *= d;
		sum.a = sum.a + d;
		*/

	}

	return sum;
}

void main()
{
	spos = gl_FragCoord.xy/iResolution.xy*2.0-1.0;

	iFade = smoothstep(19.2,20.2,iGlobalTime) + smoothstep(25.0,30.0, iGlobalTime);

	vec2 p = spos;
	vec2 uv = 0.5*spos+0.5;
	uv.y = 1.0 - uv.y;
	p.x *= iResolution.x/iResolution.y;

	p.y += snoise(iResolution*vec2(iGlobalTime*40.0,0.0)) * max(0.0,iFade) * snoise(iResolution*vec2(iGlobalTime*10.0,0.0)) * 0.05 * clamp(1.0-iFade,0.0,1.0);

	vec3 pos = vec3(0.0,0.0,-5.0);
	vec3 dir = normalize(vec3(p,1.0));

	pos = vec3(p*5.0, -5.0);
	dir = vec3(0.0,0.0,1.0);

	vec4 res = march(pos,dir);
	vec3 sundir = normalize(vec3(0.1,0.1,-1.0));
	color = vec3(res.a)+vec3(0.2,0.5,1.0)*max(0.0,dot(sundir,res.rgb));
	//vec3 invcolor = 1.0 - color;
	//color = mix(invcolor,color,iFade);

	color += snoise(vec2(iGlobalTime*40.0,0.0)) * max(0.0,iFade-0.9)  * clamp(1.0-iFade,0.0,1.0);
	color += snoise(iResolution*0.5*spos+vec2(iGlobalTime*4320.0,0.0)) * max(0.0,iFade-0.8)  * clamp(1.0-iFade,0.0,1.0);


	color *= 1.0-length(spos);
	color += vec3(0.45);
	color *= color*color*color*color;
	//color *= 2.4;

	color = clamp(color,vec3(0.0),vec3(1.0));




	color=mix(vec3(0.0),color,clamp(iGlobalTime,0.0,1.0));
	color=mix(color,vec3(0.0),max(0.0,(iFade-1.0)));
}
