#include"imemcore.h"
#include"fontstash.h"
#include"renderer.h"

#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include"JSON.h"

#include<iostream>
#include<fstream>

#include<bass.h>

int imemcore::previousItemKey(){
	return GLFW_KEY_LEFT;
}
int imemcore::nextItemKey(){
	return GLFW_KEY_RIGHT;
}
int imemcore::actionKey(){
	return GLFW_PRESS;
}
double imemcore::globalTime(){
	return glfwGetTime();
}

static char tmp[8192];
static std::string wctos(JSONValue *v, const wchar_t *str){
	std::wcstombs(tmp, v->Child(str)->AsString().c_str(), 8192);
	return tmp;
}

imemcore::imemcore() {

}

bool imemcore::dying(){
	return true;
}
void imemcore::swapBuffers(){
}