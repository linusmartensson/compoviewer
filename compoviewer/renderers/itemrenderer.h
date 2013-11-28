#pragma once
#include"../transitions.h"
#include"../core.h"
#include"../fontstash.h"

#include<GLFW/glfw3.h>
struct itemrenderer : public transitionrenderer {
	program *p;
	buffer *buf;
	varray *arr;
	std::string artist, group, title, description;
	float sx, sy, dx, dy, lh;
	int count;
	texture *texttexture;

	fsshader *fss;

	std::string previous;


	int pos;


	std::string number;

	program* subinit(){
		std::vector<GLuint> shaders;
		fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("beamer.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));
		
		texttexture = new texture(c->stash->tex);
		shaders.clear();
		p = program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("item_text.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("item_text.vert"), shaders)));
		buf = new buffer;
		arr = new varray;
		buf->set(VERT_COUNT*VERT_STRIDE, c->stash->verts, GL_DYNAMIC_DRAW); 
		arr->getconfig("text_position")->set(buf, 0, 0);
		count = 2048;

		std::ostringstream oss;
		oss<<"#";
		if(pos<10) oss<<"0";
		oss<<pos;

		number = oss.str();

		shaders.clear();
		return program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("transition2.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders)));
	}
	
	int run(int width, int height, double localtime, bool first){
		texttexture->bind(0, "tex");
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		
		
		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
		program::getuniform("iResolution")->set((float)width, (float)height);
		program::getuniform("iGlobalTime")->set((float)glfwGetTime());
		fss->run();

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
		sth_vmetrics(c->stash, 3, 100, NULL, NULL, &lh);
		dy -= lh*1.0f;
		sth_draw_text(c->stash, 3,100, dx,dy,title.c_str(),&dx);

		dx = width*6.f/7.f;
		sth_draw_text(c->stash, 3, 100, dx,dy,number.c_str(),&dx);

		sth_vmetrics(c->stash, 3, 50, NULL, NULL, &lh);
		dy -= lh*1.0f;
		dx = sx;
		sth_draw_text(c->stash, 3,50, dx,dy,artist.c_str(),&dx);
		sth_draw_text(c->stash, 3,50, dx,dy,"^",&dx);
		sth_draw_text(c->stash, 3,50, dx,dy,group.c_str(),&dx);
		std::istringstream iss(description);
		sth_vmetrics(c->stash, 3, 35, NULL, NULL, &lh);
		char str[2048];
		dy = height/4.f;
		dy -= lh*1.5f;
		dx = sx;
		while(iss.getline(str, 2048)){
			sth_draw_text(c->stash, 3,35, dx,dy,str,&dx);
		}

		dx = sx;
		dy = height-650.f-35.f;
		
		sth_draw_text(c->stash, 3,35, dx,dy,previous.c_str(),&dx);

		sth_end_draw(c->stash);

		return localtime>(c->category==3?60.0*10.0:5.0)?1:0;
	}
};
