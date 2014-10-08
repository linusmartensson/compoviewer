/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#pragma once
#define GLEW_STATIC
#include<GL/glew.h>
#include<string>
#include<vector>

#include<map>

class framebuffer;

class texture{
public:
	GLuint id;
	unsigned int w, h, datatype, internalformat, dataformat;


	static texture* load(std::string filename);

	texture();
	texture(GLuint id);
	void bind(int slot);
	void set(int internalformat, int dataformat, int datatype, int w, int h, void *data=0, bool full = true);
};

class framebuffer{
	static std::vector<framebuffer*> fbstack;
	GLuint id;
	std::map<GLuint, texture*> outputs;
public:

	framebuffer();

	void set(GLuint slot, texture *t);

	static void enter(framebuffer *fb);
	static void exit();
};