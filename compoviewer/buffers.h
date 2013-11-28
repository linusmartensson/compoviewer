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