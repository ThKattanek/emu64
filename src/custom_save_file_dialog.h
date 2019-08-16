# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: custom_save_file_dialog.h             //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 16.08.2019                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

#ifndef CUSTOM_SAVE_FILE_DIALOG_H
#define CUSTOM_SAVE_FILE_DIALOG_H

#include <QObject>
#include <QFileDialog>
#include <QMessageBox>

class CustomSaveFileDialog : public QObject
{
    Q_OBJECT
public:

static bool GetSaveFileName(QWidget *parent, QString caption, QStringList filters, QString *fileName, QString *fileExt);

};

#endif // CUSTOM_SAVE_FILE_DIALOG_H
