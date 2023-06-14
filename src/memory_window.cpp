//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: memory_window.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 14.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "memory_window.h"
#include "qdebug.h"
#include "ui_memory_window.h"

MemoryWindow::MemoryWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MemoryWindow),
    AktViewAdresse(0),
    NoFocusRun(true)
{
    c64 = 0;

    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

    ui->MemoryTable->setColumnCount(1);
    ui->MemoryTable->setRowCount(MemZeilenAnz);

    MemScrDest << "RAM" << "KERNAL" << "BASIC" << "VIC" << "FARBRAM" << "SID" << "CIA1" << "CIA2" << "IO1" << "IO2" << "CHARROM" << "ROM-LO" << "ROM-HI" << "ROM-HI" << "ADR.OPEN";
    FloppyMemScrDest << "RAM" << "VIA1" << "VIA2" << "ROM" << "OPEN";

    for(int i=0;i<256;++i)
    {
        MapReadSource[i] = FMV_OPEN;
        MapWriteDestination[i] = FMV_OPEN;
    }

    for(int i=0;i<8;i++)
    {
        MapReadSource[i] = FMV_RAM;
        MapWriteDestination[i] = FMV_RAM;
    }
    MapReadSource[0x18] = FMV_VIA1;
    MapWriteDestination[0x18] = FMV_VIA1;

    MapReadSource[0x1C] = FMV_VIA2;
    MapWriteDestination[0x1C] = FMV_VIA2;

    for(int i=0;i<64;i++)
    {
        MapReadSource[i+0xC0] = FMV_DOS;
        MapWriteDestination[i+0xC0] = FMV_OPEN;
    }

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/lucon.ttf");
    QFont font1("Lucida Console",9);

    for(int i=0;i<MemZeilenAnz;i++)
    {
        WidgetMemoryZeile *w = new WidgetMemoryZeile(&font1, this);
        ui->MemoryTable->setCellWidget(i,0,w);
        ui->MemoryTable->setRowHeight(i,w->height()+1);
        ui->MemoryTable->setColumnWidth(0,w->width());
        connect(w,SIGNAL(ChangeValue(unsigned short,unsigned char)),this,SLOT(onChangeValue(unsigned short,unsigned char)));
        connect(this,SIGNAL(NoFocus()),w,SLOT(onNoFocus()));
    }

    WidgetMemoryZeile *w = (WidgetMemoryZeile*)ui->MemoryTable->cellWidget(0,0);
    w->setEnabled(true);

    ui->MemoryTable->setMinimumWidth(w->width());
    ui->MemoryTable->setMaximumWidth(w->width());
    ui->MemoryTable->setMinimumHeight((w->height()+1)*17);
}

MemoryWindow::~MemoryWindow()
{
    delete ui;
}

void MemoryWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
    ChangeSource(AktSource);
}

void MemoryWindow::SetC64Pointer(C64Class *_c64)
{
    c64 = _c64;
}

void MemoryWindow::ChangeSource(int source)
{
    AktSource = source;
    if(AktSource > 0)
    {
        AktFloppyNr = AktSource - 1;
        ui->OnlyRam->setEnabled(false);
    }
    else ui->OnlyRam->setEnabled(true);

    switch(AktSource)
    {
    case 0:
        setWindowTitle(QString(tr("C64 Speicher")));
        break;
    case 1:
        setWindowTitle(QString(tr("Floppy #08 Speicher")));
        break;
    case 2:
        setWindowTitle(QString(tr("Floppy #09Speicher")));
        break;
    case 3:
        setWindowTitle(QString(tr("Floppy #10 Speicher")));
        break;
    case 4:
        setWindowTitle(QString(tr("Floppy #11 Speicher")));
        break;
    }
}

void MemoryWindow::onChangeValue(unsigned short adresse, unsigned char wert)
{
    if(AktSource > 0)
    {
        c64->floppy[AktFloppyNr]->WriteByte(adresse,wert);
    }
    else
    {
        c64->WriteC64Byte(adresse,wert);
    }
}

void MemoryWindow::UpdateMemoryList(void)
{
    if(c64 == 0) return;
    if(isHidden()) return;

    WidgetMemoryZeile *w;
    unsigned char puffer[16];
    unsigned char* ram_puffer;

    for(int i=0;i<MemZeilenAnz-1;i++)
    {
        if(AktSource > 0)
        {
            for(int x=0;x<16;x++) puffer[x] = c64->floppy[AktFloppyNr]->ReadByte(AktViewAdresse + (i*16) + x);
        }
        else
        {
            if(ui->OnlyRam->isChecked())
            {
                ram_puffer = c64->GetRAMPointer(AktViewAdresse + (i*16));
                for(int x=0;x<16;x++) puffer[x] = ram_puffer[x];
            }
            else for(int x=0;x<16;x++) puffer[x] = c64->ReadC64Byte(AktViewAdresse + (i*16) + x);
        }
        w = (WidgetMemoryZeile*)ui->MemoryTable->cellWidget(i+1,0);

        unsigned char page = (AktViewAdresse + (i*16)) >> 8;
        unsigned char mem_read_source;
        unsigned char mem_write_destination;

        if(AktSource > 0)
        {
            mem_read_source = MapReadSource[page];
            mem_write_destination = MapWriteDestination[page];
            w->Fill(AktViewAdresse + (i*16),puffer,FloppyMemScrDest[mem_read_source],FloppyMemScrDest[mem_write_destination]);
        }
        else
        {
            mem_read_source = c64->GetMapReadSource(page);
            mem_write_destination = c64->GetMapWriteDestination(page);
            w->Fill(AktViewAdresse + (i*16),puffer,MemScrDest[mem_read_source],MemScrDest[mem_write_destination]);
        }
    }
}

void MemoryWindow::on_MemoryScroll_valueChanged(int value)
{
    unsigned short v = value / 16;
    AktViewAdresse = v * 16;
    if(NoFocusRun) emit NoFocus();
    UpdateMemoryList();
}

void MemoryWindow::on_MemoryScroll_sliderPressed()
{
    emit NoFocus();
    NoFocusRun = false;
}

void MemoryWindow::on_MemoryScroll_sliderReleased()
{
    NoFocusRun = true;
}

void MemoryWindow::on_OnlyRam_clicked(bool)
{
    UpdateMemoryList();
}
