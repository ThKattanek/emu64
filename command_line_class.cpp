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
            int len_arg = strlen(argv[i]);
            if(len_arg > 1)
            {
                if(argv[i][0] == '-' && argv[i][1] != '-')
                {
                    // Short Commands
                    if(!CheckShortCommands(argv[i]+1))
                    {
                        // Error
                        command_count = -1;
                        break;
                    }
                }
                else if(argv[i][0] == '-' && argv[i][1] == '-')
                {
                    // Long Command
                    if(!CheckLongCommands(argv[i]+2))
                    {
                        // Error
                        command_count = -1;
                        break;
                    }
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
            printf("%3s%s, --%-*s %s\n","-",all_commands_list[i].shortCommand,max_long_command_len + 2,all_commands_list[i].longCommand,all_commands_list[i].legend);
            break;
        case 1:     // nur long vorhanen
            printf("%5s --%-*s %s\n","",max_long_command_len + 2,all_commands_list[i].longCommand,all_commands_list[i].legend);
            break;
        case 2:     // nur short vorhanden
            printf("  -%-*s %s\n",max_long_command_len + 7,all_commands_list[i].shortCommand,all_commands_list[i].legend);
            break;
        case 3:     // nichts vorhanden
            break;
        default:
            break;
        }
    }

    printf("\n");
}

bool CommandLineClass::CheckShortCommands(const char *short_commands)
{
    int len = strlen(short_commands);
    for(int i=0; i<len; i++)
    {
        bool found = false;
        for(int j=0; j<all_commands_list_count; j++)
        {
            if(all_commands_list[j].shortCommand[0] == short_commands[i])
            {
                AddCommand(all_commands_list[j].cmdCommand);
                found = true;
            }
        }

        if(!found)
        {
            // Fehlermeldung ausgeben
            char str[2];
            str[0] = short_commands[i];
            str[1] = 0;
            OutUnknowOptionError(str,false);
            return false;
            break;
        }
    }
    return true;
}

bool CommandLineClass::CheckLongCommands(const char *long_command)
{
    bool found = false;
    for(int i=0; i<all_commands_list_count; i++)
    {
        if(strcmp(long_command, all_commands_list[i].longCommand) == 0)
        {
            AddCommand(all_commands_list[i].cmdCommand);
            found = true;
        }
    }

    if(!found)
    {
        // Fehlermeldung ausgeben
        OutUnknowOptionError(long_command,true);
        return false;
    }

    return true;
}

void CommandLineClass::AddCommand(int command)
{
    if(command_count >= 0)
    {
        bool found = false;
        for(int i=0; i<command_count; i++)
        {
            if(command_list[i] == command)
                found = true;
        }

        if(!found)
        {
            command_list[command_count] = command;
            command_count++;
        }
    }
}

void CommandLineClass::OutUnknowOptionError(const char *command, bool is_long)
{
    if(!is_long)
    {
        // Short Option
        printf("%s: Ungültige Option -- %s\n",app_name,command);
    }
    else
    {
        // Long Option
        printf("%s: Ungültige Option \"--%s\"\n",app_name,command);
    }
}
