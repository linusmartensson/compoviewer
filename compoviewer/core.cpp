#include"core.h"
#include"fontstash.h"
#include"renderer.h"

#define GLEW_STATIC
#include<GL/glew.h>
#include<GLFW/glfw3.h>

#include"JSON.h"

#include<iostream>
#include<fstream>

renderer *core::current = 0;
renderer *core::previous = 0;

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


static char tmp[8192];
static std::string wctos(JSONValue *v, const wchar_t *str){
	std::wcstombs(tmp, v->Child(str)->AsString().c_str(), 8192);
	return tmp;
}

core::core() {
	std::string conf = getfile("test.json");
	JSONValue *config = JSON::Parse(conf.c_str());

	title = wctos(config, L"compo");

	category = config->Child(L"category")->AsNumber();

	width = config->Child(L"width")->AsNumber();
	height = config->Child(L"height")->AsNumber();
	bool fullscreen = config->Child(L"fullscreen")->AsBool();

	if(!glfwInit()) exit(1);
	auto s = glfwGetVideoMode(glfwGetPrimaryMonitor());
	glfwSetErrorCallback(&error_callback);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
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
	else
		glfwSwapInterval(config->Child(L"vsync")->AsBool()?1:0);

	glewExperimental = true;
	if(glewInit() != GL_NO_ERROR) exit(1);

	stash = sth_create(512,512);
	if(!(stash && sth_add_font(stash, 0, "resources/DroidSerif-Regular.ttf"))) die();
	if(!(stash && sth_add_font(stash, 1, "resources/DroidSerif-Italic.ttf"))) die();
	if(!(stash && sth_add_font(stash, 2, "resources/DreamHack Normal.ttf"))) die();
	if(!(stash && sth_add_font(stash, 3, "resources/Eurostile LT Medium.ttf"))) die();
}

void core::die(){
	glfwTerminate();
	exit(1);
}

std::string core::getfile(std::string path){
	std::string text;
	std::ifstream ifs("resources/"+path);
	char str[8192];
	while(ifs.getline(str, 8192)){
		text += std::string(str)+"\n";
	}
	ifs.close();
	return text;
}

void core::run(){
	double switchTime = glfwGetTime();
	double prevTime = switchTime;
	
	while(!glfwWindowShouldClose(w)){
		double t = glfwGetTime();
		int ret = (*core::current)(
			core::previous, width, height, t-switchTime, t-prevTime);
		if(ret != 0) {
			prevTime = switchTime;
			switchTime = glfwGetTime();
			core::previous = core::current;
			if(ret > 0 && core::current->next != 0) {
				core::current = core::current->next;
			} else if(ret < 0 && core::current->prev != 0) {
				core::current = core::current->prev;
			}
		}
		glfwSwapBuffers(w);
		glfwPollEvents();
	}
	exit(0);
}