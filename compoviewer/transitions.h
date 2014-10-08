/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#pragma once
#include"texture.h"
#include"buffers.h"
#include"shaders.h"
#include"renderer.h"
#include<bass.h>
#include"core.h"

#include<xmp.h>

struct transition {

	texture *pret, *postt;
	framebuffer *fb, *fb2;
	
	buffer *buf;
	varray *arr;
	
	transition();
	int run(program *trans, renderer *pre, renderer *post, int width, int height, double pretime, double posttime, double transitiontime, bool first);
};

struct module_event {
	//note+octave	instrument	volume(hex!)	effect (primary+secondary+teitrary)
	//C#4			02			20				R11
	std::string note;
	int notevalue;
	int instrument;
	int volume;
	std::string effect;

	std::string params;
};
typedef std::vector<module_event> module_row;
typedef std::vector<module_row> module_pattern;
typedef std::vector<module_pattern> module_song;
typedef std::map<int, int> order_to_pattern_map; 

class transitionrenderer : public renderer {
	transition *trans;
	program *transprogram;
	

	bool play;
protected:
	order_to_pattern_map opm;
	module_song ms;
	

	virtual program *subinit() = 0;
	virtual int run(int width, int height, double localtime, bool first) = 0;
public:
	xmp_context ctx;
	xmp_frame_info oldframe;
	volatile bool hasModData;
	HSTREAM audio;
	HMUSIC modfile;
	boolean tracked;
	float delay;
	double audiolength;
	double endtimehint;
	float length;

	std::string audiotrack;
	bool dotransition;
	transitionrenderer() : dotransition(true), tracked(false), hasModData(false), delay(0), audiolength(0), endtimehint(0), length(0), audiotrack("") {
		memset(&ctx, 0, sizeof(ctx));
		memset(&oldframe, 0, sizeof(oldframe));
	}
	void init();

	int operator()(renderer *pr, int width, int height, double localtime, double prevtime, bool first);
	int go;

	virtual void bye(unsigned int) {};

	void key(int key,int scancode,int action,int mods){
		if(key == c->previousItemKey() && action == c->actionKey()) {
			go = -1;
		} else if(key == c->nextItemKey() && action == c->actionKey()) {
			go = 1;
		}
	}
};

class fsshader{
	buffer *buf;
	varray *arr;
	program *p;
public:
	fsshader(program *p);
	void run();
};
