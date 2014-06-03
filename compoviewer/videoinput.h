#pragma once

#include "core.h"
#include <vlc/libvlc.h>
#include <vlc/vlc.h>
#include <string>
#include <iostream>

#define VIDEOBUFFERS 4

struct videoinput{
	struct dataStorage{
		unsigned int* data;
		volatile bool lock;
	};
private:
	static unsigned int lockCounter;
	static dataStorage m_DataStorage[VIDEOBUFFERS];
	static unsigned int *lastBuffer;
	
	libvlc_instance_t* libvlc;
	libvlc_media_t *media;
	libvlc_media_player_t *mediaplayer;

public:
	
	int width, height, lines, pitch;
	int64_t length;

	videoinput(std::string filename) : width(-1), height(-1) {

		libvlc = libvlc_new(0, NULL);

		if(NULL == libvlc) {
			std::cerr<<"LibVLC initialization failure."<<std::endl;
			core::die();
		}

		if(filename.find("://") != std::string::npos)
			media = libvlc_media_new_location(libvlc, filename.c_str());
		else
			media = libvlc_media_new_path(libvlc, filename.c_str());
		
		if(media == 0){
			std::cerr<<"Could not load video file "<<filename<<"!"<<std::endl;
			core::die();
		}
		mediaplayer = libvlc_media_player_new_from_media(media);
		if(mediaplayer == 0){
			std::cerr<<"Could not create media player for video file "<<filename<<"!"<<std::endl;
			core::die();
		}
		
		libvlc_video_set_callbacks(mediaplayer, lock, unlock, display, this);
		libvlc_video_set_format_callbacks(mediaplayer, setup, cleanup);

		libvlc_media_parse(media);

		length = libvlc_media_get_duration(media);
		if(length == 0){
			length = 1000*10000;
		}
		std::cout<<"Video length in ms: "<<length<<std::endl;
		

		libvlc_media_release(media);
	}

	void play()
	{
		libvlc_media_player_play(mediaplayer);
	}

	void stop()
	{
		libvlc_media_player_stop(mediaplayer);
	}

	~videoinput(void)
	{
		libvlc_media_player_stop(mediaplayer);
		libvlc_media_player_release(mediaplayer);
		libvlc_release(libvlc);

		for(unsigned int i=0;i<VIDEOBUFFERS;i++)
		{
			delete [] m_DataStorage[i].data;
		}

	}


	// remember to unlock
	dataStorage*	getDataStorage()
	{
		for(unsigned int i=0;i<VIDEOBUFFERS;i++)
		{
			if(m_DataStorage[i].data == lastBuffer)
			{
				m_DataStorage[i].lock = true;
				return &m_DataStorage[i];
			}
		}
		return NULL;
	}

	static unsigned setup(void **opaque, char *chroma, 
		unsigned *width, unsigned *height, 
		unsigned *pitches, unsigned *lines){

			videoinput *thiz = (videoinput*)*opaque;

			thiz->width = *width;
			thiz->height = *height;
			
			chroma[0] = 'R';
			chroma[1] = 'V';
			chroma[2] = '3';
			chroma[3] = '2';
			if(32*((*width)/32) == *width) 
				pitches[0] = *width*4;
			else {
				pitches[0] = ((*width)/32+1)*32*4;
			}
			if(32*((*height)/32) == *height) 
				lines[0] = *height;
			else {
				lines[0] = ((*height)/32+1)*32;
			}

			thiz->lines = lines[0];
			thiz->pitch = pitches[0];

			std::cout<<"allocating video space: "<<*width<<"x"<<*height<<std::endl;

			for(unsigned int i=0;i<VIDEOBUFFERS;i++) {
				m_DataStorage[i].data = new unsigned int[lines[0]*pitches[0]/4];
				memset(m_DataStorage[i].data, 0, sizeof(m_DataStorage[i].data));
				m_DataStorage[i].lock = false;
			}

			return 1;
	}
	static void cleanup(void *opaque){
		std::cout<<"CALLED CLEANUP"<<std::endl;
		lastBuffer = 0;
		for(unsigned int i=0;i<VIDEOBUFFERS;i++) {
			delete [] m_DataStorage[i].data;
			m_DataStorage[i].data = 0;
			m_DataStorage[i].lock = false;
		}
	}


	static void *lock(void *data, void **p_pixels)
	{
		//*p_pixels = m_DataStorage.data;
		for(unsigned int i=0;i<VIDEOBUFFERS;i++)
		{
			if(!m_DataStorage[i].lock && m_DataStorage[i].data != lastBuffer)
			{
				m_DataStorage[i].lock = true;
				*p_pixels = m_DataStorage[i].data;
				return NULL;
			}
		}
		return NULL;
	}


	static void unlock(void *data, void *id, void *const *p_pixels)
	{
		for(unsigned int i=0;i<VIDEOBUFFERS;i++)
		{
			if(m_DataStorage[i].data == (unsigned int*) *p_pixels)
			{
				m_DataStorage[i].lock = false;
				lastBuffer = m_DataStorage[i].data;
				return;
			}
		}
	}

	static void display(void *data, void *id)
	{
	}
public:


};
