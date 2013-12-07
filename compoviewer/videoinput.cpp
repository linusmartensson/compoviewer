#include "videoinput.h"

unsigned int videoinput::lockCounter = 0;
videoinput::dataStorage videoinput::m_DataStorage[VIDEOBUFFERS];
unsigned int *videoinput::lastBuffer = 0;