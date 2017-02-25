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
enum CMD_COMMAND {CMD_HELP,CMD_VERSION};
static const CMD_STRUCT command_list[]{
    {CMD_HELP,"-h","--help","Dieser Text."},
    {CMD_HELP,"-?","","Dieser Text."},
    {CMD_VERSION,"-v","--version","Zeigt die aktuelle Versionsnummer an."}
};

#define command_list_count sizeof(command_list) / sizeof(command_list[0])

#endif // EMU64_COMMANDS_H
