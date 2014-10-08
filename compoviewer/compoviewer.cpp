/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "deps/bass.lib")
#pragma comment(lib, "deps/BASSMOD.lib")
#pragma comment(lib, "deps/libvlc.lib")
#pragma comment(lib, "deps/libvlccore.lib")
#pragma comment(lib, "deps/libxmp.lib")
#define _CRT_SECURE_NO_WARNINGS
#ifndef NDEBUG
#pragma comment(lib, "deps/Debug/glfw3.lib")
#pragma comment(lib, "deps/Debug/glew32sd.lib")
#else
#pragma comment(lib, "deps/Release/glfw3.lib")
#pragma comment(lib, "deps/Release/glew32s.lib")
#endif

#define GLEW_STATIC
#include<GL/glew.h>

#include<iostream>
#include<vector>
#include<string>
#include<sstream>
#include<algorithm>

#include"shaders.h"
#include"glfwcore.h"
#include"imemcore.h"
#include"renderer.h"
#include"JSON.h"
#include"transitions.h"

#include"renderers\imagerenderer.h"
#include"renderers\videorenderer.h"
#include"renderers\itemrenderer.h"

struct renderer_startup : public transitionrenderer{
	texture *texttexture;
	program *p;
	buffer *buf;
	varray *arr;
	float sx, sy, dx, dy, lh;
	fsshader *fss, *fss2;
	framebuffer *fb;
	
	texture *audiotex, *audiostex;

	texture *fstexture, *bg0tex, *bg1tex;

	std::string title;
	float idata[512];
	float sdata[512];
	float ldata[2];
	float lsdata[2];

	program* subinit(){
		std::vector<GLuint> shaders;
		bg0tex = texture::load("resources/middle_overlay.png");
		bg1tex = texture::load("resources/right_overlay.png");
		audiotex = new texture;
		audiostex = new texture;
		fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("metaballs.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));
		shaders.clear();
		fss2 = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("distshader.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));

		fb = new framebuffer;

		fstexture = new texture;
		fstexture->bind(-1);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


		texttexture = new texture(c->stash->tex);
		shaders.clear();
		p = program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("item_text.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("item_text.vert"), shaders)));
		buf = new buffer;
		arr = new varray;
		buf->set(VERT_COUNT*VERT_STRIDE, c->stash->verts, GL_DYNAMIC_DRAW); 
		arr->getconfig("text_position")->set(buf, 0, 0);
		shaders.clear();
		return program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("transition_test.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders)));
	}
	int run(int width, int height, double localtime, bool first){
		
		if(fstexture->w != width || fstexture->h != height){		
			fstexture->set(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, width, height);
		}


		if(audio){
			
			if(first) {
				memset(idata, 0, sizeof(idata));
				memset(sdata, 0, sizeof(sdata));
						
				ldata[0] = ldata[1] = lsdata[0] = lsdata[1] = 0.f;
			}
			float data[512];
			BASS_ChannelGetData(audio, data, BASS_DATA_FFT1024);
			
			for(int i=0;i<512;++i){
				idata[i]=idata[i]*0.97f>data[i]?idata[i]*0.97f:data[i];
				sdata[i]=(sdata[i]+data[i]);
			}
			
			audiotex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, idata);
			
			audiostex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, sdata);
			program::getuniform("iChannel0")->set(audiotex, 1);
			program::getuniform("iChannel1")->set(audiostex, 2);
			
			auto level = BASS_ChannelGetLevel(audio);
			if(level != -1){
				ldata[0] = ldata[0]*0.97f>LOWORD(level)/32768.f?ldata[0]*0.97f:LOWORD(level)/32768.f;
				ldata[1] = ldata[1]*0.97f>HIWORD(level)/32768.f?ldata[1]*0.97f:HIWORD(level)/32768.f;
				lsdata[0] += ldata[0]*ldata[0];
				lsdata[1] += ldata[1]*ldata[1];

				program::getuniform("audiolevel")->set(ldata[0],ldata[1],lsdata[0],lsdata[1]);
			}
		}

		
		program::getuniform("tex")->set(texttexture, 0);
		dx = sx = (float)width/30.f; 
		dy = sy = (float)height;
		
		program::getuniform("resolution")->set((float)width, (float)height);
		c->stash->drawfunc = [&](int v){
			p->update();
			buf->subset(0, c->stash->nverts*VERT_STRIDE, c->stash->verts);
			arr->update();
			glDrawArrays(GL_TRIANGLES, 0, c->stash->nverts);
		};





		glViewport(0,0, width, height);
		glClearColor(0.f,0.f,0.0f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		program::getuniform("background0")->set(bg0tex, 3);
		program::getuniform("background1")->set(bg1tex, 4);
		fb->set(0, fstexture);
		program::getuniform("iResolution")->set((float)width,(float)height);
		program::getuniform("iGlobalTime")->set((float)localtime);
		framebuffer::enter(fb);
		
		fss->run();
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		
		sth_begin_draw(c->stash);

		dx = width*0.35;
		dy = height*0.45;

		program::getuniform("color")->set(1.0f,1.0f,1.0f,(float)((localtime-12.0f)*(19.0f-localtime)));
		sth_draw_text(c->stash, 2,200*width/1920.0, dx,dy,"KREATIV",&dx);
		
		sth_end_draw(c->stash);
		sth_begin_draw(c->stash);

		program::getuniform("color")->set(1.0f,1.0f,1.0f,(float)((localtime-19.0f)*(23.5f-localtime)));
		dx = width*0.38;
		dy = height*0.7;
		sth_draw_text(c->stash, 2,100*width/1920.0, dx,dy,"Sponsored by",&dx);
		sth_end_draw(c->stash);

		glDisable(GL_BLEND);
		
		
		
		framebuffer::exit();
		
		fss2->run();
		
		
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		
		sth_begin_draw(c->stash);
		
		dy = height*0.85f;
		dx = width*0.78f;

		program::getuniform("color")->set(1.0f,1.0f,1.0f,(float)((localtime-4.f)*0.1*(18.0f-localtime)));
		std::istringstream iss(core::getfile("entries.json")+"\n"+core::getfile("audiobeamer.frag")+"\n"+core::getfile("presentation2.frag"));
		sth_vmetrics(c->stash, 2, 14*width/1920.0, 0,0,&lh);
		std::string text;
		int ppp = 0;
		int kkk = 0;
		while(std::getline(iss, text) && kkk < 65.0){
			if(!iss) break;
			if(++ppp < lsdata[0]*1.0) continue;
			text = text.substr(0, 60);
			sth_draw_text(c->stash, 2,14*width/1920.0, dx,dy,text.c_str(),&dx);
			dy -= lh;
			dx = width*0.78f;
			kkk++;
		}
		sth_end_draw(c->stash);

		
		glDisable(GL_BLEND);

		program::getuniform("fstexture")->set(fstexture, 0);

		

		return localtime>endtimehint?1:0;
	}
};

struct renderer_start : public transitionrenderer{
	texture *texttexture;
	program *p;
	buffer *buf;
	varray *arr;
	float sx, sy, dx, dy, lh;
	fsshader *fss;
	
	texture *audiotex, *audiostex;

	std::string title;
	float idata[512];
	
	float sdata[512];

	program* subinit(){
		std::vector<GLuint> shaders;
		audiotex = new texture;
		audiostex = new texture;
		fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("presentation2.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));

		texttexture = new texture(c->stash->tex);
		shaders.clear();
		p = program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("item_text.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("item_text.vert"), shaders)));
		buf = new buffer;
		arr = new varray;
		buf->set(VERT_COUNT*VERT_STRIDE, c->stash->verts, GL_DYNAMIC_DRAW); 
		arr->getconfig("text_position")->set(buf, 0, 0);
		shaders.clear();
		return program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("transition_test.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders)));
	}
	int run(int width, int height, double localtime, bool first){
				
		if(audio){
			
			if(first) {
				memset(idata, 0, sizeof(idata));
				memset(sdata, 0, sizeof(sdata));
			}
			float data[512];
			BASS_ChannelGetData(audio, data, BASS_DATA_FFT1024);
			
			for(int i=0;i<512;++i){
				idata[i]=idata[i]*0.97f>data[i]?idata[i]*0.97f:data[i];
				sdata[i]=(sdata[i]+data[i]);
			}
			
			audiotex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, idata);
			
			audiostex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, sdata);
			program::getuniform("iChannel0")->set(audiotex, 1);
			program::getuniform("iChannel1")->set(audiostex, 2);
		}

		glViewport(0,0, width, height);
		glClearColor(0.f,0.f,0.0f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		program::getuniform("iResolution")->set((float)width,(float)height);
		program::getuniform("iGlobalTime")->set((float)localtime);
		fss->run();
		program::getuniform("tex")->set(texttexture, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		dx = sx = (float)width/30.f; 
		dy = sy = (float)height;
		program::getuniform("time")->set((float)localtime);
		
		program::getuniform("resolution")->set((float)width, (float)height);
		c->stash->drawfunc = [&](int v){
			p->update();
			buf->subset(0, c->stash->nverts*VERT_STRIDE, c->stash->verts);
			arr->update();
			glDrawArrays(GL_TRIANGLES, 0, c->stash->nverts);
		};
		program::getuniform("color")->set(0.f,0.f,0.0f,(float)localtime-1.f);
		sth_begin_draw(c->stash);
		dy = height*0.6f;
		sth_draw_text(c->stash, 2,150*width/1920.0, dx,dy,"Next compo:",&dx);
		sth_vmetrics(c->stash, 2, 200*width/1920.0, NULL, NULL, &lh);
		dx = sx+100;
		dy -= lh;
		sth_end_draw(c->stash);
		
		program::getuniform("color")->set(0.0f,0.f,0.0f,(float)localtime-3.0f);
		sth_begin_draw(c->stash);
		sth_draw_text(c->stash, 2,200*width/1920.0, dx,dy,title.c_str(),&dx);
		sth_end_draw(c->stash);

		return localtime>endtimehint?1:0;
	}
};
struct renderer_end : public transitionrenderer {
	texture *texttexture;
	program *p;
	buffer *buf;
	varray *arr;
	float sx, sy, dx, dy, lh;
	std::string title;
	fsshader *fss;

	program* subinit(){
		endtimehint = 5.0;
		std::vector<GLuint> shaders;
		fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("presentation2.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));
		shaders.clear();
		
		texttexture = new texture(c->stash->tex);
		
		shaders.clear();
		p = program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("item_text.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("item_text.vert"), shaders)));
		buf = new buffer;
		arr = new varray;
		buf->set(VERT_COUNT*VERT_STRIDE, c->stash->verts, GL_DYNAMIC_DRAW); 
		arr->getconfig("text_position")->set(buf, 0, 0);
		

		shaders.clear();
		return program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("transition_test.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders)));
	}
	int run(int width, int height, double localtime, bool first){
		glViewport(0,0, width, height);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		program::getuniform("iResolution")->set((float)width,(float)height);
		program::getuniform("iGlobalTime")->set((float)localtime);
		fss->run();

		program::getuniform("tex")->set(texttexture, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		dx = sx = (float)width/30.f; 
		dy = sy = (float)height;
		program::getuniform("time")->set((float)localtime);
		
		program::getuniform("resolution")->set((float)width, (float)height);
		c->stash->drawfunc = [&](int v){
			p->update();
			buf->subset(0, c->stash->nverts*VERT_STRIDE, c->stash->verts);
			arr->update();
			glDrawArrays(GL_TRIANGLES, 0, c->stash->nverts);
		};
		program::getuniform("color")->set(0.f,0.f,0.0f,4.f);
		
		sth_begin_draw(c->stash);
		dy = height*0.6f;
		
		std::ostringstream oss;
		oss<<"End of "<<title;

		sth_draw_text(c->stash, 2,100*width/1920.0, dx,dy,"End of",&dx);
		sth_vmetrics(c->stash, 2, 200*width/1920.0, NULL, NULL, &lh);
		dx = sx+100*width/1920.0;
		dy -= lh;
		sth_end_draw(c->stash);
		
		program::getuniform("color")->set(0.f,0.f,0.0f,(float)localtime);
		sth_begin_draw(c->stash);
		sth_draw_text(c->stash, 2,200*width/1920.0, dx,dy,title.c_str(),&dx);
		sth_vmetrics(c->stash, 2, 200*width/1920.0, NULL, NULL, &lh);
		dy -= 1.5*lh;
		dx = sx+800*width/1920.0;
		sth_draw_text(c->stash, 2,50*width/1920.0, dx,dy,"Don't forget to vote at kreativ.dreamhack.se!",&dx);
		sth_end_draw(c->stash);

		return localtime>endtimehint?1:0;
	}
};
class renderer_black : public transitionrenderer {
	program* subinit(){
		std::vector<GLuint> shaders;
		dotransition = false;
		return program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("transition_test.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders)));
	}
	int run(int width, int height, double localtime, bool first){
		glViewport(0,0, width, height);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		return localtime>1?1:0;
	}
};

static char tmp[8192];
static std::string wctos(JSONValue *v, const wchar_t *str){
	std::wcstombs(tmp, v->Child(str)->AsString().c_str(), 8192);
	return tmp;
}

int main(int argc, char* argv[]){

	core *tc = new tempcore;

	core *c = 0;
	
	if(tc->requestedPlayer == "imem")
		c = new imemcore;
	else if(tc->requestedPlayer == "glfw")
		c = new glfwcore;

	if(!c) core::die();

	c->init();

	std::string entryfile = "entries.json";

	if(argc > 1){
		entryfile = argv[1];
	}


	renderer_end *e = 0;
	JSONValue *efile = JSON::Parse(core::getfile(entryfile).c_str());
	auto compos= efile->AsArray();
	transitionrenderer *r = 0;
	double totallength = 0.0;
	renderer_startup *startr = 0;
	std::string previous;
	std::for_each(compos.begin(), compos.end(), [&](JSONValue *compo){
		previous = "";
		
		int category = (int)compo->Child(L"category")->AsNumber();
		
		if(category == 255){
			
			startr = new renderer_startup;
			startr->endtimehint = 25.0;
			startr->audiotrack = "resources/creative_new.wav";
			startr->delay = 0.0f;
			if(e != 0){
				e->setup(c, startr);
				e = 0;
			}
			return;
		}

		auto rs = new renderer_start;
		
		rs->audiotrack = "";
		if(rs->audiotrack == "") rs->endtimehint = 10.0; else rs->endtimehint = 500.0;
		rs->delay = 2.0f;
		
		
		std::string sponsor_file = wctos(compo, L"sponsor_file");
		if(sponsor_file != "none") sponsor_file = "resources/"+sponsor_file;
		else sponsor_file = "";
		auto entries = compo->Child(L"entries")->AsArray();
		std::string componame = wctos(compo, L"compo");

		std::cout<<"Loading compo "<<componame<<" with category"<<category<<": "<<std::endl;

		rs->title = componame;

		if(e != 0){
			e->setup(c, rs);
			e = 0;
		}else if(startr != 0){
			startr->setup(c, rs);
			if(core::current == 0)  core::current = startr;
			startr = 0;
		}else if(core::current == 0){
			core::current = rs;
		}
		
		r = rs;
		int pos = 0;
	
		double compolength = 0.0;
		
		itemrenderer *ir = 0;
		

		for_each(entries.begin(), entries.end(), [&](JSONValue *v){
			pos++;
			transitionrenderer *pr = r;
			ir = new itemrenderer;
			ir->artist = wctos(v, L"artist");
			ir->group = wctos(v, L"group");
			ir->pos = pos;
			ir->title = wctos(v, L"title");
			ir->description = wctos(v, L"description");
			ir->previous = previous;
			ir->sponsor_file = sponsor_file;
			ir->category = category;
		
			pr->setup(c, (r = ir));
		
			if(pr->audio || pr->tracked)
				compolength+=pr->audiolength+pr->delay*2.0;
			else{
				compolength+=pr->endtimehint;
			}

			if(category == core::CATEGORY_GFX_HIRES){

				//Render photo/hires
				auto br = new imagerenderer;
				br->interpolation = GL_LINEAR_MIPMAP_LINEAR;
				br->filename = "submissions/"+wctos(v, L"filename");
				ir->setup(c, (r = br));

				ir = new itemrenderer;
				ir->category = category;
				ir->pos = pos;
				ir->artist = wctos(v, L"artist");
				ir->group = wctos(v, L"group");
				ir->title = wctos(v, L"title");
				ir->description = wctos(v, L"description");
				ir->previous = previous;
				ir->sponsor_file = sponsor_file;

				br->setup(c, (r = ir));
				compolength+=br->endtimehint;
			} else if(category == core::CATEGORY_GFX_OLDSCHOOL){
			
				//Render oldschool
				auto br = new imagerenderer;
				br->interpolation = GL_NEAREST;
				br->filename = "submissions/"+wctos(v, L"filename");
				ir->setup(c, (r = br));
			
				ir = new itemrenderer;
				ir->category = category;
				ir->pos = pos;
				ir->artist = wctos(v, L"artist");
				ir->group = wctos(v, L"group");
				ir->title = wctos(v, L"title");
				ir->description = wctos(v, L"description");
				ir->previous = previous;
				ir->sponsor_file = sponsor_file;

				br->setup(c, (r = ir));
				compolength+=br->endtimehint;
			} else if(category == core::CATEGORY_VIDEO){

				//Render wild
				auto br = new videorenderer;
				
				br->filename = "submissions/"+wctos(v, L"filename");
				if(br->filename.find("://") != std::string::npos){
					br->filename = wctos(v, L"filename");
					br->endtimehint = 100.0;
				}
				br->delay = 5.0;
				ir->setup(c, (r = br));
				ir = new itemrenderer;
				ir->category = category;
				ir->pos = pos;
				ir->artist = wctos(v, L"artist");
				ir->group = wctos(v, L"group");
				ir->title = wctos(v, L"title");
				ir->description = wctos(v, L"description");
				
				ir->sponsor_file = sponsor_file;
				ir->previous = previous;

				br->setup(c, (r = ir));
				compolength+=br->endtimehint;
			} else if(category == core::CATEGORY_AUDIO){

				//Render audio
				ir->audiotrack = "submissions/"+wctos(v, L"filename");

				ir->tracked = false;
				
				ir->delay = 4.0;
			} else if(category == core::CATEGORY_TRACKED_AUDIO){

				//Render audio
				ir->audiotrack = "submissions/"+wctos(v, L"filename");
				ir->tracked = true;
				ir->delay = 4.0;
			}
			
			std::ostringstream oss;
			oss<<"Previous entry: #"<<(pos)<<" "<<ir->artist<<"^"<<ir->group<<" - "<<ir->title;

			previous = oss.str();

		});
		e = (new renderer_end);
		
		
		ir->setup(c, e);
		
		if(ir->audio)
			totallength+=r->audiolength+r->delay*2.0;
		std::cout<<rs->title<<" compo length: "<<compolength/60.f<<std::endl;
		
		e->title = wctos(compo, L"compo");
		e->audiotrack = "";
		e->delay = 2.0;
		e->endtimehint = 10.0;

		totallength += compolength;
	});	


	r->setup(c,e->setup(c, (new renderer_shutdown)->setup(c,0)));
	
	e->endtimehint = 20.0;

	std::cout<<"Minutes:"<<totallength/60.f<<std::endl;

	c->run();
}