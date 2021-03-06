/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#pragma once
#include"../transitions.h"
#include"../core.h"
#include"../fontstash.h"

#include<bassmod.h>
#include<iomanip>

struct itemrenderer : public transitionrenderer {
	program *p;
	buffer *buf;
	varray *arr;
	std::string artist, group, title, description;
	float sx, sy, dx, dy, lh;
	int count, category;
	texture *texttexture;

	fsshader *fss;

	std::string previous;


	int pos;

	texture *audiotex;
	texture *audiostex;
	texture *sponsor;
	texture *logo;

	std::string number;
	std::string sponsor_file;
	itemrenderer() : sponsor_file("") {}

	float idata[512];
	float sdata[512];
	float ldata[2];
	float lsdata[2];
	float hassponsor;

	program* subinit(){
		endtimehint = 5.0;
		std::vector<GLuint> shaders;
		fss = new fsshader(program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("audiobeamer.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders))));
		audiotex = new texture;
		audiostex = new texture;
		logo = texture::load("resources/dice-logotype-black.png");
		
		memset(idata, 0.f, sizeof(idata));
		memset(sdata, 0.f, sizeof(sdata));


		if(sponsor_file != ""){
			std::cout<<"Loading sponsor!"<<std::endl;
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
	
	float data[8192];
	
	int run(int width, int height, double localtime, bool first){
		
		program::getuniform("tex")->set(texttexture, 0);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		
		
		program::getuniform("audiolevel")->set(0.f,0.f,0.f,0.f);
		program::getuniform("category")->set((int)category);
		if(audio){
			
			if(first) {
				memset(idata, 0, sizeof(idata));
				memset(sdata, 0, sizeof(sdata));
				memset(data, 0, sizeof(data));
				ldata[0] = ldata[1] = lsdata[0] = lsdata[1] = 0.f;
			}
			
			if(localtime < (audiolength+delay) && localtime > delay)
				BASS_ChannelGetData(audio, data, BASS_DATA_FFT512|BASS_DATA_FFT_INDIVIDUAL);
			
			
			for(int i=0;i<512;++i){
				idata[i]=idata[i]*0.97f>data[i]?idata[i]*0.97f:data[i];
				sdata[i]=(sdata[i]+idata[i]);
			}
			
			program::getuniform("iChannel0")->set(audiotex, 0);
			program::getuniform("iChannel1")->set(audiostex, 1);
			
			audiotex->set(GL_R32F, GL_RED, GL_FLOAT, 2, 256, idata);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			audiostex->set(GL_R32F, GL_RED, GL_FLOAT, 2, 256, sdata);
			
			auto level = BASS_ChannelGetLevel(audio);
			if(level != -1){
				ldata[0] = ldata[0]*0.97f>LOWORD(level)/32768.f?ldata[0]*0.97f:LOWORD(level)/32768.f;
				ldata[1] = ldata[1]*0.97f>HIWORD(level)/32768.f?ldata[1]*0.97f:HIWORD(level)/32768.f;
				lsdata[0] += ldata[0]*ldata[0];
				lsdata[1] += ldata[1]*ldata[1];

				program::getuniform("audiolevel")->set(ldata[0],ldata[1],lsdata[0],lsdata[1]);
			}
		} else {
			
			program::getuniform("iChannel0")->set(audiotex, 0);
			program::getuniform("iChannel1")->set(audiostex, 1);
			audiotex->set(GL_R32F, GL_RED, GL_FLOAT, 2, 256, idata);
			audiostex->set(GL_R32F, GL_RED, GL_FLOAT, 2, 256, sdata);
			program::getuniform("audiolevel")->set(0.f,0.f,0.f,0.f);
		}
		
		program::getuniform("hassponsor")->set((float)hassponsor);
		program::getuniform("sponsor_res")->set((float)sponsor->w, (float)sponsor->h);
		program::getuniform("sponsor")->set(sponsor, 2);

		program::getuniform("logo")->set(logo, 4);
		program::getuniform("logores")->set((float)logo->w, (float)logo->h);

		glEnable(GL_BLEND);
		glBlendFuncSeparate(GL_ONE, GL_ZERO, GL_ZERO, GL_ONE);
		
		
		program::getuniform("iResolution")->set((float)width, (float)height);
		program::getuniform("iGlobalTime")->set((float)c->globalTime());
		
		fss->run();

		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		dx = sx = 50*height/1080.0; 
		dy = sy = height;
		
		program::getuniform("time")->set((float)localtime);
		program::getuniform("resolution")->set((float)width, (float)height);
		
		c->stash->drawfunc = [&](int v){
			p->update();
			buf->subset(0, c->stash->nverts*VERT_STRIDE, c->stash->verts);
			arr->update();
			glDrawArrays(GL_TRIANGLES, 0, c->stash->nverts);
		};
		program::getuniform("color")->set(0.0f,0.0f,0.f,1.f);
		sth_begin_draw(c->stash);
		sth_vmetrics(c->stash, 2, 100*height/1080.0, NULL, NULL, &lh);
		dy -= lh*1.0f;
		sth_draw_text(c->stash, 2,100*height/1080.0, dx,dy,title.c_str(),&dx);

		dx = width - 150*width/1280.f;
		sth_draw_text(c->stash, 2, 100*height/1080.0, dx,dy,number.c_str(),&dx);

		sth_vmetrics(c->stash, 2, 50*height/1080.0, NULL, NULL, &lh);
		dy -= lh*1.0f;
		dx = sx;
		sth_draw_text(c->stash, 2,50*height/1080.0, dx,dy,artist.c_str(),&dx);
		sth_draw_text(c->stash, 2,50*height/1080.0, dx,dy,"^",&dx);
		sth_draw_text(c->stash, 2,50*height/1080.0, dx,dy,group.c_str(),&dx);
		std::istringstream iss(description);
		sth_vmetrics(c->stash, 2, 35*height/1080.0, NULL, NULL, &lh);
		char str[2048];
		dy = 200*height/1080.0;
		dy -= lh*1.5f;
		dx = sx;
		while(iss.getline(str, 2048)){
			sth_draw_text(c->stash, 2,35*height/1080.0, dx,dy,str,&dx);
			dy -= lh*0.75f;
			dx = sx;
		}
		
		dx = sx;
		dy = 50;
		
		sth_draw_text(c->stash, 2,35*height/1080.0, dx,dy,previous.c_str(),&dx);

		sth_end_draw(c->stash);


		if(tracked && audio)
			drawModText(width, height);
		
		return localtime>(audio?60.0*10.0:endtimehint)?1:0;
	}

	void drawModText(int width, int height){
		
		int pat, row;

		{
			//xmp_frame_info &frame = oldframe;
			QWORD p = BASS_ChannelGetPosition(audio, BASS_POS_MUSIC_ORDER);
			pat = *(BASS_ChannelGetTags(audio, BASS_TAG_MUSIC_ORDERS)+LOWORD(p));
			row = HIWORD(p);
		}

		if(pat >= ms.size() || pat < 0) return;
		if(row >= ms[pat].size() || row < 0) return;

		auto pattern = ms[pat];
		
		
		float lhe = 10*width/1280.f;
		float smallfont = 5*width/1280.f;
		float large = 10*width/1280.f;

	
		sth_vmetrics(c->stash, 3, lhe, NULL, NULL, &lh);

		int cs = 0, ce = pattern[row].size();
		int sce = ce;

		float dxo = ce;

		int count = 20;

		if(ce > 12){

			if(ce > 24){
				ce /= 4;
				dxo = ce -cs;
				count /= 4;
				count -= 1;
				drawthis(width, height, dxo, large, pattern, row, cs, ce, height*23.f/32.f, count );
				
				cs = ce;
				ce = sce/2;
				dxo = ce -cs;
				drawthis(width, height, dxo, large, pattern, row, cs, ce, height*18.5f/32.f, count );
				
				cs = ce;
				ce = sce*3/4;
				dxo = ce -cs;
				drawthis(width, height, dxo, large, pattern, row, cs, ce, height*13.5f/32.f, count );
				
				cs = ce;
				ce = sce;
				dxo = ce -cs;
				drawthis(width, height, dxo, large, pattern, row, cs, ce, height*9.f/32.f, count );
			} else {
				ce /= 2;
				dxo = ce-cs;
				count /= 2;
				count -= 2;

				drawthis(width, height, dxo, large, pattern, row, cs, ce, height*20.5f/32.f, count );

				cs = ce;
				ce = pattern[row].size();
				dxo = ce-cs;

				drawthis(width, height, dxo, large, pattern, row, cs, ce, height*11.5f/32.f, count  );
			}
		} else {
				drawthis(width, height, dxo, large, pattern, row, cs, ce, height/2.0f, count  );
		}

		
	}

	void drawthis(float width, float height, float dxo, float large, module_pattern &pattern, int row, int cs, int ce, float dys, int count){
		
		dy = dys;
		dx = sx = width/2.f - 50*width/1280.f*dxo;
		sth_begin_draw(c->stash);
		program::getuniform("color")->set(0.98039215686f,0.34901960784f,0.f,1.f);
		std::ostringstream oss; 
		oss<<std::setfill('0')<<std::setw(3)<<row;
		sth_draw_text(c->stash, 3, large, dx, dy, oss.str().c_str(), &dx);

		for(int j=cs;j<ce;++j){
			sth_draw_text(c->stash, 3, large, dx,dy,pattern[row][j].note.c_str(),&dx);
			sth_draw_text(c->stash, 3, large, dx,dy,pattern[row][j].params.c_str(),&dx);
			dx += 10.0;
		}
		sth_end_draw(c->stash);
		sth_begin_draw(c->stash);
		program::getuniform("color")->set(0.23f,0.23f,0.23f,1.f);
		for(int i=row-1,j=0;i>=0&&j<count;--i,++j){
			dx = sx; 
			dy += lh;

			std::ostringstream oss; 
			oss<<std::setfill('0')<<std::setw(3)<<i;
			sth_draw_text(c->stash, 3, large, dx, dy, oss.str().c_str(), &dx);

			for(int j=cs;j<ce;++j){
				
				sth_draw_text(c->stash, 3, large, dx,dy,pattern[i][j].note.c_str(),&dx);
				sth_draw_text(c->stash, 3, large, dx,dy,pattern[i][j].params.c_str(),&dx);
				dx += 10.0;
			}
		}
		dy = dys;
		for(int i=row+1,j=0;i<pattern.size()&&j<count;++i,++j){
			dx = sx; 
			dy -= lh;
			std::ostringstream oss; 
			oss<<std::setfill('0')<<std::setw(3)<<i;
			sth_draw_text(c->stash, 3, large, dx, dy, oss.str().c_str(), &dx);

			for(int j=cs;j<ce;++j){
				sth_draw_text(c->stash, 3, large, dx,dy,pattern[i][j].note.c_str(),&dx);
				sth_draw_text(c->stash, 3, large, dx,dy,pattern[i][j].params.c_str(),&dx);
				dx += 10.0;
			}
		}
		sth_end_draw(c->stash);
	}

};
