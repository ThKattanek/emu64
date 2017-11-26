//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: vcd_class.h                           //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 12.05.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef VCD_CLASS_H
#define VCD_CLASS_H

#define MAX_WIRES 256

class VCDClass
{
public:
    VCDClass();
    ~VCDClass();

    ///
    /// \brief Erstellt und öffnet das VCD Ausgabefile.
    /// \param filename Der Dateiname des VCD Files
    /// \return true = wenn alles ok, false = ein Fehler ist aufgetreten
    ///
    bool Open(const char *filename);

    ///
    /// \brief Schließt das VCD Ausgabefile
    ///
    void Close();

    ///
    /// \brief SetVersionString
    /// \param version
    ///
    void SetVersionString(const char *version);
    void SetTimeScaleString(const char *timescale);
    void SetModuleString(const char *module);

    void AddWire(const char *name, bool value);
    void SetWire(int num, bool value);
    void NextStep();

private:
    void WriteHeader();

    int akt_wire_count;
    bool wire_values[MAX_WIRES];
    char *wire_names[MAX_WIRES];

    FILE *file;
    bool isOpen;

    const char *version;
    const char *timescale;
    const char *module;

    int Counter;
};

#endif // VCD_CLASS_H
