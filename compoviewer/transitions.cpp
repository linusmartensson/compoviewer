#include"transitions.h"


static float bd[] = {	0.f,0.f,
						0.f,1.f,
						1.f,0.f,
						1.f,1.f};


fsshader::fsshader(program *p) : p(p){
	buf = new buffer;
	buf->set(sizeof(float)*4*4, bd, GL_STATIC_DRAW); 
	arr = new varray;
	arr->getconfig("transition_position")->set(buf, 0, 0);
}

void fsshader::run(){
	arr->update();
	p->update();
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
}
transition::transition(){
		
	pret = new texture;
	pret->bind(-1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	postt = new texture;
	postt->bind(-1);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	fb = new framebuffer;
	fb2 = new framebuffer;


	buf = new buffer;
	buf->set(sizeof(float)*4*4, bd, GL_STATIC_DRAW); 
	arr = new varray;
	arr->getconfig("transition_position")->set(buf, 0, 0);
}

int transition::run(program *trans, renderer *pre, renderer *post, int width, int height, double pretime, double posttime, double transitiontime){
		
	if(pret->w != width || pret->h != height)
		pret->set(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, width, height);
	fb->set(0, pret);

	framebuffer::enter(fb);
	(*pre)(0, width, height, pretime, 0);
	framebuffer::exit();
		
	if(postt->w != width || postt->h != height)
		postt->set(GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE, width, height);

	fb2->set(0, postt);

	framebuffer::enter(fb2);
	int state = (*post)(0, width, height, posttime, 0);
	framebuffer::exit();

	pret->bind(0, "texture_pre");
	postt->bind(1, "texture_post");
	program::getuniform("time")->set((float)transitiontime);
	arr->update();
	trans->update();
		
	glViewport(0,0, width, height);
	glDisable(GL_BLEND);
	glClearColor(1.0,0.0,1.0,1.0);
	glClear(GL_COLOR_BUFFER_BIT);
		
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

	return state;
}


void transitionrenderer::init(){
	length = 1.f;
	trans = new transition;
	transprogram = subinit();
}

int transitionrenderer::operator()(renderer *pr, int width, int height, double localtime, double prevtime){
	if(pr && localtime/length < 1.0 && dotransition){
		return trans->run(transprogram, pr, this, width, height, prevtime, localtime, localtime/length);
	} else {
		return run(width, height, localtime);
	}
}