#include"core.h"
#include"JSON.h"
#include<fstream>
#include<string>
#include<iostream>
#include<bass.h>


renderer *core::current = 0;
renderer *core::previous = 0;

void core::die(){
	exit(1);
}

static char tmp[8192];
static std::string wctos(JSONValue *v, const wchar_t *str){
	std::wcstombs(tmp, v->Child(str)->AsString().c_str(), 8192);
	return tmp;
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
	stash = sth_create(2048,2048);
	if(!(stash && sth_add_font(stash, 0, "resources/DroidSerif-Regular.ttf"))) die();
	if(!(stash && sth_add_font(stash, 1, "resources/DroidSerif-Italic.ttf"))) die();
	if(!(stash && sth_add_font(stash, 2, "resources/Eurostile LT Medium.ttf"))) die();
	if(!(stash && sth_add_font(stash, 3, "resources/Topaz-8.ttf"))) die();
}

void core::readConfig(){
	std::wstring conf = wgetfile("config.json");
	JSONValue *config = JSON::Parse(conf.c_str());
	width = config->Child(L"width")->AsNumber();
	height = config->Child(L"height")->AsNumber();
	fullscreen = config->Child(L"fullscreen")->AsBool();
	vsync = config->Child(L"vsync")->AsBool();
	requestedPlayer = wctos(config, L"player");
}
core::core(){
	readConfig();
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

void core::init(){
	if(!BASS_Init(-1,48000,BASS_DEVICE_SPEAKERS,0,0)){
		std::cerr<<"Unable to initialize sound! Maybe already initialized?"<<std::endl;
		die();
	}/*
	BASS_SetConfig(BASS_CONFIG_UPDATEPERIOD, 1000/60);
	BASS_SetConfig(BASS_CONFIG_BUFFER, 120);
	BASS_SetConfig(BASS_CONFIG_UPDATETHREADS, 0);*/
	subinit();
	initGLFonts();
}