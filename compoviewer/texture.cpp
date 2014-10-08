/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#include"texture.h"
#include"shaders.h"
#include"lodepng.h"
#include<iostream>
#include"core.h"

std::vector<framebuffer*> framebuffer::fbstack;

texture::texture() : w(-1), h(-1) {
	glGenTextures(1, &id);
}
texture::texture(GLuint id) : w(-1), h(-1), id(id) {}

void texture::bind(int slot){
	if(slot >= 0)
		glActiveTexture(slot+GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, id);
}
void texture::set(int internalformat, int dataformat, int datatype, int w, int h, void *data, bool full){
	bind(-1);
	this->internalformat = internalformat;
	this->dataformat = dataformat;
	this->datatype = datatype;
	
	if(w == this->w && h == this->h && !full){
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat,w,h, 0,dataformat,datatype,data);
	} else {
		this->w = w;
		this->h = h;
		glTexImage2D(GL_TEXTURE_2D, 0, internalformat, w, h, 0, dataformat, datatype, data);
		if(data != 0 && full){
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
	}
}

texture *texture::load(std::string filename){
	auto tex = new texture();
	std::vector<unsigned char> data;
	if(lodepng::decode(data, tex->w, tex->h, filename)){
		std::cerr<<"Could not decode "<<filename<<", file missing or corrupt!"<<std::endl;
		core::die();
	}
	tex->set(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, tex->w, tex->h, &data[0]);
	return tex;
}

void framebuffer::enter(framebuffer *fb){
	glBindFramebuffer(GL_FRAMEBUFFER, fb->id);
	fbstack.push_back(fb);
}
void framebuffer::exit(){
	fbstack.pop_back();
	glBindFramebuffer(GL_FRAMEBUFFER, fbstack.size() > 0?fbstack.back()->id:0);
}


framebuffer::framebuffer(){
	glGenFramebuffers(1, &id);
}

void framebuffer::set(GLuint slot, texture *t){
	glBindFramebuffer(GL_FRAMEBUFFER, id);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+slot, GL_TEXTURE_2D, t->id, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, fbstack.size()?fbstack.back()->id:0);
}