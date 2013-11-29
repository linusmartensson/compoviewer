#include "videocore.h"

unsigned int videocore::lockCounter = 0;
videocore::dataStorage videocore::m_DataStorage[VIDEOBUFFERS];
unsigned int *videocore::lastBuffer = 0;