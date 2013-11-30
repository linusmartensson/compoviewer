
#pragma comment(lib, "OpenGL32.lib")
#pragma comment(lib, "deps/bass.lib")
#pragma comment(lib, "deps/libvlc.lib")
#pragma comment(lib, "deps/libvlccore.lib")
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
#include"renderer.h"
#include"JSON.h"
#include"transitions.h"

#include"renderers\imagerenderer.h"
#include"renderers\videorenderer.h"
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
				idata[i]=idata[i]*0.97f>data[i]?idata[i]*0.97f:data[i];
				sdata[i]=(sdata[i]+data[i]);
			}
			
			audiotex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, idata);
			
			audiostex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, sdata);
			program::getuniform("iChannel0")->set(audiotex, 1);
			program::getuniform("iChannel1")->set(audiostex, 2);
		}

		glViewport(0,0, width, height);
		glClearColor(1.0f,102/255.f,0.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		
		program::getuniform("iResolution")->set((float)width,(float)height);
		program::getuniform("iGlobalTime")->set((float)localtime);
		fss->run();
		program::getuniform("tex")->set(texttexture, 0);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		dx = sx = (float)width/10.f; 
		dy = sy = (float)height;
		program::getuniform("time")->set((float)localtime);
		
		program::uniforms["resolution"]->set((float)width, (float)height);
		c->stash->drawfunc = [&](int v){
			p->update();
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
		
		program::uniforms["resolution"]->set((float)width, (float)height);
		c->stash->drawfunc = [&](int v){
			p->update();
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
	core *c = new glfwcore;
	
	renderer_end *e = 0;
	JSONValue *efile = JSON::Parse(core::getfile("entries.json").c_str());
	auto compos= efile->AsArray();
	transitionrenderer *r = 0;

	double totallength = 0.0;
	std::string previous;
	std::for_each(compos.begin(), compos.end(), [&](JSONValue *compo){
		previous = "";
		auto rs = new renderer_start;
		rs->audiotrack = "resources/kreativ_jingel.wav";
		rs->delay = 0.5f;
		
		int category = (int)compo->Child(L"category")->AsNumber();
		
		std::string sponsor_file = wctos(compo, L"sponsor_file");
		if(sponsor_file != "none") sponsor_file = "resources/"+sponsor_file;
		else sponsor_file = "";
		auto entries = compo->Child(L"entries")->AsArray();
		std::string componame = wctos(compo, L"compo");

		std::cout<<"Loading compo "<<componame<<" with category"<<category<<": "<<std::endl;

		rs->title = componame;

		if(e == 0)
			core::current = rs;
		else {
			e->setup(c, rs);
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
		
			pr->setup(c, (r = ir));
		
			if(pr->audio)
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
				br->delay = 1.0;
				ir->setup(c, (r = br));
				ir = new itemrenderer;
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
				ir->delay = 2.0;
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

		totallength += compolength;
	});	


	r->setup(c,e->setup(c, (new renderer_shutdown)->setup(c,0)));
	
	e->endtimehint = 1000000.0;

	std::cout<<"Minutes:"<<totallength/60.f<<std::endl;

	c->run();
}