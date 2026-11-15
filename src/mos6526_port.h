//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: mos6526_port.h                        //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 15.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef MOS_6526_PORT_H
#define MOS_6526_PORT_H

#include <stdint.h>
#include "state_stream.h"

class PORT
{
public:
    PORT();
    ~PORT();
    uint8_t GetInput();
    void SetInput(uint8_t value);
    uint8_t GetOutput();
    void SetOutput(uint8_t value);
    uint8_t* GetOutputBitsPointer();
    uint8_t* GetInputBitsPointer();
    void SaveState(StateWriter &w) const { w.U8(input_bits); w.U8(output_bits); }
    bool LoadState(StateReader &r) { input_bits = r.U8(); output_bits = r.U8(); return r.Ok(); }
private:
    uint8_t input_bits;
    uint8_t output_bits;
};
#endif // MOS_6526_PORT_H
