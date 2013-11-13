/*
 * c64_class.cpp
 *
 *  Created on: 11.11.2013
 *      Author: thorsten
 */

#include <c64_class.h>
#include <stddef.h>

#include <android/log.h>
#define  LOG_TAG    "NATIVE"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, __VA_ARGS__)

C64Class::C64Class(int SoundBufferSize)
{
	int ret_error;

	counter = 0;

    sid1 = new MOS6581_8085(0,44100,SoundBufferSize,&ret_error);
    sid1->RESET = &RESET;

    sid1->RESET = &RESET;
    sid1->SetC64Zyklen(985248);
    sid1->SetChipType(MOS_8580);
    sid1->SoundOutputEnable = true;
    sid1->CycleExact = false;
    sid1->FilterOn = false;
    sid1->Reset();

    RESET = true;
}

C64Class::~C64Class()
{
	if(sid1 != NULL)
	{
		delete sid1;
		sid1 = NULL;
	}
}

void C64Class::FillAudioBuffer(short *stream, int laenge)
{
	static unsigned short *puffer_li;
	static unsigned short *puffer_re;

	puffer_li = (unsigned short*) stream;
	puffer_re = puffer_li+1;

	sid1->SoundBufferPos = 0;

	while(sid1->SoundBufferPos < (laenge/2))
	{
		sid1->OneZyklus();
	}

	for(int i=0;i<(laenge/2);i++)
	{
		*puffer_li = sid1->SoundBuffer[i];	// li
		*puffer_re = sid1->SoundBuffer[i];	// re

		puffer_li += 2;
		puffer_re += 2;
	}
}
