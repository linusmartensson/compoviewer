#include"renderer.h"


//Base stub implementation
renderer::~renderer() {}
renderer::renderer() : prev(0), next(0) {}
renderer* renderer::setup(core *c, renderer *next){
	this->c = c; 
	this->next = next; 
	if(next != 0) next->prev = this; 
	init();
	return this;
}
void renderer::init(){};
int renderer::operator()(renderer*,int,int,double,double,bool){ return 1; }
void renderer::key(int,int,int,int){}
void renderer::move(double,double){}
void renderer::scroll(double,double){}