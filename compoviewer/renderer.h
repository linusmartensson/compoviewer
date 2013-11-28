#pragma once

class core;

class renderer{
public:
	renderer *next;
	renderer *prev;
	core *c;
	
	
	renderer();
	virtual ~renderer();
	
	renderer* setup(core *c, renderer *next);
	virtual void init();
	
	virtual int operator()(renderer*, int w, int h, double localtime, double prevtime);
	virtual void key(int key, int scancode, int action, int mods);
	virtual void move(double xd, double yd);
	virtual void scroll(double xd, double yd);
};