/*
 * ----------------------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * <spline@secretweb.org> wrote this file. As long as you retain this notice you
 * can do whatever you want with this stuff. If we meet some day, and you think
 * this stuff is worth it, you can buy me a beer in return Linus Mårtensson
 * ----------------------------------------------------------------------------
 */
#include "videoinput.h"

unsigned int videoinput::lockCounter = 0;
videoinput::dataStorage videoinput::m_DataStorage[VIDEOBUFFERS];
unsigned int *videoinput::lastBuffer = 0;