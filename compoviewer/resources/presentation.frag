#version 420

uniform float iGlobalTime;
uniform vec2 iResolution;


vec3 COLOR1 = vec3(0.0, 0.0, 0.3);
vec3 COLOR2 = vec3(0.5, 0.0, 0.0);
float BLOCK_WIDTH = 0.01;

out vec4 c;

void main(void)
{
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	
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

	
	c = vec4(final_color, 1.0)*0.9-uv.x*uv.x;
	c -= 1.0-clamp(1.0-pow(distance(uvf.x,0.0),16.0), 0.0,1.0);
	c *= clamp(distance(uvf.x*uvf.x,1.0),0.0,1.0);
	c = max(c, vec4(0.0))*1.0;
	c = c/(c+1.0)+0.075;
	c.a = 1.0;
}