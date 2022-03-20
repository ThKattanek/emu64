//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: emu64_commands.h                      //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 19.03.2022                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef EMU64_COMMANDS_H
#define EMU64_COMMANDS_H

// Defineren aller Kommandozeilen Parameter
enum CMD_COMMAND {CMD_HELP,CMD_VERSION,CMD_MULTIPLE_INSTANCE,CMD_NOSPLASH,CMD_MINIMIZED,CMD_SOFTRESET,CMD_HARDRESET,\
                 CMD_MOUNT_DISK,CMD_MOUNT_CRT,CMD_UMOUNT_CRT,CMD_POKE_64,CMD_LIMITCYCLES,CMD_AUTOSTART,\
				 CMD_EXIT_SCREENSHOT,CMD_DEBUG_CART,CMD_WARP_MODE,CMD_RESET_INI,CMD_VIDEOCAPTURE,CMD_DATA_PATH,CMD_VIDEO_FILTER_OFF,CMD_SET_PALETTE,\
				 CMD_DOUBLE_TEXTURE_OFF, CMD_NOGUI};
static const CMD_STRUCT command_list[]{
    {CMD_AUTOSTART,"a","autostart","Laedt ein C64 Programm (prg,t00 ...) und startet es gleich.",1},
    {CMD_MOUNT_DISK,"m","mount-disk","Mountet ein Diskimage. --mount-disk <Laufwerksnummer (8-11)> <Dateiname>",2},
    {CMD_MOUNT_CRT,"","mount-crt","Steckt ein Modulimage in den Expansionport ein. --mount-crt <Dateiname>",1},
    {CMD_UMOUNT_CRT,"","umount-crt","Entfernt ein Modulimage aus dem Expansionsport. --umount-crt",0},
    {CMD_SOFTRESET,"r","soft-reset","Fuehrt einen Soft Reset durch.",0},
    {CMD_HARDRESET,"R","hard-reset","Fuehrt einen Hard Reset durch.",0},
    {CMD_WARP_MODE,"","warp","Aktiviert den WarpMode. (Maximum C64 Speed / kein Sound)",0},
    {CMD_POKE_64,"","poke64","Schreibt ein Byte an eine C64 Adresse. --poke64 <Adresse (0-65535)> <Wert (0-255)>",2},
    {CMD_LIMITCYCLES,"","limitcycles","Beendet den Emulator nach n Zyklen mit Exitcode 1. --limitcycles <nZyklen>",1},
    {CMD_NOSPLASH,"","nosplash","Der SplashScreen wird beim Start nicht angezeigt.",0},
	{CMD_NOGUI,"","nogui","Es werden keine Fenster angezeigt. Also Vorsicht, sollte nur mit --limitcycles verwendet werden.",0},
    {CMD_MINIMIZED,"","minimized","Emu64 wird mit mimimierten Fenstern gestartet.",0},
	{CMD_DOUBLE_TEXTURE_OFF,"","double-texture-off","Die Video Ausgabe Texture wird temporär nicht verdoppelt.",0},
	{CMD_VIDEO_FILTER_OFF,"","video-filter-off","Es wird die Video CRT Emulation temporär ausgeschaltet.",0},
	{CMD_SET_PALETTE,"","set-palette","Setzt eine von den internen C64 Farb Paletten. --set-palette <Palettennummer>\n\t\t\t\t 0=EMU64-NEU 1=EMU64-ALT 2=CCS64 3=PC64 4=C64S\n\t\t\t\t 5=VICE 6=FRODO 7=PEPTO 8=Colodore 9=SW",1},
    {CMD_EXIT_SCREENSHOT,"","exitscreenshot","Speichert vor dem beenden noch ein Screenshot als png. --exitscreenshot <Dateiname>",1},
    {CMD_DEBUG_CART,"","debugcart","Beendet den Emu64 bei einem Schreibzugriff auf $d7ff und gibt den Wert als Exitcode zurueck.",0},
    {CMD_MULTIPLE_INSTANCE,"","multiple-instance","Ermoeglicht das starten mehrerer Emu64 Instanzen.",0},
    {CMD_RESET_INI,"","reset-ini","Setzt die emu64.ini zurueck. (Alle bisherigen Einstellungen gehen verloren!)",0},
    {CMD_VIDEOCAPTURE,"","video-capture","Startet die Aufzeichnung des C64 Screens. --video-capture <Dateiname>",1},
    {CMD_DATA_PATH,"","data-path","Angabe eines Benutzerdefinierten Pfads für roms, floppysounds, gxf und languages. --data-path <Pfad>",1},
    {CMD_HELP,"?","help","Dieser Text.",0},
    {CMD_VERSION,"","version","Zeigt die aktuelle Versionsnummer an.",0}
};

#define command_list_count sizeof(command_list) / sizeof(command_list[0])

#endif // EMU64_COMMANDS_H
