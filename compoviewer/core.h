#pragma once
#include"renderer.h"
#include<string>

struct sth_stash;

#include"fontstash.h"

class core{
public:

	enum eCategory
	{
		CATEGORY_AUDIO = 1,
		CATEGORY_GFX_HIRES = 2,
		CATEGORY_GFX_OLDSCHOOL = 3,
		CATEGORY_VIDEO = 4,
		
	};

	virtual int previousItemKey() = 0;
	virtual int nextItemKey() = 0;
	virtual int actionKey() = 0;
	virtual double globalTime() = 0;
	
	int width, height;
	static renderer *current;
	static renderer *previous;
	sth_stash *stash;

	static void die();
	static std::string getfile(std::string path);
	static std::wstring wgetfile(std::string path);

	virtual void run() = 0;
	virtual ~core(){}

};

class renderer_shutdown : public renderer {
	int operator()(renderer *pr, int width, int height, double localtime, double prevtime, bool first){
		core::die(); 
		return 0;
	}
};