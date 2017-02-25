//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: command_line_class.h                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 25.02.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef COMMAND_LINE_CLASS_H
#define COMMAND_LINE_CLASS_H

#include <cstdio>
#include <QObject>

#define MAX_COMMANDS 255

struct CMD_STRUCT
{
    int cmdCommand;                   // CMD_COMMAND
    const char *shortCommand;         // Kurzbezeichnung (bsp.: -a)
    const char *longCommand;          // Langbezeichnung (bsp.: --all)
    const char *legend;               // Erklärung als Kurztext
};

class CommandLineClass : public QObject
{
    Q_OBJECT

public:
    CommandLineClass(int argc, char *argv[], const char* app_name, const CMD_STRUCT *command_list, int command_list_count);
    int GetCommandCount();
    int GetCommand(int number);
    bool FoundCommand(int command);
    void ShowHelp();

private:
    bool CheckShortCommands(const char *short_commands);
    bool CheckLongCommands(const char *long_command);
    void AddCommand(int command);
    void OutUnknowOptionError(const char *command, bool is_long);
    const CMD_STRUCT *all_commands_list;
    int all_commands_list_count;
    unsigned int max_long_command_len;

    const char* app_name;

    int command_count;
    int command_list[MAX_COMMANDS];
    bool command_value[MAX_COMMANDS];
};

#endif // COMMAND_LINE_CLASS_H
