//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: vcd_class.cpp                         //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 12.05.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>

#include "vcd_class.h"

VCDClass::VCDClass()
{
    isOpen = false;
    file = NULL;

    akt_wire_count = 0;
}

VCDClass::~VCDClass()
{
    if(isOpen)
    {
        Close();
    }

    for(int i=0; i<akt_wire_count; i++)
    {
        delete[] wire_names[i];
    }
}

bool VCDClass::Open(const char *filename)
{
    if(isOpen) return false;

    file = fopen(filename, "wt");
    if (file == NULL)
        return false;

    isOpen = true;
    Counter = 0;

    WriteHeader();

    return true;
}

void VCDClass::Close()
{
    if (!isOpen) return;
    fclose(file);
}

void VCDClass::SetVersionString(const char *version)
{
    this->version = version;
}

void VCDClass::SetTimeScaleString(const char *timescale)
{
    this->timescale = timescale;
}

void VCDClass::SetModuleString(const char *module)
{
    this->module = module;
}

void VCDClass::AddWire(const char *name, bool value)
{
    if(akt_wire_count < (MAX_WIRES-1))
    {
        wire_names[akt_wire_count] = new char[strlen(name)+1];
        strcpy(wire_names[akt_wire_count],name);
        wire_values[akt_wire_count] = value;
        akt_wire_count++;
    }
}

void VCDClass::SetWire(int num, bool value)
{
    static char line[256];

    if(num <= akt_wire_count && (isOpen))
    {
        if(wire_values[num] != value)
        {
            sprintf(line,"#%d\n",Counter);
            fwrite(line,sizeof(char),strlen(line),file);

            wire_values[num] = value;
            if(value)
            {
                sprintf(line,"1<%.3d\n",num);
                fwrite(line,sizeof(char),strlen(line),file);
            }
            else
            {
                sprintf(line,"0<%.3d\n",num);
                fwrite(line,sizeof(char),strlen(line),file);
            }
        }
    }
}

void VCDClass::NextStep()
{
    if(isOpen)
        Counter++;
}

void VCDClass::WriteHeader()
{
    char line[256];

    if(isOpen)
    {
        // Version schreiben
        sprintf(line,"$version %s $end\n",version);
        fwrite(line,sizeof(char),strlen(line),file);

        // Time Scale schreiben
        sprintf(line,"$timescale %s $end\n",timescale);
        fwrite(line,sizeof(char),strlen(line),file);

        // Scope Module Top schreiben
        sprintf(line,"$scope module %s $end\n",module);
        fwrite(line,sizeof(char),strlen(line),file);


        for(int i=0; i<akt_wire_count; i++)
        {
            sprintf(line,"$var wire 1 <%.3d %s $end\n",i,wire_names[i]);
            fwrite(line,sizeof(char),strlen(line),file);
        }

        // upscope definition schreiben
        sprintf(line,"$upscope $end\n");
        fwrite(line,sizeof(char),strlen(line),file);

        // upscope definition schreiben
        sprintf(line,"$enddefinitions $end\n");
        fwrite(line,sizeof(char),strlen(line),file);

        // upscope definition schreiben
        sprintf(line,"#%d\n",Counter);
        fwrite(line,sizeof(char),strlen(line),file);
    }
}
