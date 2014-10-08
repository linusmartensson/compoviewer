/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
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
	
	virtual int operator()(renderer*, int w, int h, double localtime, double prevtime, bool first);
	virtual void key(int key, int scancode, int action, int mods);
	virtual void move(double xd, double yd);
	virtual void scroll(double xd, double yd);
};