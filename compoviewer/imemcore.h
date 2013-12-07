#pragma once
#include<string>
#include"core.h"

struct GLFWwindow;

class imemcore : public core{
public:

	virtual int previousItemKey();
	virtual int nextItemKey();
	virtual int actionKey();
	virtual double globalTime();

	GLFWwindow *w;

	
	imemcore();
	virtual ~imemcore(){}

	virtual bool dying();
	virtual void swapBuffers();
};