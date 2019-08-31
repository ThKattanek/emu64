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
// Letzte Änderung am 31.08.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_FILE_BROWSE_H
#define WIDGET_FILE_BROWSE_H

#include <QWidget>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QRegExp>

#ifdef ZIP_SUPPORT
#include "quazip/quazip.h"
#include "quazip/quazipfile.h"
#endif

namespace Ui {
    class WidgetFileBrowse;
}

class WidgetFileBrowse : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetFileBrowse(QWidget *parent = nullptr);
    void RetranslateUi();
    void SetFileFilter(const QStringList & filters);
    QString GetAktDir(void);
    QString GetAktFile(void);
    void SetTempDir(QString tmp_path);
    void SetAktDir(QString akt_dir);
    void SetAktFile(QString akt_dir, QString akt_file);
    void RefreshAktDir(void);
    void EnableWriteProtectCheck(bool enabled);
    bool isFileWriteProtect(QString filename);
    ~WidgetFileBrowse();

signals:
    void FileDoubleClick(void);
    void SelectFile(QString filename);
    void WriteProtectedChanged(bool status);    // true = Schreibgeschützt

private slots:
    #ifdef ZIP_SUPPORT
    void on_listWidget_zip_itemSelectionChanged();
    #endif
    void on_listView_filebrowser_doubleClicked(const QModelIndex &index);
    void on_to_parent_clicked();
    void on_listView_filebrowser_clicked(const QModelIndex &index);
    void on_WriteProtected_clicked(bool checked);
    void on_view_refresh_clicked();
    void on_delete_file_clicked();
    void on_to_home_clicked();
    void on_drive_list_currentIndexChanged(const QString &arg1);

private:
    QString rootPathName(const QString &aPath);

    QString tmp_path;

    Ui::WidgetFileBrowse *ui;
    QFileSystemModel *dirmodel;
    QString akt_fullpath;
    QStringList ZIPExt;
};

#endif // WIDGET_FILE_BROWSE_H
