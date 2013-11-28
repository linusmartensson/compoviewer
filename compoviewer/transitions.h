#pragma once
#include"texture.h"
#include"buffers.h"
#include"shaders.h"
#include"renderer.h"

struct transition {

	texture *pret, *postt;
	framebuffer *fb, *fb2;
	
	buffer *buf;
	varray *arr;
	
	transition();
	int run(program *trans, renderer *pre, renderer *post, int width, int height, double pretime, double posttime, double transitiontime);
};
#include<GLFW/glfw3.h>
class transitionrenderer : public renderer {
	transition *trans;
	program *transprogram;
	float length;
protected:
	virtual program *subinit() = 0;
	virtual int run(int width, int height, double localtime) = 0;
public:
	bool dotransition;
	transitionrenderer() : dotransition(true) {}
	void init();

	int operator()(renderer *pr, int width, int height, double localtime, double prevtime);
	int go;
	void key(int key,int scancode,int action,int mods){
		if(key == GLFW_KEY_LEFT && action == GLFW_PRESS) {
			go = -1;
		} else if(key == GLFW_KEY_RIGHT && action == GLFW_PRESS) {
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
