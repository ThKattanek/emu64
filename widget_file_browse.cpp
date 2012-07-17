//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: widget_file_browse.cpp                //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 28.08.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include "widget_file_browse.h"
#include "ui_widget_file_browse.h"

WidgetFileBrowse::WidgetFileBrowse(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetFileBrowse)
{
    ui->setupUi(this);

    ui->listWidget_zip->setMinimumHeight(0);
    ui->listWidget_zip->setMaximumHeight(0);

    dirmodel = new QFileSystemModel(this);
    dirmodel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);
    dirmodel->setRootPath(dirmodel->myComputer().toString());

    ui->treeView_dir->setModel(dirmodel);
    ui->treeView_dir->hideColumn(1);
    ui->treeView_dir->hideColumn(2);
    ui->treeView_dir->hideColumn(3);

    filemodel = new QFileSystemModel(this);
    filemodel->setFilter(QDir::NoDotAndDotDot | QDir::Files | QDir::Files);
    ui->listView_files->setModel(filemodel);

    connect(ui->listView_files->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(onlistView_files_currentChanged(QModelIndex,QModelIndex)));

    ui->treeView_dir->setCurrentIndex(dirmodel->index(qApp->applicationDirPath()));
    ui->listView_files->setRootIndex(filemodel->setRootPath(qApp->applicationDirPath()));
    ui->treeView_dir->scrollTo(ui->treeView_dir->currentIndex(),QAbstractItemView::PositionAtCenter);
}

WidgetFileBrowse::~WidgetFileBrowse()
{
    delete dirmodel;
    delete ui;
}

void WidgetFileBrowse::on_treeView_dir_clicked(QModelIndex index)
{
    QString sPath = dirmodel->fileInfo(index).absoluteFilePath();
    ui->listView_files->setRootIndex(filemodel->setRootPath(sPath));
}

void WidgetFileBrowse::onlistView_files_currentChanged(const QModelIndex &current, const QModelIndex &)
{
    QString AktFileName = filemodel->fileInfo(current).absoluteFilePath();

    if("ZIP" == AktFileName.right(3).toUpper())
    {
        ui->listWidget_zip->clear();
        ui->listWidget_zip->setMaximumHeight(120);
        ui->listWidget_zip->setMinimumHeight(120);

        QuaZip zip(AktFileName);
        if(zip.open(QuaZip::mdUnzip))
        {
            qDebug() << "zip is open";
            QStringList FileList;
            for(int i=0;i<ZIPExt.count();i++)
            {
                FileList << zip.getFileNameList().filter(ZIPExt[i].right(4),Qt::CaseInsensitive);
            }

            FileList.sort();
            ui->listWidget_zip->addItems(FileList);
            zip.close();
            qDebug() << "zip is closed";
        }
    }
    else
    {
        ui->listWidget_zip->setMaximumHeight(0);
        ui->listWidget_zip->setMinimumHeight(0);
    }
    emit select_file(filemodel->fileInfo(current).absoluteFilePath());
}

void WidgetFileBrowse::SetFileFilter(const QStringList &filters)
{
    ZIPExt = QStringList(filters);

    QStringList filters1(filters);
    filters1 << "*.zip";                        // ZIP Anhängen (Für Zip Unterstützung)
    filemodel->setNameFilterDisables(false);
    filemodel->setNameFilters(filters1);
}

QString WidgetFileBrowse::GetAktDir(void)
{
    return dirmodel->fileInfo(ui->treeView_dir->currentIndex()).absoluteFilePath();
}

QString WidgetFileBrowse::GetAktFile(void)
{
    return filemodel->fileName(ui->listView_files->currentIndex());
}

void WidgetFileBrowse::SetAktDir(QString akt_dir)
{
    if(akt_dir == "") return;

    QModelIndex idx = dirmodel->index(akt_dir);

    ui->treeView_dir->setCurrentIndex(dirmodel->index(akt_dir));
    ui->listView_files->setRootIndex(filemodel->setRootPath(akt_dir));
    ui->treeView_dir->scrollTo(ui->treeView_dir->currentIndex(),QAbstractItemView::PositionAtCenter);
}

void WidgetFileBrowse::SetAktFile(QString akt_dir, QString akt_file)
{
    if(akt_file == "") return;
    QModelIndex idx = filemodel->index(akt_dir + "/" + akt_file);
    ui->listView_files->setCurrentIndex(idx);
    ui->listView_files->scrollTo(idx,QAbstractItemView::PositionAtCenter);
    emit select_file(filemodel->fileInfo(idx).absoluteFilePath());
}

void WidgetFileBrowse::on_listWidget_zip_itemSelectionChanged()
{/*
    QString AktZIPName = filemodel->fileInfo(ui->listView_files->currentIndex()).absoluteFilePath();
    QString ZIPInFile = ui->listWidget_zip->currentItem()->text();
    QString OutFileName = QCoreApplication::applicationDirPath() + "/tmp/" + ZIPInFile;

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

        QFile outfile(OutFileName);
        if(outfile.open(QIODevice::ReadWrite))
        {
            outfile.write(zf.readAll());
            outfile.close();
        }
        zip.close();
        emit select_file(OutFileName);
    }
    */
}
