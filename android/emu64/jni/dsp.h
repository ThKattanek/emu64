/*
 * dsp.h
 *
 *  Created on: 10.11.2013
 *      Author: thorsten
 */

#ifndef DSP_H_
#define DSP_H_

#define SOUND_BUFFER_SIZE 882*2	// 2* wegen Stereo --> 50 mal in der SEK wird Callback aufgerufen

typedef void (*AndroidAudioCallback)(short *buffer, int num_samples);
bool OpenSLWrap_Init(AndroidAudioCallback cb);
void OpenSLWrap_Shutdown();

#endif /* DSP_H_ */
