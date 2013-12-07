#include"core.h"
#include<fstream>
#include<string>

renderer *core::current = 0;
renderer *core::previous = 0;

void core::die(){
	exit(1);
}

std::string core::getfile(std::string path){
	std::string text;
	std::ifstream ifs("resources/"+path);
	char str[8192];
	while(ifs.getline(str, 8192)){
		text += std::string(str)+"\n";
	}
	ifs.close();
	return text;
}
std::wstring core::wgetfile(std::string path){
	std::wstring text;
	std::wifstream ifs("resources/"+path);
	if(ifs.peek() == (wchar_t)0xEF){
		wchar_t w;
		ifs>>w>>w>>w;
	}
	wchar_t str[8192];
	while(ifs.getline(str, 8192)){
		text += std::wstring(str)+L"\n";
	}
	ifs.close();
	return text;
}

void core::initGLFonts(){
	stash = sth_create(1024,1024);
	if(!(stash && sth_add_font(stash, 0, "resources/DroidSerif-Regular.ttf"))) die();
	if(!(stash && sth_add_font(stash, 1, "resources/DroidSerif-Italic.ttf"))) die();
	if(!(stash && sth_add_font(stash, 2, "resources/DreamHack Normal.ttf"))) die();
	if(!(stash && sth_add_font(stash, 3, "resources/Eurostile LT Medium.ttf"))) die();
}

void core::run(){
	double switchTime = globalTime();
	double prevTime = switchTime;
	bool first = true;
	while(!dying()){
		double t = globalTime();
		int ret = (*core::current)(core::previous, width, height, t-switchTime, t-prevTime, first);
		first = false;
		if(ret != 0) {
			prevTime = switchTime;
			switchTime = globalTime();
			core::previous = core::current;
			if(ret > 0 && core::current->next != 0) {
				core::current = core::current->next;
				first = true;
			} else if(ret < 0 && core::current->prev != 0) {
				core::current = core::current->prev;
				first = true;
			}
		}
		swapBuffers();
	}
	exit(0);
}