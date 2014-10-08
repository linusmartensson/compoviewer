/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#pragma once
#include<string>
#include"core.h"

struct GLFWwindow;

class imemcore : public core{
public:
	long time;
	virtual int previousItemKey();
	virtual int nextItemKey();
	virtual int actionKey();
	virtual double globalTime();

	GLFWwindow *w;

	virtual ~imemcore(){}

	virtual bool dying();
	virtual void swapBuffers();
	virtual void subinit();
};