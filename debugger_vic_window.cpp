//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: debugger_vic_window.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 09.01.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "debugger_vic_window.h"
#include "ui_debugger_vic_window.h"

DebuggerVicWindow::DebuggerVicWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebuggerVicWindow)
{
    c64 = 0;
    ui->setupUi(this);

    ui->OutputList->setColumnWidth(0,110);
    ui->OutputList->setColumnWidth(1,90);

    GrafikModi = QStringList() << "Standard Text" << "Multicolor Text" << "Standard Bitmap" << "Multicolor Bitmap" << "ECM Text" << "Ungültiger Textmodus" << "Ungültiger Bitmapmodus 1" << "Ungültiger Bitmapmodus 2";
}

DebuggerVicWindow::~DebuggerVicWindow()
{
    delete ui;
}

void DebuggerVicWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void DebuggerVicWindow::SetC64Pointer(C64Class *_c64)
{
    c64 = _c64;
}

void DebuggerVicWindow::UpdateOutputList()
{
    if(c64 == 0) return;
    if(isHidden()) return;

    VIC_STRUCT vs;
    c64->GetVicReg(&vs);

    char str00[1024];
    sprintf(str00,"$%3.3X (%d)",vs.AktRasterzeile,vs.AktRasterzeile);
    ui->OutputList->topLevelItem(0)->setText(1,str00);
    sprintf(str00,"$%3.3X (%d)",vs.RasterLatch,vs.RasterLatch);
    ui->OutputList->topLevelItem(1)->setText(1,str00);
    sprintf(str00,"%d",vs.AktZyklus);
    ui->OutputList->topLevelItem(2)->setText(1,str00);
    if(vs.DisplayStatus) ui->OutputList->topLevelItem(3)->setText(1,"Display Mode");
    else ui->OutputList->topLevelItem(3)->setText(1,"Idle Mode");
    ui->OutputList->topLevelItem(4)->setText(1,GrafikModi[vs.GrafikMode & 7]);
    sprintf(str00,"Nr.%d [$%4.4X-$%4.4X]",vs.VicBank,vs.VicBank*0x4000,(vs.VicBank*0x4000)+0x3FFF);
    ui->OutputList->topLevelItem(5)->setText(1,str00);
    sprintf(str00,"$%4.4X [$%4.4X]",vs.MatrixBase,((unsigned short)vs.VicBank<<14)|vs.MatrixBase);
    ui->OutputList->topLevelItem(6)->setText(1,str00);
    sprintf(str00,"$%4.4X [$%4.4X]",vs.CharBase,((unsigned short)vs.VicBank<<14)|vs.CharBase);
    ui->OutputList->topLevelItem(7)->setText(1,str00);
    ui->OutputList->topLevelItem(8)->setText(1,QVariant(vs.IRQ).toString());
}
