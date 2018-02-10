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
// Letzte Änderung am 09.02.2018                //
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
                else
                {
                    AddCommand(CMD_ARG,argv[i]);
                }
            }
            else
            {
                AddCommand(CMD_ARG,argv[i]);
            }
        } 
    }

    // Prüfen auf Richtige Anzahl von Argumenten

    for(int i=0; i<command_count; i++)
    {
        // Command suchen (sind immer keine Argumente)
        if(!CheckArg(i))
        {
            int arg_count = 0;
            for(int j=i+1; j<command_count; j++)
            {
                if(CheckArg(j))
                {
                    arg_count++;
                }
                else
                {
                    break;
                }
            }

            if(arg_count != GetCommandArgCount(this->command_list[i]))
            {
               // printf("%s: Ungültige Option -- %s\n",app_name,command);

                printf("%s: Ungültige Anzahl der Argumente von -- \"%s\"\n",app_name, this->command_arg[i]);
                command_count = -1;
                break;
            }
        }
        if(command_count == -1)
            break;
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

const char *CommandLineClass::GetCommandLongString(int command)
{
    for(int i=0; i<all_commands_list_count; i++)
    {
        if(all_commands_list[i].cmdCommand == command)
        {
            return all_commands_list[i].longCommand;
            break;
        }
    }
    return "";
}

bool CommandLineClass::CheckArg(int number)
{
    if(number < command_count)
    {
        if(command_list[number] == CMD_ARG)
            return true;
        else
            return false;
    }
    else return false;
}

char *CommandLineClass::GetArg(int number)
{
    if(number < command_count)
    {
        return command_arg[number];
    }

    return NULL;
}

int CommandLineClass::GetArgInt(int number, bool *err)
{
    *err = true;
    int z = 0;

    if(number < command_count)
    {
        char *arg = command_arg[number];
        char *endzgr;

        if(strlen(arg) > 2)
        {
            // prüfen auf HEX
            if(arg[0] == '0' && arg[1] == 'x')
            {
                z = strtol(arg+2, &endzgr, 16);

                if((strlen(arg) + arg) == endzgr)
                    *err = false;
            }
            else
            {
                z = strtol(arg, &endzgr, 10);

                if((strlen(arg) + arg) == endzgr)
                    *err = false;
            }
        }
        else
        {
            z = strtol(arg, &endzgr, 10);

            if((strlen(arg) + arg) == endzgr)
                *err = false;
        }

        if(*err)
            printf("%s: Ein Argument konnte nicht in eine Zahl umgewnadelt werden! -- %s\n",app_name,arg);
    }
    else
    {
        // number befindet sich auserhalb des Puffer
    }
    return z;
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
                char *str = new char[3];
                sprintf(str,"%s", all_commands_list[j].shortCommand);
                AddCommand(all_commands_list[j].cmdCommand, str);
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
            char *str = new char[255];
            sprintf(str,"--%s", long_command);
            AddCommand(all_commands_list[i].cmdCommand, str);
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

int CommandLineClass::GetCommandArgCount(int command)
{
    for(int i=0; i<all_commands_list_count; i++)
    {
        if(all_commands_list[i].cmdCommand == command)
        {
            return all_commands_list[i].arg_count;
            break;
        }
    }

    return -1;
}

void CommandLineClass::AddCommand(int command, char *arg)
{
    if(command_count >= 0 && command_count < MAX_COMMANDS)
    {
        command_list[command_count] = command;
        command_arg[command_count] = arg;
        command_count++;
    }
}

bool CommandLineClass::FoundCommand(int command)
{
    bool found = false;

    for(int i=0; i<command_count; i++)
    {
        if(command_list[i] == command)
        {
            found = true;
            break;
        }
    }

    return found;
}

void CommandLineClass::OutErrorMsg(const char *msg, const char *help_opt)
{
    printf("%s: %s\n",app_name,msg);
    printf("\"%s %s\" liefert weitere Informationen.\n",app_name, help_opt);
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
