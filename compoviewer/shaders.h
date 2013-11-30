#pragma once
#define GLEW_STATIC
#include<GL/glew.h>
#include<map>
#include<algorithm>
#include<vector>
#include<set>

class attribute{
public:
	std::string name;
	bool touched;
	void *value;
	int pos;
	int type;
	int format;
	int size;
	attribute() {}
	void configure(int stride, int offset){
		glVertexAttribPointer(pos, size, format, GL_FALSE, stride, (void*)offset);
		glEnableVertexAttribArray(pos);
	}
};

class uniform;

class uniformconfig{
public:
	bool touched;
	int pos;
	uniform* u;
};

class texture;

class uniform{
public:
	std::string name;
	void *value;
	texture *aux;
	int type;
	std::set<uniformconfig *> configs;
	uniform() : type(GL_ZERO), value(0), aux(0) {}

	void set(texture *t, int slot){
		*(GLuint*)value = slot;
		aux = t;
		touched();
	}

	template<typename T>
	void set(T t){
		if(value == 0 || *(T*)value == t) return;
		*(T*)value = t;
		touched();
	}
	template<typename T>
	void set(T t, T t2){
		if(value == 0 || (((T*)value)[0] == t && ((T*)value)[1] == t2)) return;
		((T*)value)[0] = t;
		((T*)value)[1] = t2;
		touched();
	}
	template<typename T>
	void set(T t, T t2, T t3){
		if(value == 0 || (((T*)value)[0] == t && ((T*)value)[1] == t2 && ((T*)value)[2] == t3)) return;
		((T*)value)[0] = t;
		((T*)value)[1] = t2;
		((T*)value)[2] = t3;
		touched();
	}
	template<typename T>
	void set(T t, T t2, T t3, T t4){
		if(value == 0 || (((T*)value)[0] == t && ((T*)value)[1] == t2 && ((T*)value)[2] == t3 && ((T*)value)[3] == t4)) return;
		((T*)value)[0] = t;
		((T*)value)[1] = t2;
		((T*)value)[2] = t3;
		((T*)value)[3] = t4;
		touched();
	}
	void touched(){
		uniform *u = this;
		std::vector<uniformconfig*> toremove;
		std::for_each(configs.begin(), configs.end(), [&](uniformconfig *c){
			if(c->u != u) { toremove.push_back(c); return; }
			c->touched = true;
		});
		
		std::for_each(toremove.begin(),toremove.end(),[&](uniformconfig *bc){
			configs.erase(bc);
		});
	}
};


class program{
	static GLuint bound;
	static int attribindex;
public:
	const GLuint p;
	static std::map<std::string, uniform*> uniforms;
	static uniform* getuniform(std::string s){
		auto &u = uniforms[s];
		if(u == 0) u = new uniform();
		return u;
	}
	std::set<uniformconfig*> uniformconfigs;
	static std::map<std::string, attribute*> attribs;
	program(GLuint &p);
	
	void bind();
	void update();
	
	static std::vector<GLuint>& shader(GLenum type, std::string src, std::vector<GLuint> &shaders);
	static program *createProgram(std::vector<GLuint> &shaders);
};
