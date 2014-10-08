#version 400

uniform vec2 iResolution;
uniform float iGlobalTime;

uniform sampler2D iChannel0;
uniform sampler2D iChannel1;		
uniform sampler2D background0;
uniform sampler2D background1;
uniform sampler2D fstexture;
uniform vec2 logores;
uniform vec4 audiolevel;

uniform int category;

out vec4 c;

void main(void)
{
    
					
	vec3 final = vec3(0.0);

	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	
	vec2 p = 2.0*uv-1.0;
	p.x *= iResolution.x/iResolution.y;
	
	float v = audiolevel.x;

	v*=v*v*v;

	v *= smoothstep(19.0,20.0, iGlobalTime)*1.5+0.25;
	v *= smoothstep(22.5,21.5,iGlobalTime);

	final = vec3(
		texture2D(fstexture, uv+vec2(0.0,sin(iGlobalTime*3250.0*uv.y*32.0)*0.1*v).yx).r,
		texture2D(fstexture, uv+vec2(0.0,sin(iGlobalTime*3230.0*uv.y*32.0)*0.1*v).yx).g,
		texture2D(fstexture, uv+vec2(0.0,sin(iGlobalTime*3260.0*uv.y*32.0)*0.1*v).yx).b);

	final = mix(final, length(final)>0.04?vec3(1.0):vec3(texture2D(fstexture, vec2(0.0))), smoothstep(21.2,21.9, iGlobalTime)*step(p.y,0.24));
	
				  
	// overlay
	float overlay = 0.0;

	uv.y *= -1.0;
	vec4 overlay_color = texture(background0,uv);
	vec4 textoverlay_color = texture(background1,uv);

	float flicker = textoverlay_color.a * smoothstep(0.0,1.0,sin((iGlobalTime*100.0)));
	textoverlay_color.a = mix(flicker,textoverlay_color.a,smoothstep(3.0,4.5,iGlobalTime)) * smoothstep(3.0,4.0,iGlobalTime);
	textoverlay_color.a *= 1.0 - smoothstep(18.0,19.0,iGlobalTime);

	overlay = overlay_color.a + textoverlay_color.a;
	overlay *= smoothstep(20.0,19.0,iGlobalTime);

	final=mix(final,vec3(1.0),overlay);

	c = vec4(1.0-final.bgr,1.0);
	
}