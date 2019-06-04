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
// Letzte Änderung am 04.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef COMMAND_LINE_CLASS_H
#define COMMAND_LINE_CLASS_H

#include <cstdio>
#include <vector>
#include <QObject>

#define MAX_COMMAND_NUM 512
#define CMD_ARG 0xFFFF

using namespace std;

struct CMD_STRUCT
{
    const uint16_t cmd_command;     // CMD_COMMAND
    const char *short_command;      // Kurzbezeichnung (bsp.: -a)
    const char *long_command;       // Langbezeichnung (bsp.: --all)
    const char *legend;             // Erklärung als Kurztext
    const uint16_t arg_count;       // Anzahl der benötigten Argumente

};

class CommandLineClass : public QObject
{
    Q_OBJECT

public:
    CommandLineClass(int argc, char *argv[], const char* app_name, const CMD_STRUCT *command_list, int command_list_count);
    int GetCommandCount();
    int GetCommand(int number);
    const char *GetCommandLongString(int command);
    int GetCommandArgCount(int command);
    bool CheckArg(int number);
    char* GetArg(int number);
    int GetArgInt(int number, bool *err);
    bool FoundCommand(int command);
    void OutErrorMsg(const char *msg, const char *help_opt);
    void ShowHelp();

private:
    bool CheckShortCommands(const char *short_commands);
    bool CheckLongCommands(const char *long_command);
    void AddCommand(int command, char* arg = nullptr);
    void OutUnknowOptionError(const char *command, bool is_long);

    const CMD_STRUCT *all_commands_list;
    int all_commands_list_count;
    int max_long_command_lenght;

    const char* app_name;

    int command_count;
    int command_list[MAX_COMMAND_NUM];
    char *command_arg[MAX_COMMAND_NUM];
};

#endif // COMMAND_LINE_CLASS_H
