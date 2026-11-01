//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: widget_file_browse.cpp                //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 13.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QDebug>
#include <QMenu>
#include <QStorageInfo>
#include <QStandardPaths>
#include <QSplitter>

#include "widget_file_browse.h"
#include "ui_widget_file_browse.h"

QStringList WidgetFileBrowse::favorites;
QSettings *WidgetFileBrowse::ini = nullptr;
QList<WidgetFileBrowse*> WidgetFileBrowse::instances;
int WidgetFileBrowse::sidebar_width = 120;

WidgetFileBrowse::WidgetFileBrowse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFileBrowse)
{
    ui->setupUi(this);

    ignore_splitter_moved = false;
    ui->verticalLayout->setContentsMargins(0,0,0,0);
    ui->splitter->setStretchFactor(0,0);
    ui->splitter->setStretchFactor(1,1);
    ui->splitter->setCollapsible(1,false);
    ui->listView_filebrowser->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->splitter, &QSplitter::splitterMoved, this, [this](int, int){ onSplitterMoved(); });
    instances.append(this);

    tmp_path = nullptr;

    ui->delete_file->setDisabled(true);

    ui->listWidget_zip->setMinimumHeight(0);
    ui->listWidget_zip->setMaximumHeight(0);

    // Schreibschutzoption defaultmäßig verbergen
    ui->WriteProtected->setVisible(false);

    dirmodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::AllEntries);
    dirmodel->setRootPath(QDir::rootPath());
    ui->listView_filebrowser->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->listView_filebrowser->setModel(dirmodel);
    akt_fullpath = dirmodel->rootPath();
    ui->AktPath->setText(akt_fullpath);

    // Signal für Mausklick auf Listenelemente verbinden
    connect(ui->listView_filebrowser,SIGNAL(activated(QModelIndex)),this,SLOT(on_listView_filebrowser_clicked(QModelIndex)));

    // Möglichkeit hinzugefügt, dass auch mit den Cursortasten durch die Liste navigiert werden kann und dabei die Datei ausgewählt wird
    connect(ui->listView_filebrowser->selectionModel(),
            &QItemSelectionModel::currentRowChanged,
            this,
            [this](const QModelIndex &current, const QModelIndex)
            {
                if (!current.isValid())
                    return;

                on_listView_filebrowser_clicked(current);
            });

    BuildSidebar();
    ui->splitter->setSizes(QList<int>() << sidebar_width << qMax(1, width() - sidebar_width));
}

void WidgetFileBrowse::RetranslateUi()
{
    ui->retranslateUi(this);
    BuildSidebar();
    this->update();
}

WidgetFileBrowse::~WidgetFileBrowse()
{
    instances.removeAll(this);
    delete dirmodel;
    delete ui;
}

void WidgetFileBrowse::SetFileFilter(const QStringList &filters)
{
    ZIPExt = QStringList(filters);

    QStringList filters1(filters);
#ifdef ZIP_SUPPORT
    filters1 << "*.zip";                        // ZIP Anhängen (Für Zip Unterstützung)
#endif
    dirmodel->setNameFilterDisables(false);
    dirmodel->setNameFilters(filters1);
}

QString WidgetFileBrowse::GetAktDir(void)
{
    return akt_fullpath;
}

QString WidgetFileBrowse::GetAktFile(void)
{
    return dirmodel->fileName(ui->listView_filebrowser->currentIndex());
}

void WidgetFileBrowse::SetTempDir(QString tmp_path)
{
    this->tmp_path = tmp_path;
}

void WidgetFileBrowse::SetAktDir(QString akt_dir)
{
    if(akt_dir == "") return;

    ui->listView_filebrowser->setRootIndex(dirmodel->setRootPath(akt_dir));
    akt_fullpath = dirmodel->rootPath();
    ui->AktPath->setText(akt_fullpath);

    SelectSidebarItem(akt_fullpath);
}

void WidgetFileBrowse::SetAktFile(QString akt_dir, QString akt_file)
{
    if(akt_file == "") return;

    ui->listView_filebrowser->setRootIndex(dirmodel->setRootPath(akt_dir));
    akt_fullpath = dirmodel->rootPath();
    ui->AktPath->setText(akt_fullpath);

    QModelIndex idx = dirmodel->index(akt_dir + "/" + akt_file);
    ui->listView_filebrowser->setCurrentIndex(idx);
    ui->listView_filebrowser->scrollTo(idx,QAbstractItemView::PositionAtCenter);

    on_listView_filebrowser_clicked(idx);

    // Prüfen ob Aktuell ausgewählte Datei ein Verzeichnis ist oder nicht
    // In Abhängigkeit davon den Delete Button setzen
    if(!dirmodel->fileInfo(ui->listView_filebrowser->currentIndex()).isDir())
        ui->delete_file->setDisabled(false);
    else
        ui->delete_file->setDisabled(true);


    emit SelectFile(dirmodel->fileInfo(idx).absoluteFilePath());
}

void WidgetFileBrowse::RefreshAktDir()
{
    if(akt_fullpath == "") return;

    dirmodel->setRootPath("");
    dirmodel->setRootPath(akt_fullpath);
}

void WidgetFileBrowse::EnableWriteProtectCheck(bool enabled)
{
    ui->WriteProtected->setVisible(enabled);
}

bool WidgetFileBrowse::isFileWriteProtect(QString filename)
{
    QFileInfo file(filename);
    QFile::Permissions permissions = file.permissions();

    return !permissions.testFlag(QFile::WriteOwner);
}

#ifdef ZIP_SUPPORT
void WidgetFileBrowse::on_listWidget_zip_itemSelectionChanged()
{
    QString AktZIPName = dirmodel->fileInfo(ui->listView_filebrowser->currentIndex()).absoluteFilePath();
    QString ZIPInFile = ui->listWidget_zip->currentItem()->text();

    QString file = ZIPInFile.section("/",-1,-1);    // Evt. enthaltenen Pfad abschneiden

    QString OutFileName = tmp_path + "/" + file;

    QuaZip zip(AktZIPName);
    if(zip.open(QuaZip::mdUnzip))
    {
        QuaZipFile zf(&zip);
        if(!zip.setCurrentFile(ZIPInFile))
        {
            qDebug() << "Fehler in ZIP-Datei: " << AktZIPName;
            return;
        }
        if(!zf.open(QIODevice::ReadOnly))
        {
            qDebug() << "Fehler beim Öffnen des ZIPFiles: " << ZIPInFile;
        }

        // Aus OutFileName alles entfernen mit '/'

        QFile outfile(OutFileName);
        if(outfile.open(QIODevice::ReadWrite))
        {
            outfile.write(zf.readAll());
            outfile.close();
        }
        zip.close();
        emit SelectFile(OutFileName);
    }
}
#endif

void WidgetFileBrowse::on_listView_filebrowser_doubleClicked(const QModelIndex &index)
{
    // Prüfe ob angeklicktes Element ein Verzeichnis ist
    if(dirmodel->fileInfo(index).isDir())
    {
        //Ja
        ui->listView_filebrowser->setRootIndex(index);
        akt_fullpath = dirmodel->filePath(index);
        ui->AktPath->setText(akt_fullpath);
    }
    else emit FileDoubleClick();
}

void WidgetFileBrowse::on_to_parent_clicked()
{
    akt_fullpath =  dirmodel->filePath(dirmodel->parent(dirmodel->index(akt_fullpath)));
    SetAktDir(akt_fullpath);
    ui->AktPath->setText(akt_fullpath);
}

void WidgetFileBrowse::on_listView_filebrowser_activated(const QModelIndex &index)
{
    qDebug() << "Activated: " << dirmodel->filePath(index);
}


void WidgetFileBrowse::on_listView_filebrowser_clicked(const QModelIndex &index)
{
    if(!dirmodel->fileInfo(index).isDir())
    {
        // Kein Verzeichnis
        QString AktFileName = dirmodel->filePath(index);

        ui->delete_file->setEnabled(true);

        emit SelectFile(AktFileName);

#ifdef ZIP_SUPPORT
        if("ZIP" == AktFileName.right(3).toUpper())
        {
            ui->listWidget_zip->clear();
            ui->listWidget_zip->setMaximumHeight(120);
            ui->listWidget_zip->setMinimumHeight(120);

            ui->WriteProtected->setChecked(true);
            ui->WriteProtected->setEnabled(false);

            if(ui->WriteProtected->isVisible())
                emit WriteProtectedChanged(true);

            QuaZip zip(AktFileName);
            if(zip.open(QuaZip::mdUnzip))
            {
                QStringList FileList;
                for(int i=0;i<ZIPExt.count();i++)
                {
                    FileList << zip.getFileNameList().filter(ZIPExt[i].right(4),Qt::CaseInsensitive);
                }

                FileList.sort();
                ui->listWidget_zip->addItems(FileList);
                zip.close();
            }

            ui->listWidget_zip->setCurrentRow(0);
        }
        else
#endif
        {
            ui->listWidget_zip->setMaximumHeight(0);
            ui->listWidget_zip->setMinimumHeight(0);
            ui->WriteProtected->setEnabled(true);

            QFileInfo file(AktFileName);
            QFile::Permissions permissions = file.permissions();

            bool wp = !permissions.testFlag(QFile::WriteOwner);

            ui->WriteProtected->setChecked(wp);

            if(ui->WriteProtected->isVisible())
            {
                emit WriteProtectedChanged(wp);
            }
        }
    }
    else
    {
        ui->delete_file->setEnabled(false);

        ui->listWidget_zip->setMaximumHeight(0);
        ui->listWidget_zip->setMinimumHeight(0);
    }
}

void WidgetFileBrowse::on_WriteProtected_clicked(bool checked)
{
    QString AktFileName = dirmodel->filePath(ui->listView_filebrowser->currentIndex());
    QFile file(AktFileName);
    QFile::Permissions permissions;

    if(checked)
    {
        file.setPermissions(QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther);

        permissions = QFileInfo(AktFileName).permissions();

        if(permissions.testFlag(QFile::WriteOwner))
        {
            QMessageBox::warning(this,tr("Fehler !"),tr("Leider konnte der Schreibschutz der Disk Image Datei nicht gesetzt werden.\nBitte überprüfen Sie ob Sie über genügend Rechte verfügen.\nDer Emu64 wird diese Datei dennoch nicht verändern !"));
        }
        emit WriteProtectedChanged(checked);
    }
    else
    {
        file.setPermissions(QFile::ReadOwner | QFile::ReadGroup | QFile::ReadOther | QFile::WriteOwner);

        permissions = QFileInfo(AktFileName).permissions();

        if(!permissions.testFlag(QFile::WriteOwner))
        {
            ui->WriteProtected->setChecked(true);
            QMessageBox::warning(this,tr("Fehler !"),tr("Leider konnte der Schreibschutz der Disk Image Datei nicht entfernt werden.\nBitte überprüfen Sie ob Sie über genügend Rechte verfügen.\n Auf das Disk Image kann momentan nicht geschrieben werden !"));
        }
        emit WriteProtectedChanged(ui->WriteProtected->isChecked());
    }
}

void WidgetFileBrowse::on_view_refresh_clicked()
{
    RefreshAktDir();
}

void WidgetFileBrowse::on_delete_file_clicked()
{
    QString file_path = dirmodel->fileInfo(ui->listView_filebrowser->currentIndex()).absoluteFilePath().toLocal8Bit();

    if(!QFile(file_path).exists()) return;

    if(QMessageBox::Yes == QMessageBox::question(this,tr("Löschen?"),tr("Möchten Sie diese Datei wirklich entfernen?\n\n") + "[" + file_path + "]",QMessageBox::Yes | QMessageBox::No))
    {
        QFile file(file_path);
        if(!file.remove())
        {
            QMessageBox::critical(this,tr("Fehler !"),tr("Die Datei konnte nicht gelöscht werden.\n\n") + file.errorString());
        }
    }
}

void WidgetFileBrowse::on_to_home_clicked()
{
    SetAktDir(QDir::homePath());
}

void WidgetFileBrowse::SetIni(QSettings *_ini, const QString &_local_group, const QString &_local_name)
{
    ini = _ini;
    if(ini == nullptr) return;

    local_group = _local_group;
    local_name = _local_name;
    local_favorites.clear();
    if(!local_group.isEmpty())
    {
        ini->beginGroup(local_group);
        local_favorites = ini->value("Favorites").toStringList();
        ini->endGroup();
    }

    ini->beginGroup("FileBrowser");
    favorites = ini->value("Favorites").toStringList();
    sidebar_width = ini->value("SidebarWidth", 120).toInt();
    ini->endGroup();

    UpdateAllSidebars();
}

void WidgetFileBrowse::UpdateAllSidebars()
{
    for(WidgetFileBrowse *w : instances)
    {
        w->BuildSidebar();
        w->ignore_splitter_moved = true;
        w->ui->splitter->setSizes(QList<int>() << sidebar_width << qMax(1, w->width() - sidebar_width));
        w->ignore_splitter_moved = false;
        w->SelectSidebarItem(w->akt_fullpath);
    }
}

void WidgetFileBrowse::onSplitterMoved()
{
    if(ignore_splitter_moved) return;

    sidebar_width = ui->splitter->sizes().value(0);
    if(ini != nullptr)
    {
        ini->beginGroup("FileBrowser");
        ini->setValue("SidebarWidth", sidebar_width);
        ini->endGroup();
    }

    for(WidgetFileBrowse *w : instances)
    {
        if(w == this) continue;
        w->ignore_splitter_moved = true;
        w->ui->splitter->setSizes(QList<int>() << sidebar_width << qMax(1, w->width() - sidebar_width));
        w->ignore_splitter_moved = false;
    }
}

void WidgetFileBrowse::BuildSidebar()
{
    QTreeWidget *tree = ui->sidebar;
    tree->clear();

    auto add_group = [tree](const QString &title) -> QTreeWidgetItem*
    {
        QTreeWidgetItem *g = new QTreeWidgetItem(tree, QStringList(title));
        QFont f = g->font(0);
        f.setBold(true);
        g->setFont(0, f);
        g->setFlags(Qt::ItemIsEnabled);
        return g;
    };

    auto add_entry = [](QTreeWidgetItem *parent, const QString &text, const QString &path, int favorite)
    {
        QTreeWidgetItem *i = new QTreeWidgetItem(parent, QStringList(text));
        i->setData(0, RolePath, path);
        i->setData(0, RoleFavorite, favorite);
        i->setToolTip(0, QDir::toNativeSeparators(path));
        if(favorite != FavNone && !QDir(path).exists()) i->setFlags(Qt::NoItemFlags);
    };

    // Favoriten
    QTreeWidgetItem *g_fav = add_group(tr("Favoriten (alle)"));
    for(const QString &f : favorites)
    {
        QString name = QFileInfo(f).fileName();
        if(name.isEmpty()) name = QDir::toNativeSeparators(f);
        add_entry(g_fav, name, f, FavGlobal);
    }

    // Lokale Favoriten (nur dieses Fenster)
    if(!local_group.isEmpty())
    {
        QTreeWidgetItem *g_local = add_group(local_name.isEmpty() ? tr("Favoriten (lokal)") : tr("Favoriten (%1)").arg(local_name));
        for(const QString &f : local_favorites)
        {
            QString name = QFileInfo(f).fileName();
            if(name.isEmpty()) name = QDir::toNativeSeparators(f);
            add_entry(g_local, name, f, FavLocal);
        }
    }

    // Orte
    QTreeWidgetItem *g_places = add_group(tr("Orte"));
    add_entry(g_places, tr("Homeverzeichnis"), QDir::homePath(), FavNone);

    const QList<QStandardPaths::StandardLocation> locs = {QStandardPaths::DesktopLocation, QStandardPaths::DocumentsLocation, QStandardPaths::DownloadLocation};
    for(QStandardPaths::StandardLocation loc : locs)
    {
        const QString p = QStandardPaths::writableLocation(loc);
        if(!p.isEmpty() && QDir(p).exists() && p != QDir::homePath())
            add_entry(g_places, QFileInfo(p).fileName(), p, FavNone);
    }

    // Laufwerke (inkl. Netzlaufwerke)
    QTreeWidgetItem *g_drives = add_group(tr("Laufwerke"));
    const QList<QStorageInfo> volumes = QStorageInfo::mountedVolumes();
    for(const QStorageInfo &v : volumes)
    {
        if(!v.isValid() || !v.isReady()) continue;

        const QString fs = QString::fromLatin1(v.fileSystemType());
        static const QStringList pseudo = {"tmpfs","devtmpfs","proc","sysfs","cgroup","cgroup2","squashfs","overlay","devpts","securityfs","pstore","bpf","tracefs","debugfs","configfs","fusectl","efivarfs","autofs","mqueue","hugetlbfs","binfmt_misc","ramfs","nsfs"};
        if(pseudo.contains(fs)) continue;

        const QString root = v.rootPath();
        if(root.startsWith("/boot") || root.startsWith("/snap") || root.startsWith("/var/lib")) continue;

        QString name = v.displayName();
        if(name.isEmpty()) name = QDir::toNativeSeparators(root);
        if(name != QDir::toNativeSeparators(root)) name += " (" + QDir::toNativeSeparators(root) + ")";
        add_entry(g_drives, name, root, FavNone);
    }

    tree->expandAll();
}

void WidgetFileBrowse::SelectSidebarItem(const QString &path)
{
    QTreeWidgetItem *best = nullptr;
    int best_len = -1;
    const QString p = QDir::cleanPath(path);

    for(int g = 0; g < ui->sidebar->topLevelItemCount(); g++)
    {
        QTreeWidgetItem *group = ui->sidebar->topLevelItem(g);
        for(int i = 0; i < group->childCount(); i++)
        {
            QTreeWidgetItem *it = group->child(i);
            const QString ip = QDir::cleanPath(it->data(0, RolePath).toString());
            if(ip.isEmpty()) continue;
            const bool match = (p == ip) || p.startsWith(ip.endsWith('/') ? ip : ip + "/");
            if(match && ip.length() > best_len)
            {
                best = it;
                best_len = ip.length();
            }
        }
    }

    ui->sidebar->blockSignals(true);
    ui->sidebar->setCurrentItem(best);
    ui->sidebar->blockSignals(false);
}

void WidgetFileBrowse::on_sidebar_itemClicked(QTreeWidgetItem *item, int)
{
    const QString path = item->data(0, RolePath).toString();
    if(path.isEmpty()) return;
    SetAktDir(path);
}

void WidgetFileBrowse::on_sidebar_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = ui->sidebar->itemAt(pos);
    if(item == nullptr || item->data(0, RoleFavorite).toInt() == FavNone) return;
    const bool local = item->data(0, RoleFavorite).toInt() == FavLocal;

    QMenu menu(this);
    QAction *remove = menu.addAction(tr("Aus Favoriten entfernen"));
    if(menu.exec(ui->sidebar->viewport()->mapToGlobal(pos)) == remove)
        RemoveFavorite(item->data(0, RolePath).toString(), local);
}

void WidgetFileBrowse::on_listView_filebrowser_customContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = ui->listView_filebrowser->indexAt(pos);

    // Ohne Auswahl: aktuelles Verzeichnis
    QString path = index.isValid() ? dirmodel->filePath(index) : akt_fullpath;
    if(!QFileInfo(path).isDir() || path.isEmpty()) return;

    const QString clean = QDir::cleanPath(path);
    QMenu menu(this);
    QAction *act_global = nullptr;
    QAction *act_local = nullptr;

    if(favorites.contains(clean))
        act_global = menu.addAction(tr("Aus Favoriten entfernen (alle Fenster)"));
    else
        act_global = menu.addAction(tr("Zu Favoriten hinzufügen (alle Fenster)"));

    if(!local_group.isEmpty())
    {
        if(local_favorites.contains(clean))
            act_local = menu.addAction(local_name.isEmpty() ? tr("Aus Favoriten entfernen (nur dieses Fenster)") : tr("Aus Favoriten entfernen (nur %1)").arg(local_name));
        else
            act_local = menu.addAction(local_name.isEmpty() ? tr("Zu Favoriten hinzufügen (nur dieses Fenster)") : tr("Zu Favoriten hinzufügen (nur %1)").arg(local_name));
    }

    QAction *sel = menu.exec(ui->listView_filebrowser->viewport()->mapToGlobal(pos));
    if(sel == nullptr) return;

    if(sel == act_global)
    {
        if(favorites.contains(clean)) RemoveFavorite(clean, false);
        else AddFavorite(clean, false);
    }
    else if(sel == act_local)
    {
        if(local_favorites.contains(clean)) RemoveFavorite(clean, true);
        else AddFavorite(clean, true);
    }
}

void WidgetFileBrowse::AddFavorite(const QString &path, bool local)
{
    QStringList &list = local ? local_favorites : favorites;
    if(list.contains(path)) return;
    list.append(path);
    SaveFavorites(local);
}

void WidgetFileBrowse::RemoveFavorite(const QString &path, bool local)
{
    (local ? local_favorites : favorites).removeAll(path);
    SaveFavorites(local);
}

void WidgetFileBrowse::SaveFavorites(bool local)
{
    if(ini != nullptr)
    {
        if(local)
        {
            ini->beginGroup(local_group);
            ini->setValue("Favorites", local_favorites);
            ini->endGroup();
        }
        else
        {
            ini->beginGroup("FileBrowser");
            ini->setValue("Favorites", favorites);
            ini->endGroup();
        }
    }

    if(local) BuildSidebar();
    else UpdateAllSidebars();
    SelectSidebarItem(akt_fullpath);
}
