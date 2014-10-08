/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#include"buffers.h"
#include"shaders.h"

varray::varray(){
	glGenVertexArrays(1, &id);
}
void varray::update(){
	glBindVertexArray(id);
	std::for_each(attribs.begin(),attribs.end(), [&](const std::pair<std::string, bufferconfig*> &b){
		if(!b.second->touched) return;
		glBindBuffer(GL_ARRAY_BUFFER, b.second->b->id);
		program::attribs[b.first]->configure(b.second->stride, b.second->offset);
		b.second->touched = false;
	});
}
bufferconfig* varray::getconfig(std::string attrib){
	if(attribs.count(attrib) == 0) attribs[attrib] = new bufferconfig;
	return attribs[attrib];
}

void bufferconfig::set(buffer *b, int stride, int offset){
	b->addconfig(this);
	this->b = b;
	touched = true;
	this->stride = stride;
	this->offset = offset;
}

void buffer::addconfig(bufferconfig *c){
	configs.insert(c);
}

void buffer::touch(){
	buffer *b = this;
	std::vector<bufferconfig*> toremove;
	std::for_each(configs.begin(),configs.end(),[&](bufferconfig *bc){
		if(bc->b != b){toremove.push_back(bc); return;}
		bc->touched = true;
	});
	std::for_each(toremove.begin(),toremove.end(),[&](bufferconfig *bc){
		configs.erase(bc);
	});
}

void buffer::set(GLuint sz, GLvoid *data, GLenum type){
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, sz, data, type);

	touch();
}
void buffer::subset(GLuint offset, GLuint sz, GLvoid *data){
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferSubData(GL_ARRAY_BUFFER, offset, sz, data);

	touch();
}

buffer::buffer(){
	glGenBuffers(1, &id);
}