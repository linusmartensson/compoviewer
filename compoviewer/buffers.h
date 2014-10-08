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

#include<set>
#include<vector>
#include<algorithm>
#include<map>
#include<string>
class buffer;
struct bufferconfig{
	buffer *b;
	int stride;
	int offset;
	bool touched;
	void set(buffer *b, int stride, int offset);
	bufferconfig() : touched(true) {}
};

class buffer{
	std::set<bufferconfig*> configs;
public:
	GLuint id;
	buffer();
	void addconfig(bufferconfig *c);
	void touch();
	void set(GLuint sz, GLvoid *data, GLenum type);
	void subset(GLuint offset, GLuint sz, GLvoid *data);
};

class varray{
	GLuint id;
	std::map<std::string, bufferconfig*> attribs;
public:
	varray();
	void update();
	bufferconfig* getconfig(std::string attrib);
};