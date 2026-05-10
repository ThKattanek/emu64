//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: resid_wrapper.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

// reSID Wrapper für die ReSID Library von Dag Lem <
// resid muss kommplett in /src/resid/ liegen, so wie auf github zu finden:
// https://github.com/libsidplayfp/resid
// oder als Mirror auf: https://github.com/ThKattanek/resid
// Für Emu64 müssen an den Original Code von ReSID keine Anpassungen vorgenommen werden, dafür ist dieser Wrapper da, damit die ReSID Library in Emu64 eingebunden werden kann.

#include "resid_wrapper.h"

ReSIDWrapperClass::ReSIDWrapperClass(int number, int samplerate, int buffersize, int *error)
{
    sid = new reSID::SID();

    sid_number = number;

    this->samplerate = samplerate;
    c64_cycle_sek = 985248;
    freq_conv_add_value = 1.0f/(c64_cycle_sek/samplerate);
    freq_conv_counter = 0.0;

    sound_output_enable = false;
    cycle_exact = false;

    io_dump = new SIDDumpClass((unsigned char*)(sid->read_state().sid_register));
    io_dump->WriteReg = &write_register;

    cycle_counter = 0;
    sound_buffer_pos = 0;
    sound_buffer_size = buffersize;
    sound_buffer = new short[sound_buffer_size];
    sound_buffer_v0 = new short[sound_buffer_size];
    sound_buffer_v1 = new short[sound_buffer_size];
    sound_buffer_v2 = new short[sound_buffer_size];

    pot_x = pot_y = 0xff;

    recording = false;
    rec_sample_counter = 0;

    sid_model = reSID::MOS6581;
    sid->set_chip_model(reSID::MOS6581);
    sid->adjust_filter_bias(0.0f);

    reset = nullptr;
    sid->reset();

    *error = 0;
}

ReSIDWrapperClass::~ReSIDWrapperClass()
{
    if(sid != nullptr)
    {
        delete sid;
        sid = nullptr;
    }
}

void ReSIDWrapperClass::SetClockFrequency(float clock_freq)
{
    c64_cycle_sek = clock_freq;
    sid->set_sampling_parameters(clock_freq, reSID::SAMPLE_INTERPOLATE, samplerate);
    freq_conv_add_value = 1.0f/(c64_cycle_sek/samplerate);
}

void ReSIDWrapperClass::SetChipModel(int model)
{
    sid_model = model;
    switch(model)
    {
    case 0:
        sid->set_chip_model(reSID::MOS6581);
        break;

    case 1:
        sid->set_chip_model(reSID::MOS8580);
        break;
    }
}

void ReSIDWrapperClass::EnableFilter(bool enable)
{
    sid->enable_filter(enable);
}

void ReSIDWrapperClass::Reset()
{
    sid->reset();
}

unsigned char ReSIDWrapperClass::ReadRegister(unsigned char offset)
{
    switch(offset & 0x1f)
    {
    case 0x19:
        return pot_x;
    case 0x1a:
        return pot_y;
    default:
        return sid->read(offset & 0x1f);
        break;
    }
}

void ReSIDWrapperClass::WriteRegister(unsigned char offset, unsigned char value)
{
    sid->write(offset & 0x1f, value);
}

void ReSIDWrapperClass::SetPotXY(unsigned char pot_x, unsigned char pot_y)
{
    this->pot_x = pot_x;
    this->pot_y = pot_y;
}

bool ReSIDWrapperClass::OneCycle()
{
    int ret = false;

    if(reset != nullptr)
        if(!*reset) sid->reset();

    if(cycle_exact)
    {
        sid->clock();
    }

    cycle_counter++;
    freq_conv_counter += freq_conv_add_value;
    if(freq_conv_counter >= 1.0f)
    {
        freq_conv_counter -= 1.0f;
        if(!cycle_exact)
        {
            /*
            OscZyklus(Zyklencounter);
            EnvZyklus(Zyklencounter);
            FilterZyklus(Zyklencounter,VoiceOutput(0),VoiceOutput(1),VoiceOutput(2),0);
            */
            sid->clock(cycle_counter);
        }
        cycle_counter = 0;

        if(sound_output_enable)
        {
            if(sound_buffer_pos < sound_buffer_size)
            {
                /*
                SoundBufferV0[SoundBufferPos] = VoiceOutput(0);
                SoundBufferV1[SoundBufferPos] = VoiceOutput(1);
                SoundBufferV2[SoundBufferPos] = VoiceOutput(2);
                */
                sound_buffer[sound_buffer_pos++] = sid->output() << 1;
            }
        }
        else
        {
            if(sound_buffer_pos < sound_buffer_size)
            {
                /*
                SoundBufferV0[SoundBufferPos] = VoiceOutput(0);
                SoundBufferV1[SoundBufferPos] = VoiceOutput(1);
                SoundBufferV2[SoundBufferPos] = VoiceOutput(2);
                */
                sound_buffer[sound_buffer_pos++] = 0;
            }
        }
        ret = true;
    }
    return ret;
}

void ReSIDWrapperClass::SetSoundBufferPosToZero()
{
    sound_buffer_pos = 0;
}
