#pragma once
#include"renderer.h"
#include<string>

struct GLFWwindow;
struct sth_stash;

class core{
public:

	enum eCategory
	{
		CATEGORY_AUDIO = 1,
		CATEGORY_GFX_HIRES = 2,
		CATEGORY_GFX_OLDSCHOOL = 3,
		CATEGORY_VIDEO = 4,
		
	};

	int width, height;

	static renderer *current;
	static renderer *previous;
	GLFWwindow *w;
	
	sth_stash *stash;

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