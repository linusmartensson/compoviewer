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
		CATEGORY_TRACKED_AUDIO = 5,
		
	};

	virtual int previousItemKey() = 0;
	virtual int nextItemKey() = 0;
	virtual int actionKey() = 0;
	virtual double globalTime() = 0;
	virtual void subinit() = 0;
	
	void init();
	
	int width, height;
	bool vsync, fullscreen;
	static renderer *current;
	static renderer *previous;
	std::string requestedPlayer;
	sth_stash *stash;
	void initGLFonts();
	void readConfig();
	void run();
	
	static void die();
	static std::string getfile(std::string path);
	static std::wstring wgetfile(std::string path);

	core();
	virtual ~core(){}

	virtual bool dying() = 0;
	virtual void swapBuffers() = 0;
};

class tempcore : public core{
	virtual int previousItemKey(){return 0;};
	virtual int nextItemKey(){return 0;};
	virtual int actionKey(){return 0;};
	virtual double globalTime(){return 0.0;};
	virtual void subinit(){};
	virtual bool dying(){return true;};
	virtual void swapBuffers(){};
};

class renderer_shutdown : public renderer {
	int operator()(renderer *pr, int width, int height, double localtime, double prevtime, bool first){
		core::die(); 
		return 0;
	}
};