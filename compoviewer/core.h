#pragma once
#include"renderer.h"
#include<string>

struct GLFWwindow;
struct sth_stash;

class core{
public:

	enum eCategory
	{
		CATEGORY_GFX_HIRES= 0,
		CATEGORY_GFX_OLDSCHOOL,
		CATEGORY_VIDEO,
		CATEGORY_AUDIO
	};

	int width, height;

	int category;

	static renderer *current;
	static renderer *previous;
	GLFWwindow *w;
	
	sth_stash *stash;

	std::string title;


	static void die();
	static std::string getfile(std::string path);
	static std::wstring wgetfile(std::string path);

	core();
	void run();
};

class renderer_shutdown : public renderer {
	int operator()(renderer *pr, int width, int height, double localtime, double prevtime, bool first){
		core::die(); 
		return 0;
	}
};