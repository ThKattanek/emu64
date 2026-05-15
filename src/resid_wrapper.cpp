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
    c64_cycle_sek = 985248;             // Default Clockfrequenz eines C64 (PAL), wird aber später mit SetClockFrequency() überschrieben, damit die richtige Clockfrequenz verwendet wird, je nachdem ob es sich um einen PAL oder NTSC C64 handelt
    freq_conv_add_value = 1.0f/(c64_cycle_sek/samplerate);
    freq_conv_counter = 0.0;

    sound_output_enable = false;
    cycle_exact = false;

    cycle_counter = 0;
    sound_buffer_pos = 0;
    sound_buffer_size = buffersize;
    sound_buffer = new short[sound_buffer_size];
    sound_buffer_v0 = new short[sound_buffer_size];
    sound_buffer_v1 = new short[sound_buffer_size];
    sound_buffer_v2 = new short[sound_buffer_size];

    pot_x = pot_y = 0xff;

    sid_model = reSID::MOS6581;
    sid->set_chip_model(reSID::MOS6581);

    enable_digi_boost = false;

    // Filtereinstellungen so setzen, dass es nicht zu Übersteuerungen kommt

    // MOS6581
    passband_frequency[0] = 0.9*samplerate/2;
    filter_scale[0] = 0.97;
    filter_bias[0] = 0.5f;

    // MOS8580
    passband_frequency[1] = 0.9*samplerate/2;
    filter_scale[1] = 0.97;
    filter_bias[1] = 0.0;

    // MOS8580 mit DigiBoost
    passband_frequency[2] = 0.9*samplerate/2;
    filter_scale[2] = 0.97;
    filter_bias[2] = 0.00; // DigiBoost benötigt etwas mehr Filter Bias, damit es nicht zu Übersteuerungen kommt

    filter_settings_index = 0; // Standardmäßig die Filtereinstellungen für MOS6581 verwenden

    SetClockFrequency(c64_cycle_sek);

    io_dump = new SIDDumpClass((unsigned char*)io);
    io_dump->WriteReg = &write_register;

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

    if(sound_buffer != nullptr)
    {
        delete[] sound_buffer;
        sound_buffer = nullptr;
    }

    if(sound_buffer_v0 != nullptr)
    {
        delete[] sound_buffer_v0;
        sound_buffer_v0 = nullptr;
    }

    if(sound_buffer_v1 != nullptr)
    {
        delete[] sound_buffer_v1;
        sound_buffer_v1 = nullptr;
    }

    if(sound_buffer_v2 != nullptr)
    {
        delete[] sound_buffer_v2;
        sound_buffer_v2 = nullptr;
    }

    if(io_dump != nullptr)
    {
        delete io_dump;
        io_dump = nullptr;
    }
}

void ReSIDWrapperClass::SetClockFrequency(float clock_freq)
{
    c64_cycle_sek = clock_freq;
    freq_conv_add_value = 1.0f/(c64_cycle_sek/samplerate);
}

void ReSIDWrapperClass::SetChipModel(int model)
{
    sid_model = model;
    switch(model)
    {
    case 0:
        sid->set_chip_model(reSID::MOS6581);
        sid->set_voice_mask(0x07);
        sid->input(0);
        filter_settings_index = 0; // Filtereinstellungen für MOS6581 verwenden
        break;

    case 1:
        sid->set_chip_model(reSID::MOS8580);
        if(enable_digi_boost)
        {
            sid->set_voice_mask(0x0f);
            sid->input(-32768);
            filter_settings_index = 2; // Filtereinstellungen für MOS8580 mit DigiBoost verwenden
        }
        else
        {
            sid->set_voice_mask(0x07);
            sid->input(0);
            filter_settings_index = 1; // Filtereinstellungen für MOS8580 ohne DigiBoost verwenden
        }
        break;
    default:
            filter_settings_index = 0; // Standardmäßig die Filtereinstellungen für MOS6581 verwenden
        break;
    }

    UpdateFilterSettings();
}

void ReSIDWrapperClass::EnableFilter(bool enable)
{
    sid->enable_filter(enable);
}

void ReSIDWrapperClass::EnableDigiBoost(bool enable)
{
    enable_digi_boost = enable;

    if(enable && (sid_model == 1))
    {
        sid->set_voice_mask(0x0f);
        sid->input(-32768);
    }
    else
    {
        sid->set_voice_mask(0x07);
        sid->input(0);
    }
}

void ReSIDWrapperClass::Reset()
{
    sid->reset();
}

uint8_t ReSIDWrapperClass::ReadRegister(uint8_t offset)
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

void ReSIDWrapperClass::WriteRegister(uint8_t offset, uint8_t value)
{
    io[offset & 0x1f] = value;
    write_register = offset & 0x1f;
    sid->write(offset & 0x1f, value);
}

void ReSIDWrapperClass::SetPotXY(uint8_t pot_x, uint8_t pot_y)
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
            sid->clock(cycle_counter);
        }
        cycle_counter = 0;

        if(sound_output_enable)
        {
            if(sound_buffer_pos < sound_buffer_size)
            {
                sound_buffer_v0[sound_buffer_pos] = sid->voice[0].output();
                sound_buffer_v1[sound_buffer_pos] = sid->voice[1].output();
                sound_buffer_v2[sound_buffer_pos] = sid->voice[2].output();

                int output = sid->output() << 1;

                // Clipping, damit es nicht zu Übersteuer
                if(output > 32767) output = 32767;
                if(output < -32768) output = -32768;

                sound_buffer[sound_buffer_pos++] = output;
            }
        }
        else
        {
            if(sound_buffer_pos < sound_buffer_size)
            {
                sound_buffer_v0[sound_buffer_pos] = sid->voice[0].output();
                sound_buffer_v1[sound_buffer_pos] = sid->voice[1].output();
                sound_buffer_v2[sound_buffer_pos] = sid->voice[2].output();
                sound_buffer[sound_buffer_pos++] = 0;
            }
        }
        ret = true;
    }

    ///////// IO DUMP //////////
    ////////////////////////////

    // Capture
    io_dump->CycleTickCapture();
    // Playing
    if(io_dump->CycleTickPlay())
        WriteRegister(io_dump->RegOut, io_dump->RegWertOut);
    write_register = 0xff;

    return ret;
}

void ReSIDWrapperClass::SetSoundBufferPosToZero()
{
    sound_buffer_pos = 0;
}

void ReSIDWrapperClass::UpdateFilterSettings()
{
    sid->set_sampling_parameters(c64_cycle_sek, reSID::SAMPLE_RESAMPLE, samplerate, passband_frequency[filter_settings_index], filter_scale[filter_settings_index]);
    sid->adjust_filter_bias(filter_bias[filter_settings_index]);
}
