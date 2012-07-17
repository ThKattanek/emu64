//////////////////////////////////////////////////
//						//
// Emu64                                        //
// von Thorsten Kattanek			//
//                                              //
// #file: floppy_window.cpp                     //
//						//
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek		//
//						//
// Letzte Änderung am 12.10.2011		//
// www.emu64.de					//
//						//
//////////////////////////////////////////////////

#include <QFontDatabase>

#include "floppy_window.h"
#include "ui_floppy_window.h"

#define MAX_D64_FILES 128

FloppyWindow::FloppyWindow(QWidget *parent, QSettings *_ini) :
    QDialog(parent),
    ui(new Ui::FloppyWindow),
    isOneShowed(false)
{
    ini = _ini;
    ui->setupUi(this);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/emu64.ttf");
    c64_font = new QFont("Emu64 D64 Directory",16);

    connect(ui->FileBrowser,SIGNAL(select_file(QString)),this,SLOT(OnSelectFile(QString)));
    ui->FileBrowser->SetFileFilter(QStringList()<<"*.d64"<<"*.g64");

    ui->D64Table->setRowCount(MAX_D64_FILES);
    ui->D64Table->setColumnCount(1);

    for(int i=0; i<MAX_D64_FILES; i++)
    {
        ui->D64Table->setRowHeight(i,8);
        WidgetD64File *w = new WidgetD64File(this);
        ui->D64Table->setCellWidget(i,0,w);
        ui->D64Table->setColumnWidth(0,w->width());
    }

    FileTypes = QStringList() << "DEL" << "SEQ" << "PRG" << "USR" << "REL" << "CBM" << "E00" << "E?C";
}

FloppyWindow::~FloppyWindow()
{
    ////////// Save to INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("FloppyWindow");
        if(isOneShowed) ini->setValue("Geometry",saveGeometry());
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
        ini->endGroup();

        char group_name[32];
        for(int i=0; i<FloppyAnzahl; i++)
        {
            sprintf(group_name,"Floppy1541_%2.2X",i+8);
            ini->beginGroup(group_name);

            AktDir[i] = ini->value("AktDir","").toString();
            AktFile[i] = ini->value("AktFile","").toString();
            AktFileName[i] = AktDir[i] + "/" + AktFile[i];
            d64[i].LoadD64(AktFileName[i].toAscii().data());

            ini->endGroup();
        }

        ui->FileBrowser->SetAktDir(AktDir[0]);
        ui->FileBrowser->SetAktFile(AktDir[0],AktFile[0]);
        RefreshD64Table();
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
            d64[FloppyNr].LoadD64(AktFileName[FloppyNr].toAscii().data());
            RefreshD64Table();
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
        RefreshD64Table();
    }
}

QString FloppyWindow::GetAktFilename(int floppynr)
{
    return AktFileName[floppynr];
}

QString FloppyWindow::GetAktD64Name(int floppynr)
{
    return d64[floppynr].D64Name;
}

void FloppyWindow::RefreshD64Table(void)
{
    QString filename;
    QString spur;
    QString sektor;
    QString adresse;
    QString size;
    QString typ;

    unsigned short Adresse;
    unsigned char Typ;
    unsigned char Spur;
    unsigned char Sektor;
    unsigned short Laenge;

    int floppy = ui->FloppySelect->currentIndex();
    int d64_files = d64[floppy].DateiAnzahl;

    WidgetD64File *w;

    for(int i=0; i<MAX_D64_FILES;i++)
    {
        if(ui->D64Table->isRowHidden(i)) break;
        ui->D64Table->setRowHidden(i,true);
    }
    for(int i=0; i<d64_files;i++)
    {
        if(MAX_D64_FILES == i) break;

        w = (WidgetD64File*)ui->D64Table->cellWidget(i,0);

        Adresse = d64[floppy].D64Files[i].Adresse;
        Typ = d64[floppy].D64Files[i].Typ;
        Spur = d64[floppy].D64Files[i].Track;
        Sektor = d64[floppy].D64Files[i].Sektor;
        Laenge = d64[floppy].D64Files[i].Laenge;

        if((Adresse == 0x0801) && ((Typ & 0x07) == 2)) w->SetRMode(0);
        else
        {
            if((Adresse < 0x0801) && ((Typ & 0x07) == 2)) w->SetRMode(1);
            else w->SetRMode(2);
        }
        filename = d64[floppy].D64Files[i].Name;
        filename = "   " + filename;
        if(Typ & 0x40) typ = FileTypes[Typ&0x07] + "<";
        else typ = FileTypes[Typ&0x07];
        if(Typ & 0x80) typ = "  " + typ;
        else typ = " *" + typ;


        char str01[16];
        sprintf(str01,"%2.2d",Sektor);
        sektor = str01;

        sprintf(str01,"%2.2d",Spur);
        spur = str01;

        sprintf(str01,"$%4.4X",Adresse);
        adresse = str01;

        sprintf(str01,"%d",Laenge);
        size = str01;
        size = " " + size;

        w->SetLabels(filename,spur,sektor,adresse,size,typ);
        ui->D64Table->setRowHidden(i,false);
    }
}

void FloppyWindow::on_D64Table_cellDoubleClicked(int row, int column)
{
    qDebug() << "Row: " << row << " -- Column:" << column;
}
