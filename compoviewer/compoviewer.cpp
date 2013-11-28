
#pragma comment(lib, "OpenGL32.lib")
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

struct renderer_start : public renderer {
	texture *texttexture;
	program *p;
	buffer *buf;
	varray *arr;
	float sx, sy, dx, dy, lh;
	fsshader *fss;
	
	std::string title;

	void init(){
		std::vector<GLuint> shaders;
		fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("presentation.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));

		texttexture = new texture(c->stash->tex);
		shaders.clear();
		p = program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("item_text.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("item_text.vert"), shaders)));
		buf = new buffer;
		arr = new varray;
		buf->set(VERT_COUNT*VERT_STRIDE, c->stash->verts, GL_DYNAMIC_DRAW); 
		arr->getconfig("text_position")->set(buf, 0, 0);
		
	}
	int operator()(renderer *pr, int width, int height, double localtime, double prevtime){
		glViewport(0,0, width, height);
		glClearColor(1.0f,102/255.f,0.f,1.f);
		glClear(GL_COLOR_BUFFER_BIT);

		texttexture->bind(0, "tex");
		
		program::getuniform("iResolution")->set((float)width,(float)height);
		program::getuniform("iGlobalTime")->set((float)localtime);
		fss->run();

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
		program::uniforms["color"]->set(1.f,102/255.f,0.f,1.f);
		sth_begin_draw(c->stash);
		sth_vmetrics(c->stash, 2, 100, NULL, NULL, &lh);
		dy -= lh*1.5f;
		sth_draw_text(c->stash, 2,100, dx,dy,title.c_str(),&dx);
		sth_end_draw(c->stash);
		return localtime>5.0?1:0;
	}
};
class renderer_end : public transitionrenderer {
	texture *texttexture;
	program *p;
	buffer *buf;
	varray *arr;
	float sx, sy, dx, dy, lh;

	fsshader *fss;

	program* subinit(){
		
		std::vector<GLuint> shaders;
		fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("presentation.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));
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
	int run(int width, int height, double localtime){
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
		program::uniforms["color"]->set(1.f,102/255.f,0.f,1.f);
		sth_begin_draw(c->stash);
		sth_vmetrics(c->stash, 2, 100, NULL, NULL, &lh);
		dy -= lh*1.5f;
		sth_draw_text(c->stash, 2,100, dx,dy,"COMPO END :D!",&dx);
		sth_end_draw(c->stash);

		return localtime>5.0?1:0;
	}
};
class renderer_black : public renderer {
	void init(){}
	int operator()(renderer *prev, int width, int height, double localtime, double prevtime){
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
	rs->title = c->title;
	core::current = rs;
	JSONValue *efile = JSON::Parse(core::getfile("entries.json").c_str());
	auto entries = efile->AsArray();
	renderer *r = core::current;
	int pos = 0;
	for_each(entries.begin(), entries.end(), [&](JSONValue *v){
		pos++;
		renderer *pr = r;
		auto ir = new itemrenderer;
		ir->artist = wctos(v, L"artist");
		ir->group = wctos(v, L"group");
		ir->pos = pos;
		ir->title = wctos(v, L"title");
		ir->description = wctos(v, L"description");
		pr->setup(c, (r = ir));

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
			br->setup(c, (r = ir));
		}
	});
	r->setup(c,(new renderer_end)->setup(c, (new renderer_shutdown)->setup(c,0)));
	c->run();
}