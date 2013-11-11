/*
 * dsp.h
 *
 *  Created on: 10.11.2013
 *      Author: thorsten
 */

#ifndef DSP_H_
#define DSP_H_

typedef void (*AndroidAudioCallback)(short *buffer, int num_samples);
bool OpenSLWrap_Init(AndroidAudioCallback cb);
void OpenSLWrap_Shutdown();

#endif /* DSP_H_ */
