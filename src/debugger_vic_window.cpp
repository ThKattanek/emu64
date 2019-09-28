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
// Letzte Änderung am 28.09.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "./debugger_vic_window.h"
#include "./ui_debugger_vic_window.h"

DebuggerVicWindow::DebuggerVicWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DebuggerVicWindow)
{
    c64 = nullptr;
    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

    ui->OutputList->setColumnWidth(0,110);
    ui->OutputList->setColumnWidth(1,90);

    graphic_modi = QStringList() << "Standard Text" << "Multicolor Text" << "Standard Bitmap" << "Multicolor Bitmap" << "ECM Text" << "Ungültiger Textmodus" << "Ungültiger Bitmapmodus 1" << "Ungültiger Bitmapmodus 2";
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

void DebuggerVicWindow::SetC64Pointer(C64Class *c64)
{
    this->c64 = c64;
}

void DebuggerVicWindow::UpdateOutputList()
{
    if(c64 == nullptr)
        return;

    if(isHidden())
        return;

    VIC_STRUCT vs;
    c64->GetVicReg(&vs);

    char str00[1024];
    sprintf(str00,"$%3.3X (%d)",vs.current_rasterline,vs.current_rasterline);
    ui->OutputList->topLevelItem(0)->setText(1,str00);
    sprintf(str00,"$%3.3X (%d)",vs.raster_latch,vs.raster_latch);
    ui->OutputList->topLevelItem(1)->setText(1,str00);
    sprintf(str00,"%d",vs.current_cycle);
    ui->OutputList->topLevelItem(2)->setText(1,str00);
    if(vs.display_status) ui->OutputList->topLevelItem(3)->setText(1,"Display Mode");
    else ui->OutputList->topLevelItem(3)->setText(1,"Idle Mode");
    ui->OutputList->topLevelItem(4)->setText(1,graphic_modi[vs.graphic_mode & 7]);
    sprintf(str00,"Nr.%d [$%4.4X-$%4.4X]",vs.vic_bank,vs.vic_bank*0x4000,(vs.vic_bank*0x4000)+0x3FFF);
    ui->OutputList->topLevelItem(5)->setText(1,str00);
    sprintf(str00,"$%4.4X [$%4.4X]",vs.matrix_base,(static_cast<uint16_t>(vs.vic_bank<<14)|vs.matrix_base));
    ui->OutputList->topLevelItem(6)->setText(1,str00);
    sprintf(str00,"$%4.4X [$%4.4X]",vs.char_base,(static_cast<uint16_t>(vs.vic_bank<<14)|vs.char_base));
    ui->OutputList->topLevelItem(7)->setText(1,str00);
    ui->OutputList->topLevelItem(8)->setText(1,QVariant(vs.irq).toString());
}
