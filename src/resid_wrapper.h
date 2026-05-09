#ifndef RESID_WRAPPER_H
#define RESID_WRAPPER_H

#include "siddump.h"
#include "./resid/sid.h"

class ReSIDWrapperClass
{
public:
    ReSIDWrapperClass(int number, int samplerate,int buffersize,int *error);
    ~ReSIDWrapperClass();

    void SetClockFrequency(float clock_freq);
    void SetChipModel(int model);
    void Reset();
    unsigned char ReadRegister(unsigned char offset);
    void WriteRegister(unsigned char offset, unsigned char value);
    bool OneCycle();

    void SetSoundBufferPosToZero();

    short *sound_buffer;
    short *sound_buffer_v0;
    short *sound_buffer_v1;
    short *sound_buffer_v2;
    int sound_buffer_pos;
    int sound_buffer_size;
    bool cycle_exact;
    bool filter_on;
    bool sound_output_enable;
    bool *reset;

    SIDDumpClass *io_dump;
    unsigned char	write_register;

    /// Recording ///
    bool recording;
    int rec_sample_counter;
    short rec_sample_buffer[19656];

private:
    reSID::SID *sid;

    int	sid_number;
    int sid_model;

    float samplerate;

    int c64_cycle_sek;
    float freq_conv_counter;	// interner Counter für Fast Fast Fast Resampling ;-)
    float freq_conv_add_value;
    int cycle_counter;

    unsigned char pot_x;
    unsigned char pot_y;
};

#endif // RESID_WRAPPER_H
