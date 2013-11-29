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
	void bind(int slot, std::string name="");
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