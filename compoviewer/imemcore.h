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