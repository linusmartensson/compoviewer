#ifndef _VIDEOCORE_H
#define _VIDEOCORE_H

#include "core.h"
#include <vlc/libvlc.h>
#include <vlc/vlc.h>
#include <string>
#include <iostream>

class videocore
{
public:
	videocore(unsigned int width, unsigned int height, std::string filename)
	{

		for(unsigned int i=0;i<numBuffers;i++)
		{
			m_DataStorage[i].data = new unsigned short[width*height];
			m_DataStorage[i].lock = false;
		}

		lastBuffer = 0;

		libvlc = libvlc_new(0, NULL);

		if(NULL == libvlc) {
			std::cerr<<"LibVLC initialization failure."<<std::endl;
			core::die();
		} 

		media = libvlc_media_new_path(libvlc, filename.c_str());
		mediaplayer = libvlc_media_player_new_from_media(media);
		libvlc_media_release(media);

		
		libvlc_video_set_callbacks(mediaplayer, lock, unlock, display, NULL);
		libvlc_video_set_format(mediaplayer, "RV16", width, height, width*2);
	}

	void play()
	{
		libvlc_media_player_play(mediaplayer);
	}

	void stop()
	{
		libvlc_media_player_stop(mediaplayer);
	}

	~videocore(void)
	{
		libvlc_media_player_stop(mediaplayer);
		libvlc_media_player_release(mediaplayer);
		libvlc_release(libvlc);

		for(unsigned int i=0;i<numBuffers;i++)
		{
			delete [] m_DataStorage[i].data;
		}

	}

	//needs mutex?
	struct dataStorage
	{
		unsigned short* data;
		unsigned int width;
		unsigned int height;
		bool lock;
	};

	// remember to unlock
	dataStorage*	getDataStorage()
	{
		for(unsigned int i=0;i<numBuffers;i++)
		{
			if(m_DataStorage[i].data == lastBuffer)
			{
				m_DataStorage[i].lock = true;
				return &m_DataStorage[i];
			}
		}
		return NULL;
	}

private:

	libvlc_instance_t* libvlc;
	libvlc_media_t *media;
	libvlc_media_player_t *mediaplayer;

	static void *lock(void *data, void **p_pixels)
	{
		//*p_pixels = m_DataStorage.data;
		for(unsigned int i=0;i<numBuffers;i++)
		{
			if(!m_DataStorage[i].lock)
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
		for(unsigned int i=0;i<numBuffers;i++)
		{
			if(m_DataStorage[i].data = (unsigned short*) p_pixels)
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

	static const unsigned int numBuffers = 4;
	static unsigned int lockCounter;
	static dataStorage m_DataStorage[numBuffers];
	static unsigned short *lastBuffer;
	static unsigned int m_width,m_height;


};

#endif // _VIDEOCORE_H

