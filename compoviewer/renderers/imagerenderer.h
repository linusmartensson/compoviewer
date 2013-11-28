#pragma once
#include "../transitions.h"
#include "../core.h"

struct imagerenderer : public transitionrenderer {
	program *p;
	buffer *buf;
	varray *arr;
	texture *tex;
	GLuint interpolation;
	std::string filename;
	program* subinit(){
		tex = texture::load(filename);
		tex->bind(-1, "");
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, interpolation);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, interpolation);

		std::vector<GLuint> shaders;
		p = program::createProgram(
			program::shader(GL_FRAGMENT_SHADER, 
			core::getfile("item_image.frag"), 
			program::shader(GL_VERTEX_SHADER, 
			core::getfile("item_image.vert"), shaders)));

		float bd[] = {   0.f,	0.f,
						 0.f,   1.f,
					  1280.f,   0.f,
					     1.f,   1.f,
						 0.f, 720.f,
						 0.f,   0.f,
					  1280.f, 720.f,
		                 1.f,   0.f};

		buf = new buffer;
		buf->set(sizeof(float)*4*4, bd, GL_STATIC_DRAW); 
		arr = new varray;
		arr->getconfig("text_position")->set(buf, 0, 0);
		
		shaders.clear();
		return program::createProgram(program::shader(GL_FRAGMENT_SHADER, core::getfile("transition2.frag"), program::shader(GL_VERTEX_SHADER, core::getfile("transition_test.vert"), shaders)));
	}
	int run(int width, int height, double localtime, bool first){
		glViewport(0, 0, width, height);
		glClearColor(0.0,0.0,0.0,1.0);
		glClear(GL_COLOR_BUFFER_BIT);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		program::getuniform("resolution")->set((float)width, (float)height);

		float tw = (float)tex->w;
		float th = (float)tex->h;

		
		//Scale largest image axis to respective window axis
		if(tw > th){
			th = th/tw*width;
			tw = (float)width;
		} else {
			tw = tw/th*height;
			th = (float)height;
		}

		//Scale any axis still outside bounds to window in case of strange aspect ratios
		if(tw > width){
			th = th/tw*width;
			tw = (float)width;
		}
		if(th > height){
			tw = tw/th*height;
			th = (float)height;
		}


		program::getuniform("image_resolution")->set(tw,th);
		tex->bind(0, "tex");
		p->update();
		arr->update();
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); 
		return localtime>10.0?1:0;
	}
};

