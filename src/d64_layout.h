//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: d64_layout.h                          //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

// Gemeinsames D64 Modell (Spurzahl, Sektortabellen, Fehlerinfo)
// Wird von Floppy1541 und D64Class benutzt.

#ifndef D64_LAYOUT_H
#define D64_LAYOUT_H

#include <cstdint>
#include <cstddef>

#define D64_IMAGE_SIZE 174848       // D64 mit 35 Spuren (ohne Fehlerinfo)
#define D64_IMAGE_SIZE_40 196608    // D64 mit 40 Spuren (ohne Fehlerinfo)
#define D64_IMAGE_SIZE_MAX 205312   // D64 mit 42 Spuren (802 Sektoren)
#define D64_MAX_SECTORS 802
#define D64_MAX_TRACKS 42

// Sektoren pro Spur (Index = Spur - 1)
static const uint8_t D64_NUM_SECTORS[D64_MAX_TRACKS] = {21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,21,19,19,19,19,19,19,19,18,18,18,18,18,18,17,17,17,17,17,17,17,17,17,17,17,17};

// Nummer des ersten Sektors einer Spur (Index = Spur, 1..42; Index 43-1 = Gesamtzahl bei 42 Spuren)
// Zeiger auf Track Anfang //	<------------------------- Track 1-17 ----------------------> <------ Track 18-24 ------> <---- Track 25-30 ----> <---- Track 31-42 ---->
static const uint16_t D64_TRACK_INDEX[D64_MAX_TRACKS + 2] = {0,0,21,42,63,84,105,126,147,168,189,210,231,252,273,294,315,336,357,376,395,414,433,452,471,490,508,526,544,562,580,598,615,632,649,666,683,700,717,734,751,768,785,802};

// Ermittelt Spurzahl (35, 40, 42) und Fehlerinfo aus der Dateigröße. false = zu klein für ein D64
static inline bool D64DetectLayout(long file_size, int *tracks, bool *has_error_map)
{
    if(file_size >= D64_IMAGE_SIZE_MAX)     { *tracks = 42; *has_error_map = (file_size >= D64_IMAGE_SIZE_MAX + 802); }
    else if(file_size >= D64_IMAGE_SIZE_40) { *tracks = 40; *has_error_map = (file_size >= D64_IMAGE_SIZE_40 + 768); }
    else if(file_size >= D64_IMAGE_SIZE)    { *tracks = 35; *has_error_map = (file_size >= D64_IMAGE_SIZE + 683); }
    else return false;
    return true;
}

#endif // D64_LAYOUT_H
