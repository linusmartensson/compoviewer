
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "deps/bass.lib")
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
#include"core.h"
#include"renderer.h"
#include"JSON.h"
#include"transitions.h"

#include"renderers\imagerenderer.h"
#include"renderers\itemrenderer.h"

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
				idata[i]=idata[i]*0.97>data[i]?idata[i]*0.97:data[i];
				sdata[i]=(sdata[i]+data[i]);
			}
			
			audiotex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, idata);
			
			audiostex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, sdata);
			audiotex->bind(1, "iChannel0");
			
			audiostex->bind(2, "iChannel1");
			
			
		}

		glViewport(0,0, width, height);
		glClearColor(1.0f,102/255.f,0.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		program::getuniform("iResolution")->set((float)width,(float)height);
		program::getuniform("iGlobalTime")->set((float)localtime);
		fss->run();
		
		texttexture->bind(0, "tex");
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		dx = sx = (float)width/10.f; 
		dy = sy = (float)height;
		program::getuniform("time")->set((float)localtime);
		
		program::uniforms["resolution"]->set((float)width, (float)height);
		c->stash->drawfunc = [&](int v){
			p->update();
			texttexture->bind(0, "tex");
			buf->subset(0, c->stash->nverts*VERT_STRIDE, c->stash->verts);
			arr->update();
			glDrawArrays(GL_TRIANGLES, 0, c->stash->nverts);
		};
		program::uniforms["color"]->set(1.f,102/255.f,0.f,(float)localtime-1.f);
		sth_begin_draw(c->stash);
		sth_vmetrics(c->stash, 3, 50, NULL, NULL, &lh);
		dy = height*0.7f+lh*1.f;
		sth_draw_text(c->stash, 3,50, dx,dy,"Next compo:",&dx);
		sth_vmetrics(c->stash, 3, 100, NULL, NULL, &lh);
		dx = sx;
		dy = height*0.7f-lh*0.5f;
		sth_end_draw(c->stash);
		
		program::getuniform("color")->set(1.f,102/255.f,0.f,(float)localtime-3.0f);
		sth_begin_draw(c->stash);
		sth_draw_text(c->stash, 3,100, dx,dy,title.c_str(),&dx);
		sth_end_draw(c->stash);

		return localtime>500.0?1:0;
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

		texttexture->bind(0, "tex");
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		dx = sx = (float)width/30.f; 
		dy = sy = (float)height;
		program::getuniform("time")->set((float)localtime);
		
		program::uniforms["resolution"]->set((float)width, (float)height);
		c->stash->drawfunc = [&](int v){
			p->update();
			texttexture->bind(0, "tex");
			buf->subset(0, c->stash->nverts*VERT_STRIDE, c->stash->verts);
			arr->update();
			glDrawArrays(GL_TRIANGLES, 0, c->stash->nverts);
		};
		program::uniforms["color"]->set(1.f,102/255.f,0.f,4.f-(float)localtime);
		sth_begin_draw(c->stash);
		sth_vmetrics(c->stash, 3, 100, NULL, NULL, &lh);
		dy = height*0.5f-lh*0.5f;
		sth_draw_text(c->stash, 3,100, dx,dy,(std::string("End of ")+title).c_str(),&dx);
		sth_vmetrics(c->stash, 3, 50, NULL, NULL, &lh);
		sth_end_draw(c->stash);
		program::uniforms["color"]->set(1.f,102/255.f,0.f,4.0f-(float)localtime);
		sth_begin_draw(c->stash);
		dy -= lh*1.0f;
		dx = sx;
		sth_draw_text(c->stash, 3,50, dx,dy,std::string("Don't forget to vote!").c_str(),&dx);
		dy -= lh*1.0f;
		dx = sx;
		sth_draw_text(c->stash, 3,50, dx,dy,std::string("http://kreativ.dreamhack.se").c_str(),&dx);
		sth_end_draw(c->stash);

		return localtime>5.0?1:0;
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
	core *c = new core;
	auto rs = new renderer_start;
	rs->audiotrack = "resources/kreativ_jingel.wav";
	rs->delay = 0.5f;
	rs->title = c->title;
	core::current = rs;
	JSONValue *efile = JSON::Parse(core::getfile("entries.json").c_str());
	auto entries = efile->AsArray();
	transitionrenderer *r = rs;
	int pos = 0;
	
	double totallength = 0.0;


	std::string previous ="";
	for_each(entries.begin(), entries.end(), [&](JSONValue *v){
		pos++;
		transitionrenderer *pr = r;
		auto ir = new itemrenderer;
		ir->artist = wctos(v, L"artist");
		ir->group = wctos(v, L"group");
		ir->pos = pos;
		ir->title = wctos(v, L"title");
		ir->description = wctos(v, L"description");
		ir->previous = previous;
		
		pr->setup(c, (r = ir));
		
		if(pr->audio)
			totallength+=pr->audiolength+pr->delay*2.0;

		if(c->category == 0){

			//Render photo/hires
			auto br = new imagerenderer;
			br->interpolation = GL_LINEAR_MIPMAP_LINEAR;
			br->filename = wctos(v, L"filename");
			ir->setup(c, (r = br));
			
			ir = new itemrenderer;
			ir->pos = pos;
			ir->artist = wctos(v, L"artist");
			ir->group = wctos(v, L"group");
			ir->title = wctos(v, L"title");
			ir->description = wctos(v, L"description");
					ir->previous = previous;

			br->setup(c, (r = ir));
		} else if(c->category == 1){
			
			//Render oldschool
			auto br = new imagerenderer;
			br->interpolation = GL_NEAREST;
			br->filename = wctos(v, L"filename");
			ir->setup(c, (r = br));
			
			ir = new itemrenderer;
			ir->pos = pos;
			ir->artist = wctos(v, L"artist");
			ir->group = wctos(v, L"group");
			ir->title = wctos(v, L"title");
			ir->description = wctos(v, L"description");
					ir->previous = previous;

			br->setup(c, (r = ir));
		} else if(c->category == 2){

			//Render wild
			auto br = new renderer_black;
			ir->setup(c, (r = br));
			ir = new itemrenderer;
			ir->dotransition = false;
			ir->pos = pos;
			ir->artist = wctos(v, L"artist");
			ir->group = wctos(v, L"group");
			ir->title = wctos(v, L"title");
			ir->description = wctos(v, L"description");
					ir->previous = previous;

			br->setup(c, (r = ir));
		} else if(c->category == 3){

			//Render audio
			ir->audiotrack = wctos(v, L"filename");
			ir->delay = 2.0;
		}
		std::ostringstream oss;
		oss<<"Previous entry: #"<<(pos)<<" "<<ir->artist<<"^"<<ir->group<<" - "<<ir->title;

		previous = oss.str();

	});
	auto e = (new renderer_end);
	e->title = c->title;
	

	r->setup(c,e->setup(c, (new renderer_shutdown)->setup(c,0)));
	if(r->audio)
		totallength+=r->audiolength+r->delay*2.0;

	std::cout<<"Minutes:"<<totallength/60.f<<std::endl;

	c->run();
}