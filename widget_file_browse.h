//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_file_browse.h                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 29.10.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_FILE_BROWSE_H
#define WIDGET_FILE_BROWSE_H

#include <QWidget>
#include <QFileSystemModel>
#include <QMessageBox>

#include "quazip/quazip.h"
#include "quazip/quazipfile.h"

namespace Ui {
    class WidgetFileBrowse;
}

class WidgetFileBrowse : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetFileBrowse(QWidget *parent = 0);
    void RetranslateUi();
    void SetFileFilter(const QStringList & filters);
    QString GetAktDir(void);
    QString GetAktFile(void);
    void SetAktDir(QString akt_dir);
    void SetAktFile(QString akt_dir, QString akt_file);
    void RefreshAktDir(void);
    void EnableWriteProtectCheck(bool enabled);
    bool isFileWriteProtect(QString filename);
    ~WidgetFileBrowse();

signals:
    void FileDoubleClick(void);
    void WriteProtectedChanged(bool status);    // true = Schreibgeschützt

private slots:
    void on_listWidget_zip_itemSelectionChanged();
    void on_listView_filebrowser_doubleClicked(const QModelIndex &index);
    void on_to_parent_clicked();
    void on_listView_filebrowser_clicked(const QModelIndex &index);
    void on_WriteProtected_clicked(bool checked);

private:
    Ui::WidgetFileBrowse *ui;
    QFileSystemModel *dirmodel;
    QString akt_fullpath;
    QStringList ZIPExt;

signals:
    void select_file(QString filename);
};

#endif // WIDGET_FILE_BROWSE_H
