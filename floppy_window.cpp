//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: floppy_window.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 26.07.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QFontDatabase>
#include <QDebug>
#include <QMenu>
#include <QFileDialog>
#include <QMessageBox>

#include "floppy_window.h"
#include "ui_floppy_window.h"

#define MAX_D64_FILES 128

FloppyWindow::FloppyWindow(QWidget *parent, QSettings *_ini, C64Class *c64, QString tmp_path) :
    QDialog(parent),
    ui(new Ui::FloppyWindow),
    isOneShowed(false)
{
    ini = _ini;
    ui->setupUi(this);

    this->c64 = c64;
    this->TmpPath = tmp_path;

    FileTypes = QStringList() << "DEL" << "SEQ" << "PRG" << "USR" << "REL" << "CBM" << "E00" << "E?C";

    green_led = new QIcon(":/grafik/green_led_7x7.png");
    yellow_led = new QIcon(":/grafik/yellow_led_7x7.png");
    red_led = new QIcon(":/grafik/red_led_7x7.png");

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/emu64.ttf");
    c64_font = new QFont("Emu64 D64 Directory",18); // Linux18
    c64_font->setStyleStrategy(QFont::PreferAntialias);
    c64_font->setBold(false);
    c64_font->setKerning(true);

    ui->DiskName->setFont(*c64_font);

    // Spalten für D64 Datei-Anzeige hinzufügen
    ui->D64FileTable->setColumnCount(7);
    ui->D64FileTable->setColumnWidth(0,20);
    ui->D64FileTable->setColumnWidth(1,140);
    ui->D64FileTable->setColumnWidth(2,30);
    ui->D64FileTable->setColumnWidth(3,30);
    ui->D64FileTable->setColumnWidth(4,55);
    ui->D64FileTable->setColumnWidth(5,40);
    ui->D64FileTable->setColumnWidth(6,40);

    connect(ui->FileBrowser,SIGNAL(select_file(QString)),this,SLOT(OnSelectFile(QString)));
    ui->FileBrowser->SetFileFilter(QStringList()<<"*.d64"<<"*.g64");

    // Kontextmenü erstellen
    CompatibleMMCFileName = false;
    ui->D64FileTable->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->D64FileTable, SIGNAL(customContextMenuRequested(QPoint)),SLOT(OnCustomMenuRequested(QPoint)));
}

FloppyWindow::~FloppyWindow()
{
    ////////// Save to INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("FloppyWindow");
        if(isOneShowed)
        {
            ini->setValue("Geometry",saveGeometry());
            ini->setValue("PrgExportMMCCompatible",CompatibleMMCFileName);
        }

        if(isHidden()) ini->setValue("Show",false);
        else ini->setValue("Show",true);
        ini->endGroup();

        char group_name[32];
        for(int i=0; i<FloppyAnzahl; i++)
        {
            sprintf(group_name,"Floppy1541_%2.2X",i+8);
            ini->beginGroup(group_name);
            ini->setValue("AktDir",AktDir[i]);
            ini->setValue("AktFile",AktFile[i]);
            ini->endGroup();
        }
    }
    ////////////////////////////////////
    delete ui;
}

void FloppyWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
}

void FloppyWindow::LoadIni()
{
    ////////// Load from INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("FloppyWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        CompatibleMMCFileName = ini->value("PrgExportMMCCompatible", true).toBool();
        ini->endGroup();

        char group_name[32];
        for(int i=0; i<FloppyAnzahl; i++)
        {
            sprintf(group_name,"Floppy1541_%2.2X",i+8);
            ini->beginGroup(group_name);

            AktDir[i] = ini->value("AktDir","").toString();
            AktFile[i] = ini->value("AktFile","").toString();
            AktFileName[i] = AktDir[i] + "/" + AktFile[i];
            d64[i].LoadD64(AktFileName[i].toLatin1().data());

            ini->endGroup();
        }

        ui->FileBrowser->SetAktDir(AktDir[0]);
        ui->FileBrowser->SetAktFile(AktDir[0],AktFile[0]);
        RefreshD64FileList();
    }
    ////////////////////////////////////
}

void FloppyWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void FloppyWindow::OnSelectFile(QString filename)
{
    int FloppyNr = ui->FloppySelect->currentIndex();

    if(FloppyNr < FloppyAnzahl)
    {
        AktFileName[FloppyNr] = filename;
        AktDir[FloppyNr] = ui->FileBrowser->GetAktDir();
        AktFile[FloppyNr] = ui->FileBrowser->GetAktFile();
        if("D64" == AktFileName[FloppyNr].right(3).toUpper())
        {
            d64[FloppyNr].LoadD64(AktFileName[FloppyNr].toLatin1().data());
            RefreshD64FileList();
            emit ChangeFloppyImage(FloppyNr);
        }
    }
}

void FloppyWindow::OnChangeFloppyNummer(int floppynr)
{
    show();
    ui->FloppySelect->setCurrentIndex(floppynr);
}

void FloppyWindow::OnRemoveImage(int floppynr)
{
    AktFile[floppynr] = "";
    AktFileName[floppynr] = AktDir[floppynr] + "/" + AktFile[floppynr];

    if(floppynr == ui->FloppySelect->currentIndex())
    {
        ui->FileBrowser->SetAktDir(AktDir[floppynr]);
        ui->FileBrowser->SetAktFile(AktDir[floppynr],AktFile[floppynr]);
    }
}

void FloppyWindow::on_FloppySelect_currentIndexChanged(int index)
{
    if(isOneShowed)
    {
        ui->FileBrowser->SetAktDir(AktDir[index]);
        ui->FileBrowser->SetAktFile(AktDir[index],AktFile[index]);
        RefreshD64FileList();
    }
}

void FloppyWindow::OnCustomMenuRequested(QPoint pos)
{
    QMenu *menu=new QMenu(this);
    menu->addAction(new QAction(trUtf8("Laden und Starten mit Reset ohne Kernal"), this));
    menu->addAction(new QAction(trUtf8("Laden und Starten mit Reset"), this));
    menu->addAction(new QAction(trUtf8("Laden und Starten"), this));
    menu->addAction(new QAction(trUtf8("Laden"), this));

    QMenu *menu_export = new QMenu(this);
    menu_export->setTitle("Export");
    menu_export->addAction(new QAction(trUtf8("Deitei exportieren"), this));
    menu_export->addAction(new QAction(trUtf8("Dateiname MMC/SD2IEC kompatibel"), this));
    menu_export->actions().at(1)->setCheckable(true);
    menu_export->actions().at(1)->setChecked(CompatibleMMCFileName);

    menu->addMenu(menu_export);

    // Oberster Eintrag hervorheben
    QFont font = menu->actions().at(0)->font();
    font.setBold(true);
    menu->actions().at(0)->setFont(font);

    menu->popup(ui->D64FileTable->viewport()->mapToGlobal(pos));
    connect(menu->actions().at(0),SIGNAL(triggered(bool)),this,SLOT(OnD64FileStart0(bool)));
    connect(menu->actions().at(1),SIGNAL(triggered(bool)),this,SLOT(OnD64FileStart1(bool)));
    connect(menu->actions().at(2),SIGNAL(triggered(bool)),this,SLOT(OnD64FileStart2(bool)));
    connect(menu->actions().at(3),SIGNAL(triggered(bool)),this,SLOT(OnD64FileStart3(bool)));
    connect(menu_export->actions().at(0),SIGNAL(triggered(bool)),this,SLOT(OnPRGExport(bool)));
    connect(menu_export->actions().at(1),SIGNAL(triggered(bool)),this,SLOT(OnPRGNameMMCKompatibel(bool)));
}

void FloppyWindow::OnD64FileStart0(bool)
{
    int file_index = ui->D64FileTable->currentIndex().row();
    int floppy_nr = ui->FloppySelect->currentIndex();
    QString FileName = TmpPath + "/tmp.prg";

    if(d64[floppy_nr].ExportPrg(file_index,FileName.toAscii().data()))
    {
        c64->LoadAutoRun(floppy_nr,FileName.toAscii().data());
    }
}

void FloppyWindow::OnD64FileStart1(bool)
{
    int file_index = ui->D64FileTable->currentIndex().row();
    int floppy_nr = ui->FloppySelect->currentIndex();
    c64->LoadPRGFromD64(floppy_nr,d64[floppy_nr].D64Files[file_index].Name,0);
}

void FloppyWindow::OnD64FileStart2(bool)
{
    int file_index = ui->D64FileTable->currentIndex().row();
    int floppy_nr = ui->FloppySelect->currentIndex();
    c64->LoadPRGFromD64(floppy_nr,d64[floppy_nr].D64Files[file_index].Name,1);
}

void FloppyWindow::OnD64FileStart3(bool)
{
    int file_index = ui->D64FileTable->currentIndex().row();
    int floppy_nr = ui->FloppySelect->currentIndex();
    c64->LoadPRGFromD64(floppy_nr,d64[floppy_nr].D64Files[file_index].Name,2);
}

void FloppyWindow::OnPRGExport(bool)
{
    int file_index = ui->D64FileTable->currentIndex().row();
    int floppy_nr = ui->FloppySelect->currentIndex();

    // Alle nichterlaubten Zeichen in einem Dateinamen entfernen.
    char c64_filename[17];
    for (int i=0;i<17;i++)
    {
       c64_filename[i] = d64[floppy_nr].D64Files[file_index].Name[i];
       if(c64_filename[i]=='/') c64_filename[i]=' ';
       if(c64_filename[i]=='\\') c64_filename[i]=' ';
       if(c64_filename[i]==':') c64_filename[i]=' ';
       if(c64_filename[i]=='<') c64_filename[i]=' ';
       if(c64_filename[i]=='>') c64_filename[i]=' ';
       if(c64_filename[i]=='.') c64_filename[i]=' ';
    }
    c64_filename[16] = 0;

    QString filename = QString(c64_filename);
    QString fileext = FileTypes[d64[floppy_nr].D64Files[file_index].Typ & 7];

    if(CompatibleMMCFileName)
    {
        filename = filename.toLower();
        fileext = fileext.toLower();
    }

    if(c64 == NULL) return;

    if(!getSaveFileName(this,trUtf8("C64 Datei Exportieren"),trUtf8("C64 Programmdatei ") + "(*.prg *.seq *.usr *.rel *.cbm *.e00 *.del);;" + trUtf8("Alle Dateien ") + "(*.*)",&filename,&fileext))
        return;

    if(filename != "")
    {
        if(!d64[floppy_nr].ExportPrg(file_index,filename.toAscii().data()))
        {
            QMessageBox::critical(this,"C64 Datei Export","Fehler beim exportieren der C64 Datei.\nDie Datei konnte nicht exortiert.");
        }
    }
}

void FloppyWindow::OnPRGNameMMCKompatibel(bool)
{
    if(CompatibleMMCFileName)
        CompatibleMMCFileName = false;
    else CompatibleMMCFileName = true;
}

QString FloppyWindow::GetAktFilename(int floppynr)
{
    return AktFileName[floppynr];
}

QString FloppyWindow::GetAktD64Name(int floppynr)
{
    return d64[floppynr].D64Name;
}

void FloppyWindow::RefreshD64FileList()
{
    // D64 File-Anzeige löschen
    ui->D64FileTable->clear();

    // Aktuell ausgewählte Floppy
    int floppy = ui->FloppySelect->currentIndex();
    // Anzahl der Files
    int d64_files = d64[floppy].DateiAnzahl;

    // D64 Name ermitteln und anzeigen
    ui->DiskName->setText(d64[floppy].D64Name);

    // Headerlabels setzen
    ui->D64FileTable->setHorizontalHeaderLabels(QStringList() << "" << "DATEI" << "SP" << "SK" << "ADR" << "SIZE" << "TYP");

    // Anzahl der Zeilen setzen
    ui->D64FileTable->setRowCount(d64_files);
    for(int i=0; i<d64_files;i++)
    {
        // Icon in Abhängigkeit der Startbarkeit setzen
        QTableWidgetItem *icon_item = new QTableWidgetItem;
        if((d64[floppy].D64Files[i].Adresse==0x0801) && ((d64[floppy].D64Files[i].Typ & 7)==(2))) icon_item->setIcon(*green_led);
        else
        {
            if((d64[floppy].D64Files[i].Adresse<0x0801) && ((d64[floppy].D64Files[i].Typ & 7)==(2))) icon_item->setIcon(*yellow_led);
            else icon_item->setIcon(*red_led);
        }
        ui->D64FileTable->setItem(i,0,icon_item);

        // Dateiname setzen
        ui->D64FileTable->setCellWidget(i,1,new QLabel(d64[floppy].D64Files[i].Name));
        ui->D64FileTable->cellWidget(i,1)->setFont(*c64_font);
        QLabel* label = (QLabel*) ui->D64FileTable->cellWidget(i,1);
        label->setAlignment(Qt::AlignTop);
        label->setStyleSheet("color: rgb(100, 100, 255);");

        // Spur setzen
        ui->D64FileTable->setCellWidget(i,2,new QLabel(QVariant(d64[floppy].D64Files[i].Track).toString() + " "));
        ui->D64FileTable->cellWidget(i,2)->setFont(*c64_font);
        label = (QLabel*) ui->D64FileTable->cellWidget(i,2);
        label->setAlignment(Qt::AlignTop | Qt::AlignRight);
        label->setStyleSheet("color: rgb(100, 100, 255);");

        // Sektor setzen
        ui->D64FileTable->setCellWidget(i,3,new QLabel(QVariant(d64[floppy].D64Files[i].Sektor).toString() + " "));
        ui->D64FileTable->cellWidget(i,3)->setFont(*c64_font);
        label = (QLabel*) ui->D64FileTable->cellWidget(i,3);
        label->setAlignment(Qt::AlignTop | Qt::AlignRight);
        label->setStyleSheet("color: rgb(100, 100, 255);");

        // Adresse setzen
        QString str;
        str.sprintf(" $%04X",d64[floppy].D64Files[i].Adresse);
        ui->D64FileTable->setCellWidget(i,4,new QLabel(str));
        ui->D64FileTable->cellWidget(i,4)->setFont(*c64_font);
        label = (QLabel*) ui->D64FileTable->cellWidget(i,4);
        label->setAlignment(Qt::AlignTop);
        label->setStyleSheet("color: rgb(100, 100, 255);");

        // Size setzen
        ui->D64FileTable->setCellWidget(i,5,new QLabel(QVariant(d64[floppy].D64Files[i].Laenge).toString() + " "));
        ui->D64FileTable->cellWidget(i,5)->setFont(*c64_font);
        label = (QLabel*) ui->D64FileTable->cellWidget(i,5);
        label->setAlignment(Qt::AlignTop | Qt::AlignRight);
        label->setStyleSheet("color: rgb(100, 100, 255);");

        // Typ setzen
        QString strTyp;
        int FileTyp = d64[floppy].D64Files[i].Typ;

        if(FileTyp & 0x40) strTyp = FileTypes[FileTyp & 0x07] + "<";
        else strTyp = FileTypes[FileTyp & 0x07];
        if(FileTyp & 0x80) strTyp = " " + strTyp;
        else strTyp = "*" + strTyp;

        ui->D64FileTable->setCellWidget(i,6,new QLabel(strTyp));
        ui->D64FileTable->cellWidget(i,6)->setFont(*c64_font);
        label = (QLabel*) ui->D64FileTable->cellWidget(i,6);
        label->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
        label->setStyleSheet("color: rgb(100, 100, 255);");

        // Höhe der Zeile setzen
#ifdef _WIN32
    ui->D64FileTable->setRowHeight(i,9);
    ui->D64FileTable->cellWidget(i,1)->setFixedHeight(10);
#endif

#ifdef __linux__
    ui->D64FileTable->setRowHeight(i,8);
    ui->D64FileTable->cellWidget(i,1)->setFixedHeight(9);
#endif

        if(!(FileTyp & 0x80) && !ui->ViewSplatFiles->isChecked())
        {
            ui->D64FileTable->setRowHeight(i,0);
        }
    }
}

void FloppyWindow::on_ViewSplatFiles_clicked()
{
    RefreshD64FileList();
}

void FloppyWindow::on_D64FileTable_cellDoubleClicked(int , int )
{
    OnD64FileStart0(0);
}

bool FloppyWindow::getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt)
{
   if (fileName == NULL)      // "parent" is allowed to be NULL!
      return false;

   QFileDialog saveDialog(parent);
   saveDialog.setWindowTitle(caption);
   saveDialog.setAcceptMode(QFileDialog::AcceptSave);
   saveDialog.setConfirmOverwrite(false);
   saveDialog.setFilter(filter);
   saveDialog.selectFile(*fileName);
   saveDialog.setDefaultSuffix(*fileExt);
   saveDialog.setOptions(QFileDialog::DontUseNativeDialog);

   *fileName = "";

   if (!saveDialog.exec())
      return false;      // User pressed "Cancel"

   QStringList fileList = saveDialog.selectedFiles();
   if (fileList.count() != 1)
      return false;      // Should not happen, just to be sure

   QString tmpFileName = fileList.at(0);
   QString extension;

   QFileInfo fileInfo(tmpFileName);
   if (fileInfo.suffix().isEmpty()) {
      // Add the suffix selected by the user

      extension = saveDialog.selectedFilter();
      extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
      extension = extension.left(extension.indexOf(")"));
      extension = extension.simplified();

      // If the filter specifies more than one extension, choose the first one
      if (extension.indexOf(" ") != -1)
         extension = extension.left(extension.indexOf(" "));

      tmpFileName = tmpFileName + QString(".") + extension;
      fileInfo.setFile(tmpFileName);
   }

   *fileName = tmpFileName;
   *fileExt = extension;
   return true;
}
