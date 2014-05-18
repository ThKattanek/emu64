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
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_FILE_BROWSE_H
#define WIDGET_FILE_BROWSE_H

#include <QWidget>
#include <QFileSystemModel>

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
    void SetFileFilter(const QStringList & filters);
    QString GetAktDir(void);
    QString GetAktFile(void);
    void SetAktDir(QString akt_dir);
    void SetAktFile(QString akt_dir, QString akt_file);
    ~WidgetFileBrowse();

private slots:
    void on_treeView_dir_clicked(QModelIndex index);
    void onlistView_files_currentChanged(const QModelIndex &current, const QModelIndex &previous);
    void on_listWidget_zip_itemSelectionChanged();

private:
    Ui::WidgetFileBrowse *ui;
    QFileSystemModel *dirmodel;
    QFileSystemModel *filemodel;
    QStringList ZIPExt;

signals:
    void select_file(QString filename);
};

#endif // WIDGET_FILE_BROWSE_H
