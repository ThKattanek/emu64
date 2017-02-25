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
// Letzte Änderung am 25.02.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef EMU64_COMMANDS_H
#define EMU64_COMMANDS_H

// Defineren aller Kommandozeilen Parameter
enum CMD_COMMAND {CMD_HELP,CMD_VERSION,CMD_MULTIPLE_INSTANCE,CMD_NOSPLASH};
static const CMD_STRUCT command_list[]{
    {CMD_NOSPLASH,"","nosplash","Der SplashScreen wird beim Start nicht angezeigt."},
    {CMD_MULTIPLE_INSTANCE,"","multiple-instance","Ermöglicht das starten mehrerer Emu64 Instanzen."},
    {CMD_HELP,"?","help","Dieser Text."},
    {CMD_VERSION,"","version","Zeigt die aktuelle Versionsnummer an."}
};

#define command_list_count sizeof(command_list) / sizeof(command_list[0])

#endif // EMU64_COMMANDS_H
