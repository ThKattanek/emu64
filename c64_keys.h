//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_keys.h                            //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef C64_KEYS_H
#define C64_KEYS_H

#include "SDL2/SDL_keycode.h"

struct C64_KEYS
{
    unsigned char MatrixCode;
    int           SDLKeyCode;
    bool          Shift;
};

#define C64KeyNum 70

C64_KEYS C64KeyTable[C64KeyNum]={
                                            // C64 Keys //
    {0x00,SDLK_BACKSPACE,false},            /// DEL
    {0x01,SDLK_RETURN,false},               /// RETURN
    {0x02,SDLK_RIGHT,false},                /// CRSR_R
    {0x03,SDLK_F7,false},                   /// F7
    {0x04,SDLK_F1,false},                   /// F5
    {0x05,SDLK_F3,false},                   /// F3
    {0x06,SDLK_F5,false},                   /// F1
    {0x07,SDLK_DOWN,false},                 /// CRSR_D

    {0x10,SDLK_3,false},                    /// 3
    {0x11,SDLK_w,false},                    /// W
    {0x12,SDLK_a,false},                    /// A
    {0x13,SDLK_4,false},                    /// 4
    {0x14,SDLK_z,false},                    /// Z
    {0x15,SDLK_s,false},                    /// S
    {0x16,SDLK_e,false},                    /// E
    {0x17,SDLK_LSHIFT,false},               /// SHIFT_LINKS

    {0x20,SDLK_5,false},                    /// 5
    {0x21,SDLK_r,false},                    /// R
    {0x22,SDLK_d,false},                    /// D
    {0x23,SDLK_6,false},                    /// 6
    {0x24,SDLK_c,false},                    /// C
    {0x25,SDLK_f,false},                    /// F
    {0x26,SDLK_t,false},                    /// T
    {0x27,SDLK_x,false},                    /// X

    {0x30,SDLK_7,false},                    /// 7
    {0x31,SDLK_y,false},                    /// Y
    {0x32,SDLK_g,false},                    /// G
    {0x33,SDLK_8,false},                    /// 8
    {0x34,SDLK_b,false},                    /// B
    {0x35,SDLK_h,false},                    /// H
    {0x36,SDLK_u,false},                    /// U
    {0x37,SDLK_v,false},                    /// V

    {0x40,SDLK_9,false},                    /// 9
    {0x41,SDLK_i,false},                    /// I
    {0x42,SDLK_j,false},                    /// J
    {0x43,SDLK_0,false},                    /// 0
    {0x44,SDLK_m,false},                    /// M
    {0x45,SDLK_k,false},                    /// K
    {0x46,SDLK_o,false},                    /// O
    {0x47,SDLK_n,false},                    /// N

    {0x50,SDLK_KP_PLUS,false},              /// +
    {0x51,SDLK_p,false},                    /// P
    {0x52,SDLK_l,false},                    /// L
    {0x53,SDLK_KP_MINUS,false},             /// -
    {0x54,SDLK_PERIOD,false},               /// .

    //{0x55,SDLK_WORLD_86,false},             /// :

    //{0x56,SDLK_WORLD_92,false},             /// @

    {0x57,SDLK_COMMA,false},                /// ,

    //{0x60,SDLK_WORLD_63,false},             /// ENGL. PFUND

    {0x61,SDLK_KP_MULTIPLY,false},          /// *

    //{0x62,SDLK_WORLD_68,false},             /// ;
    {0x62,SDLK_SEMICOLON,false},

    //{0x63,SDLK_COMPOSE,false},              /// HOME

    {0x64,SDLK_RSHIFT,false},               /// SHIF_RECHTS
    {0x65,SDLK_HASH,false},                 /// =
    {0x66,SDLK_PLUS,false},                 /// PFEIL HOCH
    {0x67,SDLK_MINUS,false},                /// SLASH (/)

    {0x70,SDLK_1,false},                    /// 1
    {0x71,SDLK_CARET,false},                /// PFEIL LINKS
    {0x72,SDLK_LCTRL,false},                /// CONTOL
    {0x73,SDLK_2,false},                    /// 2
    {0x74,SDLK_SPACE,false},                /// SPACE
    {0x75,SDLK_LESS,false},                 /// COMMODORE
    {0x76,SDLK_q,false},                    /// Q
    {0x77,SDLK_TAB,false},                  /// RUN/STOP

    {0x02,SDLK_LEFT,true},                  /// CRSR_L
    {0x03,SDLK_F8,true},                    /// F8
    {0x04,SDLK_F2,true},                    /// F2
    {0x05,SDLK_F4,true},                    /// F4
    {0x06,SDLK_F6,true},                    /// F6
    {0x07,SDLK_UP,true},                    /// CRSR_U
};

#endif // C64_KEYS_H
