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
// Letzte Änderung am 02.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef WIDGET_FILE_BROWSE_H
#define WIDGET_FILE_BROWSE_H

#include <QWidget>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QSettings>
#include <QList>
#include <QTreeWidgetItem>

#ifdef ZIP_SUPPORT
#include <quazip.h>
#include <quazipfile.h>
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
    void SetIni(QSettings *ini, const QString &local_group = QString(), const QString &local_name = QString());
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
    void on_sidebar_itemClicked(QTreeWidgetItem *item, int column);
    void on_sidebar_customContextMenuRequested(const QPoint &pos);
    void on_listView_filebrowser_customContextMenuRequested(const QPoint &pos);
    void onSplitterMoved();
    void on_listView_filebrowser_activated(const QModelIndex &index);

private:
    enum SidebarRole { RolePath = Qt::UserRole, RoleFavorite = Qt::UserRole + 1 };
    enum FavoriteKind { FavNone = 0, FavGlobal = 1, FavLocal = 2 };

    void BuildSidebar();
    void SelectSidebarItem(const QString &path);
    void AddFavorite(const QString &path, bool local);
    void RemoveFavorite(const QString &path, bool local);
    void SaveFavorites(bool local);
    static void UpdateAllSidebars();
    static QStringList favorites;       // global
    QStringList local_favorites;        // nur dieses Fenster
    QString local_group;
    QString local_name;
    static QSettings *ini;
    static QList<WidgetFileBrowse*> instances;
    static int sidebar_width;
    bool ignore_splitter_moved;

    QString tmp_path;

    Ui::WidgetFileBrowse *ui;
    QFileSystemModel *dirmodel;
    QString akt_fullpath;
    QStringList ZIPExt;
};

#endif // WIDGET_FILE_BROWSE_H
