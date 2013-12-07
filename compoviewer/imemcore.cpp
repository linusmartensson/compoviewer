#include"imemcore.h"
#include"fontstash.h"
#include"renderer.h"

#define GLEW_STATIC
#include<GL/glew.h>
#include<gl/wglew.h>
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
	return time/1000000000.0;
}

void imemcore::subinit(){
	time = 0;
	HWND hwnd = CreateWindowEx(WS_EX_TOOLWINDOW,TEXT("STATIC"),TEXT("glctx"),
WS_POPUP|WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN, 
0,0,640,480,0,0,GetModuleHandle(NULL),0); 

	//Setup GL context
	HDC dc = GetWindowDC(hwnd);
	std::cout<<"Got DC:"<<dc<<std::endl;
	PIXELFORMATDESCRIPTOR pfd = { 
    sizeof(PIXELFORMATDESCRIPTOR),   // size of this pfd  
    1,                     // version number  
    PFD_DRAW_TO_WINDOW |   // support window  
    PFD_SUPPORT_OPENGL |   // support OpenGL  
    PFD_DOUBLEBUFFER,      // double buffered  
    PFD_TYPE_RGBA,         // RGBA type  
    32,                    // 24-bit color depth  
    0, 0, 0, 0, 0, 0,      // color bits ignored  
    0,                     // no alpha buffer  
    0,                     // shift bit ignored  
    0,                     // no accumulation buffer  
    0, 0, 0, 0,            // accum bits ignored  
    32,                    // 32-bit z-buffer  
    0,                     // no stencil buffer  
    0,                     // no auxiliary buffer  
    PFD_MAIN_PLANE,        // main layer  
    0,                     // reserved  
    0, 0, 0                // layer masks ignored  
}; 
	int  iPixelFormat; 
	iPixelFormat = ChoosePixelFormat(dc, &pfd); 
	std::cout<<SetPixelFormat(dc, iPixelFormat, &pfd)<<std::endl;
	std::cout<<GetLastError()<<std::endl;
	HGLRC trc = wglCreateContext(dc);
	std::cout<<"Got temporary rendering context:"<<trc<<" - "<<GetLastError()<<std::endl;
	wglMakeCurrent(dc, trc);
	

	glewExperimental = true;
	int i = glewInit();
	std::cout<<i<<", "<<glewGetErrorString(i)<<std::endl;
	if(i != GL_NO_ERROR) exit(1);

	int attribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
		WGL_CONTEXT_MINOR_VERSION_ARB, 0,
		0
	};

	HGLRC rc = wglCreateContextAttribsARB(dc, NULL, attribs);
	wglMakeCurrent(NULL, NULL);
	wglDeleteContext(trc);
	wglMakeCurrent(dc, rc);
}

bool imemcore::dying(){
	return true;
}
void imemcore::swapBuffers(){
	
	//Manage a buffer flip with imem

	//Increment time by 1/60ms up to nanosecond precision.
	time += (long)(1000000000L/60L);

}