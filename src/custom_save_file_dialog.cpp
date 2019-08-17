# //////////////////////////////////////////////////
# //                                              //
# // Emu64                                        //
# // von Thorsten Kattanek                        //
# //                                              //
# // #file: custom_save_file_dialog.cpp           //
# //                                              //
# // Dieser Sourcecode ist Copyright geschützt!   //
# // Geistiges Eigentum von Th.Kattanek           //
# //                                              //
# // Letzte Änderung am 17.08.2019                //
# // www.emu64.de                                 //
# //                                              //
# //////////////////////////////////////////////////

#include "custom_save_file_dialog.h"

#include <QDebug>

bool CustomSaveFileDialog::GetSaveFileName(QWidget *parent, QString caption, QStringList filters, QString *fileName, QString *fileExt)
{
    if (fileName == nullptr)
       return false;

    QFileDialog saveDialog(parent);
    saveDialog.setWindowTitle(caption);
    saveDialog.setAcceptMode(QFileDialog::AcceptSave);
    saveDialog.setOption(QFileDialog::DontConfirmOverwrite);

    saveDialog.setNameFilters(filters);
    saveDialog.selectFile(*fileName);

    *fileName = "";

    if (!saveDialog.exec())
       return false;      // User pressed "Cancel"

    QStringList fileList = saveDialog.selectedFiles();
    if (fileList.count() != 1)
       return false;      // Should not happen, just to be sure

    QString tmpFileName = fileList.at(0);
    QString extension;

    QFileInfo fileInfo(tmpFileName);

    // Get Selected Fileextension
    extension = saveDialog.selectedNameFilter();
    extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
    extension = extension.left(extension.indexOf(")"));
    extension = extension.simplified();

    // If the filter specifies more than one extension, choose the first one
    if (extension.indexOf(" ") != -1)
       extension = extension.left(extension.indexOf(" "));


    if (fileInfo.suffix().isEmpty())
    {
       // Add the suffix selected by the user
       tmpFileName = tmpFileName + QString(".") + extension;
       fileInfo.setFile(tmpFileName);
    }
    else
    {
        QString ext = tmpFileName;
        ext = ext.right(ext.size() - ext.indexOf(".") - 1);
        ext = ext.simplified();

        if(extension != ext)
            tmpFileName = tmpFileName + QString(".") + extension;

        fileInfo.setFile(tmpFileName);
    }

    // Does the file already exist?

    if (QFile::exists(tmpFileName))
    {
        extension = saveDialog.selectedNameFilter();
        extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
        extension = extension.left(extension.indexOf(")"));
        extension = extension.simplified();

       int result = QMessageBox::question(parent, QObject::tr("Überschreiben?"),
          QObject::tr("Soll die Datei \"%1\" überschrieben werden?").arg(fileInfo.fileName()),
          QMessageBox::Yes,
          QMessageBox::No | QMessageBox::Default,
          QMessageBox::Cancel | QMessageBox::Escape);
       if (result == QMessageBox::Cancel)
          return false;
       else if (result == QMessageBox::No) {
          // Next chance for the user to select a filename
          if (!GetSaveFileName(parent, caption, filters, &tmpFileName, &extension))
             // User decided to cancel, exit function here
             return false;
       // User clicked "Yes", so process the execution
       fileInfo.setFile(tmpFileName);
       }
    }

    *fileName = tmpFileName;
    *fileExt = extension;
    return true;
}
