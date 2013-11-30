#pragma once
#include"texture.h"
#include"buffers.h"
#include"shaders.h"
#include"renderer.h"
#include<bass.h>
#include"core.h"

struct transition {

	texture *pret, *postt;
	framebuffer *fb, *fb2;
	
	buffer *buf;
	varray *arr;
	
	transition();
	int run(program *trans, renderer *pre, renderer *post, int width, int height, double pretime, double posttime, double transitiontime, bool first);
};

class transitionrenderer : public renderer {
	transition *trans;
	program *transprogram;
	

	bool play;
protected:
	virtual program *subinit() = 0;
	virtual int run(int width, int height, double localtime, bool first) = 0;
public:
	HSTREAM audio;
	float delay;
	double audiolength;
	double endtimehint;
	float length;

	std::string audiotrack;
	bool dotransition;
	transitionrenderer() : dotransition(true) {}
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
