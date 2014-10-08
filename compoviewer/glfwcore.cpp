/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#define _GLFW_USE_DWM_SWAP_INTERVAL 1

#include"glfwcore.h"
#include"fontstash.h"
#include"renderer.h"

#define GLEW_STATIC
#include<GL/glew.h>
#include<GL/wglew.h>
#include<GLFW/glfw3.h>

#include<iostream>
#include<fstream>

int glfwcore::previousItemKey(){
	return GLFW_KEY_LEFT;
}
int glfwcore::nextItemKey(){
	return GLFW_KEY_RIGHT;
}
int glfwcore::actionKey(){
	return GLFW_PRESS;
}
double glfwcore::globalTime(){
	return glfwGetTime();
}
static void error_callback(int error, const char* description){
	glfwTerminate();
	std::cout<<description<<std::endl;
	exit(1);
}
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods){
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
	core::current->key(key,scancode,action,mods);
}
static void scroll_callback(GLFWwindow *window, double x, double y){core::current->scroll(x,y);}
static double cx = 0.0, cy = 0.0;
static void move_callback(GLFWwindow *window, double x, double y){core::current->move(x-cx, y-cy);cx = x;cy = y;}


void glfwcore::subinit() {
	
	if(!glfwInit()) exit(1);
	auto s = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetErrorCallback(&error_callback);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_DECORATED, 0);
	if(!(w=glfwCreateWindow(width, height, "fonttest", fullscreen?glfwGetPrimaryMonitor():0, 0))) exit(1);
	glfwGetFramebufferSize(w, &width, &height);
	glfwSetKeyCallback(w, &key_callback);
	glfwSetScrollCallback(w, &scroll_callback);
	glfwSetCursorPosCallback(w, &move_callback);
	glfwSetInputMode(w, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(w, cx, cy);
	glfwMakeContextCurrent(w);
	if(!fullscreen)
		glfwSetWindowPos(w, (s->width-width)/2, (s->height-height)/2);
	
	
	glfwSwapInterval(1);

	glewExperimental = true;
	if(glewInit() != GL_NO_ERROR) exit(1);
}

bool glfwcore::dying(){
	return glfwWindowShouldClose(w)!=0;
}
void glfwcore::swapBuffers(){
	glfwSwapBuffers(w);
	glFinish();
	glfwPollEvents();
}