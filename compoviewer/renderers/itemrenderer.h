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

	texture *audiotex;
	texture *audiostex;
	texture *sponsor;

	std::string number;
	std::string sponsor_file;

	float idata[512];
	float sdata[512];
	float ldata[2];
	float lsdata[2];
	float hassponsor;

	program* subinit(){
		endtimehint = 5.0;
		std::vector<GLuint> shaders;
		if(audio){
			fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("audiobeamer.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));
			audiotex = new texture;
			audiostex = new texture;
		} else {
			fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("beamer.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));
		}

		
		if(sponsor_file != ""){
			std::cout<<"Loading sponsor file"<<std::endl;
			sponsor = texture::load(sponsor_file);
			hassponsor = 1.0;
		} else {
			hassponsor = 0.0;
			sponsor = new texture;
			unsigned int i[] = {0,0,0,0};
			sponsor->set(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, 2, 2, &i);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 1);
		}

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
		
		
		program::getuniform("audiolevel")->set(0.f,0.f,0.f,0.f);
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
				sdata[i]=(sdata[i]+idata[i]);
			}
			
			audiotex->bind(0, "iChannel0");
			
			audiostex->bind(1, "iChannel1");
			
			audiotex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, idata);
			
			audiostex->set(GL_R32F, GL_RED, GL_FLOAT, 512, 1, sdata);
			

			auto level = BASS_ChannelGetLevel(audio);
			if(level != -1){
				ldata[0] = ldata[0]*0.97f>LOWORD(level)/32768.f?ldata[0]*0.97f:LOWORD(level)/32768.f;
				ldata[1] = ldata[1]*0.97f>HIWORD(level)/32768.f?ldata[1]*0.97f:HIWORD(level)/32768.f;
				lsdata[0] += ldata[0];
				lsdata[1] += ldata[1];

				program::getuniform("audiolevel")->set(ldata[0],ldata[1],lsdata[0],lsdata[1]);
			}
		}

		program::getuniform("hassponsor")->set((float)hassponsor);
		program::getuniform("sponsor_res")->set((float)sponsor->w, (float)sponsor->h);
		sponsor->bind(2, "sponsor");

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
		dy = height-650.f;
		
		sth_draw_text(c->stash, 3,35, dx,dy,previous.c_str(),&dx);

		sth_end_draw(c->stash);

		return localtime>(audio?60.0*10.0:endtimehint)?1:0;
	}
};
