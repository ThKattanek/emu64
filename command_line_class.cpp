//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: command_line_class.cpp                //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 25.02.2017                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "command_line_class.h"

CommandLineClass::CommandLineClass(int argc, char *argv[], const char *app_name, const CMD_STRUCT *command_list, int command_list_count)
{
    this->app_name = app_name;
    this->all_commands_list = command_list;
    this->all_commands_list_count = command_list_count;

    command_count = 0;
    if(argc > 1)
    {
        for(int i=1; i<argc; i++)
        {
            for(int j=0; j<command_list_count; j++)
            {
                if(strcmp(argv[i],command_list[j].shortCommand) == 0)
                {
                    this->command_list[command_count] = command_list[j].cmdCommand;
                    command_count++;
                    break;
                }

                if(strcmp(argv[i],command_list[j].longCommand) == 0)
                {
                    this->command_list[command_count] = command_list[j].cmdCommand;
                    if(strlen(command_list[j].longCommand) > max_long_command_len)
                        max_long_command_len = strlen(command_list[j].longCommand);
                    command_count++;
                    break;
                }
            }
        }
    }

    // Größe des längsten LongCommand ermitteln
    max_long_command_len = 0;
    for(int i=0; i<all_commands_list_count; i++)
    {
        if(strlen(all_commands_list[i].longCommand) > max_long_command_len)
            max_long_command_len = strlen(all_commands_list[i].longCommand);
    }
}

int CommandLineClass::GetCommandCount()
{
    return command_count;
}

int CommandLineClass::GetCommand(int number)
{
    if(number < command_count)
    {
        return command_list[number];
    }

    return -1;
}

void CommandLineClass::ShowHelp()
{
    printf("\nProgrammaufruf: %s [OPTIONS]\n\nFolgende Optionen stehen Ihnen zur Verfügung:\n\n", app_name);

    for(int i=0; i<all_commands_list_count; i++)
    {
        int out_mode = 0;
        if(strcmp(all_commands_list[i].shortCommand,"") == 0)
            out_mode |= 1;
        if(strcmp(all_commands_list[i].longCommand,"") == 0)
            out_mode |= 2;

        switch(out_mode)
        {
        case 0:     // short und long vorhanden
            printf("%4s, %-*s %s\n",all_commands_list[i].shortCommand,max_long_command_len + 3,all_commands_list[i].longCommand,all_commands_list[i].legend);
            break;
        case 1:     // nur long vorhanen
            printf("%5s %-*s %s\n","",max_long_command_len + 3,all_commands_list[i].longCommand,all_commands_list[i].legend);
            break;
        case 2:     // nur short vorhanden
            printf("  %-*s %s\n",max_long_command_len + 7,all_commands_list[i].shortCommand,all_commands_list[i].legend);
            break;
        case 3:     // nichts vorhanden
            break;
        default:
            break;
        }
    }

    printf("\n");
}
