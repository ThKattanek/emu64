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

     sid->adjust_filter_bias(0.0f);

     Sid_model = reSID::MOS6581;
     sid->set_chip_model(reSID::MOS6581);

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
