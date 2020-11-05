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

#include "widget_file_browse.h"
#include "ui_widget_file_browse.h"

WidgetFileBrowse::WidgetFileBrowse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFileBrowse)
{
    ui->setupUi(this);

    tmp_path = nullptr;

    ui->delete_file->setDisabled(true);

    connect(ui->listView_filebrowser,SIGNAL(activated(QModelIndex)),this,SLOT(on_listView_filebrowser_clicked(QModelIndex)));

    ui->listWidget_zip->setMinimumHeight(0);
    ui->listWidget_zip->setMaximumHeight(0);

    // Schreibschutzoption defaultmäßig verbergen
    ui->WriteProtected->setVisible(false);

    dirmodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs | QDir::Files | QDir::AllEntries);
    dirmodel->setRootPath(QDir::rootPath());
    ui->listView_filebrowser->setModel(dirmodel);
    akt_fullpath = dirmodel->rootPath();
    ui->AktPath->setText(akt_fullpath);

    // Laufwerke in Liste eintragen
    QFileInfoList file_info_list = QDir::drives();

    for(int i=0; i < file_info_list.count(); i++)
    {
        ui->drive_list->addItem(file_info_list.at(i).absoluteFilePath());
    }
}

void WidgetFileBrowse::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

WidgetFileBrowse::~WidgetFileBrowse()
{
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

    QString lw = rootPathName(akt_dir);

    if(lw == "")
    {
        QDir dir(akt_dir);
        lw = dir.rootPath();
    }

    int idx = ui->drive_list->findText(lw);
    ui->drive_list->setCurrentIndex(idx);
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

QString WidgetFileBrowse::rootPathName(const QString &aPath)
{
    QRegExp reg("([A-Za-z]:[\\\\|/])");

    int nIndex = reg.indexIn(aPath);

    QString strRootPathName;

    if (nIndex > -1)
    {
        strRootPathName = reg.cap(1);
    }
    return strRootPathName;
}

void WidgetFileBrowse::on_drive_list_currentIndexChanged(const QString &arg1)
{
    SetAktDir(arg1);
}
