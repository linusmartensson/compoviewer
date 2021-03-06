/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#include"transitions.h"

#include<bass.h>
#include<iostream>
#include<iomanip>
#include<sstream>
#include"core.h"
#include<xmp.h>
static char *note_name[] = {
	"C-", "C#", "D-", "D#", "E-", "F-", "F#", "G-", "G#", "A-", "A#", "B-"
};

static float bd[] = {	0.f,0.f,
						0.f,1.f,
						1.f,0.f,
						1.f,1.f};




fsshader::fsshader(program *p) : p(p){
	buf = new buffer;
	buf->set(sizeof(float)*4*4, bd, GL_STATIC_DRAW); 
	arr = new varray;
	arr->getconfig("transition_position")->set(buf, 0, 0);
}

void fsshader::run(){
	arr->update();
	p->update();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
transition::transition(){
		
	pret = new texture;
	pret->bind(-1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	postt = new texture;
	postt->bind(-1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	fb = new framebuffer;
	fb2 = new framebuffer;


	buf = new buffer;
	buf->set(sizeof(float)*4*4, bd, GL_STATIC_DRAW); 
	arr = new varray;
	arr->getconfig("transition_position")->set(buf, 0, 0);
}

int transition::run(program *trans, renderer *pre, renderer *post, int width, int height, double pretime, double posttime, double transitiontime, bool first){
		
	if(pret->w != width || pret->h != height)
		pret->set(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, width, height);
	fb->set(0, pret);

	framebuffer::enter(fb);
	(*pre)(0, width, height, pretime, 0, false);
	framebuffer::exit();
		
	if(postt->w != width || postt->h != height)
		postt->set(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, width, height);

	fb2->set(0, postt);

	framebuffer::enter(fb2);
	int state = (*post)(0, width, height, posttime, 0, first);
	framebuffer::exit();

	program::getuniform("texture_pre")->set(pret,0);
	program::getuniform("texture_post")->set(postt,1);
	program::getuniform("time")->set((float)transitiontime);
	arr->update();
	trans->update();
		
	glViewport(0,0, width, height);
	glDisable(GL_BLEND);
	glClearColor(1.0,0.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
		
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return state;
}


void transitionrenderer::init(){
	length = 1.f;
	go = 0;
	trans = new transition;
	

	std::string modname;
	std::string modtype;
	
	if(audiotrack != ""){
		
		if(!tracked)
			audio = BASS_StreamCreateFile(FALSE,audiotrack.c_str(),0,0,0);
		else {
			std::cerr<<"loading tracked music"<<std::endl;
			audio = modfile = BASS_MusicLoad(FALSE,audiotrack.c_str(),0,0,BASS_MUSIC_PRESCAN|BASS_MUSIC_NONINTER,0);
			
			if(audiotrack.rfind(".mod") != std::string::npos && audiotrack.size()-audiotrack.rfind(".mod") == 4){
				BASS_ChannelSetAttribute(audio, BASS_ATTRIB_MUSIC_PANSEP, 50);
				std::cout<<"using 50 stereo sep."<<std::endl;
			} else {
				BASS_ChannelSetAttribute(audio, BASS_ATTRIB_MUSIC_PANSEP, 100);
				std::cout<<"using 100 stereo sep."<<std::endl;
			}

			struct xmp_module_info mi;
			int row, pos;
			ctx = xmp_create_context();


			char * constcastfuckyou = const_cast<char *>(audiotrack.c_str());

			if (xmp_load_module(ctx, constcastfuckyou) < 0) {
				std::cerr<<"Could not load module XMP"<<std::endl;
			}

			if (xmp_start_player(ctx, 44100, 0) == 0) {
				xmp_get_module_info(ctx, &mi);
				modname = mi.mod->name;
				modtype = mi.mod->type;

				
				for(int i=0;i<256;++i){
					opm[i] = mi.mod->xxo[i];
				}

				row = pos = -1;
				for(int i=0;i<mi.mod->pat;++i){
					module_pattern mp;
					for(int j=0;j<mi.mod->xxp[i]->rows;++j){
						module_row mr;
						for(int k=0;k<mi.mod->chn;++k){
							xmp_event *e = &mi.mod->xxt[mi.mod->xxp[i]->index[k]]->event[j];
							module_event m;
				
							if(e->note > 0 && e->note < 0x80){
								std::ostringstream oss;
								oss<<note_name[(e->note-1)%12]<<(e->note-1)/12;
								m.note = oss.str();
							} else {
								m.note = e->note > 0?"===":"---";
							}
							m.notevalue = e->note-1;
							m.instrument = e->ins;
							
							m.volume = e->vol;
							std::ostringstream ess, prm;
							ess<<std::hex<<std::setfill(' ')<<std::setw(3)<<(int)e->fxt;
							m.effect = ess.str();
							
							prm<<std::hex<<std::setfill(' ')<<std::setw(2)<<m.instrument<<std::setw(2)<<m.volume<<m.effect;
							m.params = prm.str();
							mr.push_back(m);
						}
						mp.push_back(mr);
					}
					ms.push_back(mp);
				}
				xmp_end_player(ctx);
			}


			/*
			xmp_release_module(ctx);

			xmp_free_context(ctx);
			*/

		}
		if(!audio)
		{
			std::cerr<<"Missing audio file:"<<audiotrack<<std::endl;
			core::die();
		}
		audiolength = BASS_ChannelBytes2Seconds(audio, BASS_ChannelGetLength(audio, BASS_POS_BYTE));
		/*if(tracked){
			BASS_MusicFree(audio);
			audio = 0;
		}*/
	} else {
		audio = 0;
	}
	transprogram = subinit();
}

int transitionrenderer::operator()(renderer *pr, int width, int height, double localtime, double prevtime, bool first){
	
	if(first && (audio||tracked))
		play = true;

	if(play && localtime > delay){
		if(tracked){
			/*xmp_start_player(ctx, 44100, 0);
			if(audiotrack.rfind(".mod") != std::string::npos && audiotrack.size()-audiotrack.rfind(".mod") == 4){
				xmp_set_player(ctx, XMP_PLAYER_MIX, 50);
				std::cout<<"using 50 stereo sep."<<std::endl;
			} else {
				xmp_set_player(ctx, XMP_PLAYER_MIX, 100);
				std::cout<<"using 100 stereo sep."<<std::endl;
			}
			xmp_set_player(ctx, XMP_PLAYER_INTERP, XMP_INTERP_NEAREST);
			
			audio = modfile = BASS_StreamCreate(44100, 2, BASS_STREAM_AUTOFREE, &streamproc, (void *)this);
			BASS_ChannelSetAttribute(audio,BASS_ATTRIB_NOBUFFER,1);
			*/

		}
		BASS_ChannelPlay(audio, true);
		
		play = false;
	}
	
	if((audio||tracked) && localtime > audiolength+delay*2.0)
		go = 1;
	/*
	if(audio){
		BASS_ChannelUpdate(audio, 0);
	}*/

	int ret = go;
	go = 0;
	if(pr && localtime/length < 1.0 && dotransition){
		int r = trans->run(transprogram, pr, this, width, height, prevtime, localtime, localtime/length, first);
		if(!ret) ret = r;
	} else {
		int r = run(width, height, localtime, first);
		if(!ret) ret = r;
	}
	if(ret && (audio||tracked)){

		if(audio)
			BASS_ChannelStop(audio);
		/*
		if(tracked){
			xmp_restart_module(ctx);
		}*/
		play = false;
	}
	if(ret != 0){
		bye(ret);
	}
	return ret;
}