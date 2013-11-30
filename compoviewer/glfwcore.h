#pragma once
#include<string>
#include"core.h"

struct GLFWwindow;

class glfwcore : public core{
public:

	virtual int previousItemKey();
	virtual int nextItemKey();
	virtual int actionKey();
	virtual double globalTime();

	GLFWwindow *w;

	
	glfwcore();
	virtual void run();
	virtual ~glfwcore(){}

};