//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: debugger_window.cpp                   //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 16.02.2013                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QMenu>
#include <QMessageBox>
#include <QFileDialog>
#include <QTimer>
#include <QFontDatabase>

#include "debugger_window.h"
#include "ui_debugger_window.h"
#include "micro_code_tbl_6510.h"
#include "micro_code_string_tbl_6510.h"

DebuggerWindow::DebuggerWindow(QWidget *parent, QSettings *_ini) :
    QDialog(parent),
    ui(new Ui::DebuggerWindow)
{    
    ini = _ini;
    c64 = 0;

    FillMicroCodeStringTable();

    TableBackColor = QColor(255,255,255);
    TablePosColor = QColor(255,200,200);

    old_adresse = 0;
    old_make_idx = 0;

    ui->setupUi(this);

    memory_window = new MemoryWindow(this);
    memory_window->ChangeSource(0);

    vic_window = new DebuggerVicWindow(this);

    NewRefresh = false;
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimerAnimationRefresh()));
    timer1->setInterval(10);

    iOff = new QIcon(":/grafik/blue_led_off.png");
    iOn = new QIcon(":/grafik/blue_led_on.png");
    ui->EingabeFeld->hide();

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/lucon.ttf");
    QFont font1("Lucida Console",8);

    ui->MCodeHelp->setFont(font1);

    ui->DisAssTable->setColumnCount(4);
    ui->DisAssTable->setColumnWidth(0,44);
    ui->DisAssTable->setColumnWidth(1,70);
    ui->DisAssTable->setColumnWidth(2,30);
    ui->DisAssTable->setColumnWidth(3,57);

    ui->DisAssTable->setFont(font1);
    ui->DisAssTable->setRowCount(DisAssZeilen);

    ui->AssAdresseIn->setFont(font1);
    ui->AssMnemonicIn->setFont(font1);
    ui->AssAdressierungIn->setFont(font1);

    ui->BreakpointTree->setColumnCount(2);
    ui->BreakpointTree->setColumnWidth(0,175);
    ui->BreakpointTree->setColumnWidth(1,50);
    NewBreakpointfound = false;

    ui->HistoryList->setFont(font1);
    for(int i=0;i<HistoryZeilen;i++)
    {
        QListWidgetItem *item = new QListWidgetItem(ui->HistoryList);
        item->setText(QVariant(i).toString());
        ui->HistoryList->addItem(item);
    }

    for(int i=0;i<DisAssZeilen;i++)
    {
        ViewCodeAdressen[i] = 0;
        DisAssPC[i] = new QTableWidgetItem();
        DisAssPC[i]->setBackgroundColor(TableBackColor);
        ui->DisAssTable->setItem(i,0,DisAssPC[i]);
        DisAssMem[i] = new QTableWidgetItem();
        DisAssMem[i]->setBackgroundColor(TableBackColor);
        ui->DisAssTable->setItem(i,1,DisAssMem[i]);
        DisAssMenmo[i] = new QTableWidgetItem();
        DisAssMenmo[i]->setBackgroundColor(TableBackColor);
        ui->DisAssTable->setItem(i,2,DisAssMenmo[i]);
        DisAssAdr[i] = new QTableWidgetItem();
        DisAssAdr[i]->setBackgroundColor(TableBackColor);
        ui->DisAssTable->setItem(i,3,DisAssAdr[i]);
    }

    connect(ui->sr_widget,SIGNAL(ChangeValue(unsigned char)),this,SLOT(onSr_widget_ValueChange(unsigned char)));
    connect(ui->pc_out,SIGNAL(clicked(LabelWidgetMod*)),this,SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->sp_out,SIGNAL(clicked(LabelWidgetMod*)),this,SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->ac_out,SIGNAL(clicked(LabelWidgetMod*)),this,SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->xr_out,SIGNAL(clicked(LabelWidgetMod*)),this,SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->yr_out,SIGNAL(clicked(LabelWidgetMod*)),this,SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->sr_out,SIGNAL(clicked(LabelWidgetMod*)),this,SLOT(onReg_label_clicked(LabelWidgetMod*)));

    AktEditReg = -1;

    RWString = QStringList() << "[READ]" << "[WRITE]";

    connect(ui->DisAssTable,SIGNAL(customContextMenuRequested(QPoint)),this,SLOT(onShowContextMenu(QPoint)));

    ////////// Load from INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("DebuggerWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        //if(ini->value("Show",false).toBool()) show();
        ini->endGroup();
    }
    ////////////////////////////////////

    timer1->start();
}

DebuggerWindow::~DebuggerWindow()
{
    ////////// Save to INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("DebuggerWindow");
        if(isOneShowed) ini->setValue("Geometry",saveGeometry());
        if(isHidden()) ini->setValue("Show",false);
        else ini->setValue("Show",true);
        ini->endGroup();
    }
    ////////////////////////////////////

    delete iOff;
    delete iOn;
    delete timer1;
    delete ui;

    delete memory_window;
    delete vic_window;
}

void DebuggerWindow::AnimationRefreshProc()
{
    NewRefresh = true;
}

void DebuggerWindow::onTimerAnimationRefresh(void)
{
    if(NewRefresh)
    {
        NewRefresh = false;
        if(c64 != 0)
        {
            if(AktSource > 0)
            {
                FloppyCpuReg[AktFloppyNr].REG_MASK = REG_MASK_ALL;
                c64->floppy[AktFloppyNr]->GetCpuReg(&FloppyCpuReg[AktFloppyNr],&FloppyCpuIReg[AktFloppyNr]);

                FillDisassemblerList(FloppyCpuIReg[AktFloppyNr].AktOpcodePC,false);
            }
            else
            {
                C64CpuReg.REG_MASK = REG_MASK_ALL;
                c64->GetC64CpuReg(&C64CpuReg,&C64CpuIReg);

                FillDisassemblerList(C64CpuIReg.AktOpcodePC,false);
            }
        }

        UpdateRegister();
        FillHistoryList(ui->HistoryScroll->value());
        memory_window->UpdateMemoryList();
        vic_window->UpdateOutputList();
    }

    if(NewBreakpointfound)
    {
        NewBreakpointfound = false;
        on_AnimationStop_clicked();

        for(int i=0;i<c64->GetBreakGroupAnz();i++)
        {
            BREAK_GROUP *bg = c64->GetBreakGroup(i);
            if(bg->bTrue)
            {
                ui->ChangeSource->setCurrentIndex(0);
                if(i < ui->BreakpointTree->topLevelItemCount())
                {
                    ui->BreakpointTree->topLevelItem(i)->setBackgroundColor(0,QColor(0,255,0));
                    ui->BreakpointTree->topLevelItem(i)->setBackgroundColor(1,QColor(1,255,0));
                    ui->BreakpointTree->topLevelItem(i)->setText(1,tr("ERFÜLLT !"));
                }
            }
        }

        for(int i=0;i<FloppyAnzahl;i++)
        {
            if(c64->floppy[i]->GetEnableFloppy())
            {
                for(int ii=0;ii<c64->floppy[i]->GetBreakGroupAnz();ii++)
                {
                    BREAK_GROUP *bg = c64->floppy[i]->GetBreakGroup(ii);
                    if(bg->bTrue)
                    {
                        ui->ChangeSource->setCurrentIndex(i+1);
                        if(ii < ui->BreakpointTree->topLevelItemCount())
                        {
                            ui->BreakpointTree->topLevelItem(ii)->setBackgroundColor(0,QColor(0,255,0));
                            ui->BreakpointTree->topLevelItem(ii)->setBackgroundColor(1,QColor(1,255,0));
                            ui->BreakpointTree->topLevelItem(ii)->setText(1,tr("ERFÜLLT !"));
                        }
                    }
                }
            }
        }
        show();
    }
}

void DebuggerWindow::FillMicroCodeStringTable()
{
    MicroCodeStringTable6510 = QStringList()
    /*000*/ << tr("Opcode von PC-Adresse holen // PC+1")
    /*001*/ << tr("Lesen von PC-Adresse und verwerfen // PC+1")
    /*002*/ << tr("PC Hi -> Stack // SR|16 // SP-1")
    /*003*/ << tr("PC Lo -> Stack // SP-1")
    /*004*/ << tr("SR -> Stack // SR|4 // SP-1")
    /*005*/ << tr("PC Lo von 0xFFFE holen")
    /*006*/ << tr("PC Hi von 0xFFFF holen")
    /*007*/ << tr("Pointer von PC-Adresse holen // PC+1")
    /*008*/ << tr("Lesen von Pointer und verwerfen // Pointer+XR")
    /*009*/ << tr("Adresse Lo von Pointer-Adresse holen // Pointer+1")
    /*010*/ << tr("Adresse Hi von Pointer-Adresse holen")
    /*011*/ << tr("TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ)")
    /*012*/ << tr("JAM")
    /*013*/ << tr("TMPByte von Adresse holen")
    /*014*/ << tr("TMPByte nach Adresse schreiben // ASL MEMORY // ORA")
    /*015*/ << tr("TMPByte nach Adresse schreiben")
    /*016*/ << tr("Adresse Hi = 0 // Adresse Lo von PC-Adresse holen // PC+1")
    /*017*/ << tr("TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ))")
    /*018*/ << tr("TMPByte nach Adresse schreiben // TMPByte<<1 // Set SR(NZC)")
    /*019*/ << tr("TMPByte von PC-Adresse holen")
    /*020*/ << tr("SR nach SP+0x0100 schreiben // SP-1")
    /*021*/ << tr("TMPByte von PC-Adresse holen // AC or TMPByte // Set SR(NZ) // PC+1")
    /*022*/ << tr("TMPByte von PC-Adresse holen // AC<<1 // Set SR(NZC)")
    /*023*/ << tr("TMPByte von PC-Adresse holen // AC and TMPByte // Set SR(NZC) // PC+1")
    /*024*/ << tr("Adresse Lo von PC-Adresse holen // PC+1")
    /*025*/ << tr("Adresse Hi von PC-Adresse holen // PC+1")
    /*026*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(N) auf FALSE prüfen (BPL)")
    /*027*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(N) auf TRUE prüfen (BMI)")
    /*028*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(V) auf FALSE prüfen (BVC)")
    /*029*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(V) auf TRUE prüfen (BVS)")
    /*030*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(C) auf FALSE prüfen (BCC)")
    /*031*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(C) auf TRUE prüfen (BCS)")
    /*032*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf FALSE prüfen (BNE)")
    /*033*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf TRUE prüfen (BEQ)")
    /*034*/ << tr("Lesen von PC-Adresse und verwerfen // BranchAdresse=PC+TMPByte")
    /*035*/ << tr("FIX PC Hi Adresse")
    /*036*/ << tr("Adresse Hi von Pointer-Adresse holen // Adresse+YR")
    /*037*/ << tr("TMPByte von Adresse holen // Fix Adresse Hi MCT+1 // AC or TMPByte")
    /*038*/ << tr("Adresse Hi von PC-Adresse holen // PC=Adresse")
    /*039*/ << tr("Lesen von PC-Adresse und verwerfen // XR=AC // Set SR(NZ)")
    /*040*/ << tr("Lesen von PC-Adresse und verwerfen // YR=AC // Set SR(NZ)")
    /*041*/ << tr("Lesen von PC-Adresse und verwerfen // XR=SP // Set SR(NZ)")
    /*042*/ << tr("Lesen von PC-Adresse und verwerfen // AC=XR // Set SR(NZ)")
    /*043*/ << tr("Lesen von PC-Adresse und verwerfen // SP=XR")
    /*044*/ << tr("Lesen von PC-Adresse und verwerfen // AC=YR // Set SR(NZ)")
    /*045*/ << tr("AC nach SP+0x0100 schreiben // SP-1")
    /*046*/ << tr("AC von SP+0x0100 lesen // SP+1")
    /*047*/ << tr("AC von SP+0x0100 lesen // Set SR(NZ)")
    /*048*/ << tr("SR von SP+0x0100 lesen // SP+1")
    /*049*/ << tr("SR von SP+0x0100 lesen")
    /*050*/ << tr("TMPByte von PC-Adresse lesen // AC + TMPByte + Carry // PC+1")
    /*051*/ << tr("TMPByte von Adresse lesen // AC + TMPByte + Carry")
    /*052*/ << tr("TMPByte von Adresse lesen // Adresse Lo + XR")
    /*053*/ << tr("Adresse Hi von PC-Adresse holen // Adresse+XR  // PC+1")
    /*054*/ << tr("Adresse Hi von PC-Adresse holen // Adresse+YR  // PC+1")
    /*055*/ << tr("TMPByte von Adresse lesen // AC + TMPByte + Carry // if(idxReg<Adresse Lo) MCT++")
    /*056*/ << tr("TMPByte von PC-Adresse lesen // AC - TMPByte - Carry // PC+1")
    /*057*/ << tr("TMPByte von Adresse lesen // AC - TMPByte - Carry")
    /*058*/ << tr("TMPByte von Adresse lesen // AC - TMPByte - Carry // if(idxReg<Adresse Lo) MCT++")
    /*059*/ << tr("TMPByte von SP+0x0100 holen")
    /*060*/ << tr("PC-Adresse Hi nach SP+0x0100 schreiben // SP--")
    /*061*/ << tr("PC-Adresse Lo nach SP+0x0100 schreiben // SP--")
    /*062*/ << tr("TMPByte von SP+0x0100 holen // SP+1")
    /*063*/ << tr("PC-Adresse Lo von SP+0x0100 holen // SP+1")
    /*064*/ << tr("PC-Adresse Hi von SP+0x0100 holen")
    /*065*/ << tr("TMPByte von PC-Adresse laden // PC+1")
    /*066*/ << tr("TMPByte von PC-Adresse lesen // AC and TMPByte // Set SR(NZ) // PC+1")
    /*067*/ << tr("TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ)")
    /*068*/ << tr("TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*069*/ << tr("TMPByte von Adresse lesen // CarrayFalg=0")
    /*070*/ << tr("TMPByte von Adresse lesen // DezimalFalg=0")
    /*071*/ << tr("TMPByte von Adresse lesen // InterruptFalg=0")
    /*072*/ << tr("TMPByte von Adresse lesen // OverflowFalg=0")
    /*073*/ << tr("TMPByte von Adresse lesen // CarrayFalg=1	")
    /*074*/ << tr("TMPByte von Adresse lesen // DezimalFalg=1")
    /*075*/ << tr("TMPByte von Adresse lesen // InterruptFalg=1")
    /*076*/ << tr("TMPByte von Adresse lesen // BIT Operation")
    /*077*/ << tr("AC nach Adresse schreiben")
    /*078*/ << tr("XR nach Adresse schreiben")
    /*079*/ << tr("YR nach Adresse schreiben")
    /*080*/ << tr("AC von PC-Adresse lesen // Set SR(NZ) // PC+1")
    /*081*/ << tr("AC von PC-Adresse lesen // Set SR(NZ)")
    /*082*/ << tr("AC von PC-Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*083*/ << tr("XR von PC-Adresse lesen // Set SR(NZ) // PC+1")
    /*084*/ << tr("XR von Adresse lesen // Set SR(NZ)")
    /*085*/ << tr("XR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*086*/ << tr("YR von PC-Adresse lesen // Set SR(NZ) // PC+1")
    /*087*/ << tr("YR von Adresse lesen // Set SR(NZ)")
    /*088*/ << tr("YR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*089*/ << tr("TMPByte von Adresse lesen // XR+1 // Set SR(NZ)")
    /*090*/ << tr("TMPByte von Adresse lesen // YR+1 // Set SR(NZ)")
    /*091*/ << tr("TMPByte von Adresse lesen // XR-1 // Set SR(NZ)")
    /*092*/ << tr("TMPByte von Adresse lesen // YR-1 // Set SR(NZ)")
    /*093*/ << tr("Illegaler Opcode - wird noch nicht unterstützt // Reset")
    /*094*/ << tr("PC LO von Adresse lesen // Adresse+1 (Nur Low Wert)")
    /*095*/ << tr("PC HI von Adresse lesen")
    /*096*/ << tr("PC LO von $FFFC lesen")
    /*097*/ << tr("PC HI von $FFFD lesen")
    /*098*/ << tr("TMPByte von PC-Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC) // PC+1")
    /*099*/ << tr("TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC)")
    /*100*/ << tr("TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // if(idxReg<Adresse Lo) MCT++")
    /*101*/ << tr("TMPByte von PC-Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1")
    /*102*/ << tr("TMPByte von Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC)")
    /*103*/ << tr("TMPByte von PC-Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1")
    /*104*/ << tr("TMPByte von Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC)")
    /*105*/ << tr("TMPByte von PC-Adresse lesen // AC XOR TMPByte // Set SR(NZC) // PC+1")
    /*106*/ << tr("TMPByte von Adresse lesen // AC XOR TMPByte // Set SR(NZC)")
    /*107*/ << tr("TMPByte von Adresse lesen // AC XOR TMPByte // if(idxReg<Adresse Lo) MCT++")
    /*108*/ << tr("TMPByte von PC-Adresse holen // AC>>1 // Set SR(NZC)")
    /*109*/ << tr("TMPByte nach Adresse schreiben // TMPByte>>1 // Set SR(NZC)")
    /*110*/ << tr("TMPByte von PC-Adresse holen // C<-AC<<1<-C // Set SR(NZC)")
    /*111*/ << tr("TMPByte nach Adresse schreiben // C<-TMPByte<<1<-C // Set SR(NZC)")
    /*112*/ << tr("TMPByte von PC-Adresse holen // C->AC>>1->C // Set SR(NZC)")
    /*113*/ << tr("TMPByte nach Adresse schreiben // C->TMPByte>>1->C // Set SR(NZC)")
    /*114*/ << tr("TMPByte nach Adresse schreiben // TMPByte+1 // Set SR(NZ)")
    /*115*/ << tr("TMPByte nach Adresse schreiben // TMPByte-1 // Set SR(NZ)")
    /*116*/ << tr("SR nach 0x100+SP schreiben // SP-- // IFlag setzen // BFlag löschen")
    /*117*/ << tr("PC Lo von 0xFFFA holen")
    /*118*/ << tr("PC Hi von 0xFFFB holen")
    /*119*/ << tr("TMPByte von Adresse holen // Fix Adresse Hi MCT+1")
    /*120*/ << tr("TMPByte nach Adresse schreiben // Illegal [SLO]")
    /*121*/ << tr("TMPByte nach Adresse schreiben // Illegal [RLA]")
    /*122*/ << tr("TMPByte nach Adresse schreiben // Illegal [SRE]")
    /*123*/ << tr("TMPByte nach Adresse schreiben // Illegal [RRA]")
    /*124*/ << tr("TMPByte nach Adresse schreiben // Illegal [DCP]")
    /*125*/ << tr("TMPByte nach Adresse schreiben // Illegal [ISB]")
    /*126*/ << tr("AC von Adresse lesen // AC -> XR // Set SR(NZ) // Illegal [LAX]")
    /*127*/ << tr("AC von Adresse lesen // AC -> XR // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++ // Illegal [LAX]")
    /*128*/ << tr("AC von Adresse lesen // AC -> XR // Set SR(NZ) // Illegal [LAX]")
    /*129*/ << tr("Illegal [ASR]")
    /*130*/ << tr("Illegal [ARR]")
    /*131*/ << tr("Illegal [ANE]")
    /*132*/ << tr("Illegal [LXA]")
    /*133*/ << tr("Illegal [SBX]")
    /*134*/ << tr("Illegal [SHY]")
    /*135*/ << tr("Illegal [SHX]")
    /*136*/ << tr("Illegal [SHA]")
    /*137*/ << tr("Illegal [SHS]")
    /*138*/ << tr("Illegal [ANC]")
    /*139*/ << tr("Illegal [LAE]")
    /*140*/ << tr("Illegal [LAE] // if(idxReg<Adresse Lo) MCT++");
}

void DebuggerWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    ui->sr_widget->RetranslateUi();

    MicroCodeStringTable6510 = QStringList()
    /*000*/ << tr("Opcode von PC-Adresse holen // PC+1")
    /*001*/ << tr("Lesen von PC-Adresse und verwerfen // PC+1")
    /*002*/ << tr("PC Hi -> Stack // SR|16 // SP-1")
    /*003*/ << tr("PC Lo -> Stack // SP-1")
    /*004*/ << tr("SR -> Stack // SR|4 // SP-1")
    /*005*/ << tr("PC Lo von 0xFFFE holen")
    /*006*/ << tr("PC Hi von 0xFFFF holen")
    /*007*/ << tr("Pointer von PC-Adresse holen // PC+1")
    /*008*/ << tr("Lesen von Pointer und verwerfen // Pointer+XR")
    /*009*/ << tr("Adresse Lo von Pointer-Adresse holen // Pointer+1")
    /*010*/ << tr("Adresse Hi von Pointer-Adresse holen")
    /*011*/ << tr("TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ)")
    /*012*/ << tr("JAM")
    /*013*/ << tr("TMPByte von Adresse holen")
    /*014*/ << tr("TMPByte nach Adresse schreiben // ASL MEMORY // ORA")
    /*015*/ << tr("TMPByte nach Adresse schreiben")
    /*016*/ << tr("Adresse Hi = 0 // Adresse Lo von PC-Adresse holen // PC+1")
    /*017*/ << tr("TMPByte von Adresse holen // AC or TMPByte // Set SR(NZ))")
    /*018*/ << tr("TMPByte nach Adresse schreiben // TMPByte<<1 // Set SR(NZC)")
    /*019*/ << tr("TMPByte von PC-Adresse holen")
    /*020*/ << tr("SR nach SP+0x0100 schreiben // SP-1")
    /*021*/ << tr("TMPByte von PC-Adresse holen // AC or TMPByte // Set SR(NZ) // PC+1")
    /*022*/ << tr("TMPByte von PC-Adresse holen // AC<<1 // Set SR(NZC)")
    /*023*/ << tr("TMPByte von PC-Adresse holen // AC and TMPByte // Set SR(NZC) // PC+1")
    /*024*/ << tr("Adresse Lo von PC-Adresse holen // PC+1")
    /*025*/ << tr("Adresse Hi von PC-Adresse holen // PC+1")
    /*026*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(N) auf FALSE prüfen (BPL)")
    /*027*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(N) auf TRUE prüfen (BMI)")
    /*028*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(V) auf FALSE prüfen (BVC)")
    /*029*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(V) auf TRUE prüfen (BVS)")
    /*030*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(C) auf FALSE prüfen (BCC)")
    /*031*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(C) auf TRUE prüfen (BCS)")
    /*032*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf FALSE prüfen (BNE)")
    /*033*/ << tr("TMPByte von PC-Adresse holen // PC+1 // SR(Z) auf TRUE prüfen (BEQ)")
    /*034*/ << tr("Lesen von PC-Adresse und verwerfen // BranchAdresse=PC+TMPByte")
    /*035*/ << tr("FIX PC Hi Adresse")
    /*036*/ << tr("Adresse Hi von Pointer-Adresse holen // Adresse+YR")
    /*037*/ << tr("TMPByte von Adresse holen // Fix Adresse Hi MCT+1 // AC or TMPByte")
    /*038*/ << tr("Adresse Hi von PC-Adresse holen // PC=Adresse")
    /*039*/ << tr("Lesen von PC-Adresse und verwerfen // XR=AC // Set SR(NZ)")
    /*040*/ << tr("Lesen von PC-Adresse und verwerfen // YR=AC // Set SR(NZ)")
    /*041*/ << tr("Lesen von PC-Adresse und verwerfen // XR=SP // Set SR(NZ)")
    /*042*/ << tr("Lesen von PC-Adresse und verwerfen // AC=XR // Set SR(NZ)")
    /*043*/ << tr("Lesen von PC-Adresse und verwerfen // SP=XR")
    /*044*/ << tr("Lesen von PC-Adresse und verwerfen // AC=YR // Set SR(NZ)")
    /*045*/ << tr("AC nach SP+0x0100 schreiben // SP-1")
    /*046*/ << tr("AC von SP+0x0100 lesen // SP+1")
    /*047*/ << tr("AC von SP+0x0100 lesen // Set SR(NZ)")
    /*048*/ << tr("SR von SP+0x0100 lesen // SP+1")
    /*049*/ << tr("SR von SP+0x0100 lesen")
    /*050*/ << tr("TMPByte von PC-Adresse lesen // AC + TMPByte + Carry // PC+1")
    /*051*/ << tr("TMPByte von Adresse lesen // AC + TMPByte + Carry")
    /*052*/ << tr("TMPByte von Adresse lesen // Adresse Lo + XR")
    /*053*/ << tr("Adresse Hi von PC-Adresse holen // Adresse+XR  // PC+1")
    /*054*/ << tr("Adresse Hi von PC-Adresse holen // Adresse+YR  // PC+1")
    /*055*/ << tr("TMPByte von Adresse lesen // AC + TMPByte + Carry // if(idxReg<Adresse Lo) MCT++")
    /*056*/ << tr("TMPByte von PC-Adresse lesen // AC - TMPByte - Carry // PC+1")
    /*057*/ << tr("TMPByte von Adresse lesen // AC - TMPByte - Carry")
    /*058*/ << tr("TMPByte von Adresse lesen // AC - TMPByte - Carry // if(idxReg<Adresse Lo) MCT++")
    /*059*/ << tr("TMPByte von SP+0x0100 holen")
    /*060*/ << tr("PC-Adresse Hi nach SP+0x0100 schreiben // SP--")
    /*061*/ << tr("PC-Adresse Lo nach SP+0x0100 schreiben // SP--")
    /*062*/ << tr("TMPByte von SP+0x0100 holen // SP+1")
    /*063*/ << tr("PC-Adresse Lo von SP+0x0100 holen // SP+1")
    /*064*/ << tr("PC-Adresse Hi von SP+0x0100 holen")
    /*065*/ << tr("TMPByte von PC-Adresse laden // PC+1")
    /*066*/ << tr("TMPByte von PC-Adresse lesen // AC and TMPByte // Set SR(NZ) // PC+1")
    /*067*/ << tr("TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ)")
    /*068*/ << tr("TMPByte von Adresse lesen // AC and TMPByte // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*069*/ << tr("TMPByte von Adresse lesen // CarrayFalg=0")
    /*070*/ << tr("TMPByte von Adresse lesen // DezimalFalg=0")
    /*071*/ << tr("TMPByte von Adresse lesen // InterruptFalg=0")
    /*072*/ << tr("TMPByte von Adresse lesen // OverflowFalg=0")
    /*073*/ << tr("TMPByte von Adresse lesen // CarrayFalg=1	")
    /*074*/ << tr("TMPByte von Adresse lesen // DezimalFalg=1")
    /*075*/ << tr("TMPByte von Adresse lesen // InterruptFalg=1")
    /*076*/ << tr("TMPByte von Adresse lesen // BIT Operation")
    /*077*/ << tr("AC nach Adresse schreiben")
    /*078*/ << tr("XR nach Adresse schreiben")
    /*079*/ << tr("YR nach Adresse schreiben")
    /*080*/ << tr("AC von PC-Adresse lesen // Set SR(NZ) // PC+1")
    /*081*/ << tr("AC von PC-Adresse lesen // Set SR(NZ)")
    /*082*/ << tr("AC von PC-Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*083*/ << tr("XR von PC-Adresse lesen // Set SR(NZ) // PC+1")
    /*084*/ << tr("XR von Adresse lesen // Set SR(NZ)")
    /*085*/ << tr("XR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*086*/ << tr("YR von PC-Adresse lesen // Set SR(NZ) // PC+1")
    /*087*/ << tr("YR von Adresse lesen // Set SR(NZ)")
    /*088*/ << tr("YR von Adresse lesen // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++")
    /*089*/ << tr("TMPByte von Adresse lesen // XR+1 // Set SR(NZ)")
    /*090*/ << tr("TMPByte von Adresse lesen // YR+1 // Set SR(NZ)")
    /*091*/ << tr("TMPByte von Adresse lesen // XR-1 // Set SR(NZ)")
    /*092*/ << tr("TMPByte von Adresse lesen // YR-1 // Set SR(NZ)")
    /*093*/ << tr("Illegaler Opcode - wird noch nicht unterstützt // Reset")
    /*094*/ << tr("PC LO von Adresse lesen // Adresse+1 (Nur Low Wert)")
    /*095*/ << tr("PC HI von Adresse lesen")
    /*096*/ << tr("PC LO von $FFFC lesen")
    /*097*/ << tr("PC HI von $FFFD lesen")
    /*098*/ << tr("TMPByte von PC-Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC) // PC+1")
    /*099*/ << tr("TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // Set SR(NZC)")
    /*100*/ << tr("TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht verändert) // if(idxReg<Adresse Lo) MCT++")
    /*101*/ << tr("TMPByte von PC-Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1")
    /*102*/ << tr("TMPByte von Adresse lesen // XR - TMPByte (XR wird nicht verändert) // Set SR(NZC)")
    /*103*/ << tr("TMPByte von PC-Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC) // PC+1")
    /*104*/ << tr("TMPByte von Adresse lesen // YR - TMPByte (XR wird nicht verändert) // Set SR(NZC)")
    /*105*/ << tr("TMPByte von PC-Adresse lesen // AC XOR TMPByte // Set SR(NZC) // PC+1")
    /*106*/ << tr("TMPByte von Adresse lesen // AC XOR TMPByte // Set SR(NZC)")
    /*107*/ << tr("TMPByte von Adresse lesen // AC XOR TMPByte // if(idxReg<Adresse Lo) MCT++")
    /*108*/ << tr("TMPByte von PC-Adresse holen // AC>>1 // Set SR(NZC)")
    /*109*/ << tr("TMPByte nach Adresse schreiben // TMPByte>>1 // Set SR(NZC)")
    /*110*/ << tr("TMPByte von PC-Adresse holen // C<-AC<<1<-C // Set SR(NZC)")
    /*111*/ << tr("TMPByte nach Adresse schreiben // C<-TMPByte<<1<-C // Set SR(NZC)")
    /*112*/ << tr("TMPByte von PC-Adresse holen // C->AC>>1->C // Set SR(NZC)")
    /*113*/ << tr("TMPByte nach Adresse schreiben // C->TMPByte>>1->C // Set SR(NZC)")
    /*114*/ << tr("TMPByte nach Adresse schreiben // TMPByte+1 // Set SR(NZ)")
    /*115*/ << tr("TMPByte nach Adresse schreiben // TMPByte-1 // Set SR(NZ)")
    /*116*/ << tr("SR nach 0x100+SP schreiben // SP-- // IFlag setzen // BFlag löschen")
    /*117*/ << tr("PC Lo von 0xFFFA holen")
    /*118*/ << tr("PC Hi von 0xFFFB holen")
    /*119*/ << tr("TMPByte von Adresse holen // Fix Adresse Hi MCT+1")
    /*120*/ << tr("TMPByte nach Adresse schreiben // Illegal [SLO]")
    /*121*/ << tr("TMPByte nach Adresse schreiben // Illegal [RLA]")
    /*122*/ << tr("TMPByte nach Adresse schreiben // Illegal [SRE]")
    /*123*/ << tr("TMPByte nach Adresse schreiben // Illegal [RRA]")
    /*124*/ << tr("TMPByte nach Adresse schreiben // Illegal [DCP]")
    /*125*/ << tr("TMPByte nach Adresse schreiben // Illegal [ISB]")
    /*126*/ << tr("AC von Adresse lesen // AC -> XR // Set SR(NZ) // Illegal [LAX]")
    /*127*/ << tr("AC von Adresse lesen // AC -> XR // Set SR(NZ) // if(idxReg<Adresse Lo) MCT++ // Illegal [LAX]")
    /*128*/ << tr("AC von Adresse lesen // AC -> XR // Set SR(NZ) // Illegal [LAX]")
    /*129*/ << tr("Illegal [ASR]")
    /*130*/ << tr("Illegal [ARR]")
    /*131*/ << tr("Illegal [ANE]")
    /*132*/ << tr("Illegal [LXA]")
    /*133*/ << tr("Illegal [SBX]")
    /*134*/ << tr("Illegal [SHY]")
    /*135*/ << tr("Illegal [SHX]")
    /*136*/ << tr("Illegal [SHA]")
    /*137*/ << tr("Illegal [SHS]")
    /*138*/ << tr("Illegal [ANC]")
    /*139*/ << tr("Illegal [LAE]")
    /*140*/ << tr("Illegal [LAE] // if(idxReg<Adresse Lo) MCT++");

    this->update();
    this->UpdateRegister();

    vic_window->RetranslateUi();
    memory_window->RetranslateUi();
}

void DebuggerWindow::SetC64Pointer(C64Class *_c64)
{
    c64 = _c64;
    memory_window->SetC64Pointer(_c64);
    vic_window->SetC64Pointer(_c64);
    c64->AnimationRefreshProc = bind(&DebuggerWindow::AnimationRefreshProc,this);
    c64->BreakpointProc = bind(&DebuggerWindow::BreakpointProc,this);
}

void DebuggerWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
    c64->SetDebugMode(true);
    RefreshGUI();
}

void DebuggerWindow::hideEvent(QHideEvent*)
{
    /// HIDE ///

    memory_window->hide();
    vic_window->hide();

    c64->SetDebugMode(false);
    c64->SetDebugAnimation(false);

    ui->OneOpcode->setEnabled(true);
    ui->OneZyklus->setEnabled(true);
    ui->AnimationStart->setEnabled(true);
    ui->AnimationStop->setEnabled(false);
    ui->DisAssScroll->setEnabled(true);

    ui->AssAdresseIn->setEnabled(true);
    ui->AssMnemonicIn->setEnabled(true);
    ui->AssAdressierungIn->setEnabled(true);
}

void DebuggerWindow::UpdateRegister()
{
    if(c64 == 0) return;

    char str00[1024];

    if(AktSource > 0)
    {
        /// Floppy's ///

        if(!c64->floppy[AktFloppyNr]->GetEnableFloppy()) return;

        sprintf(str00,"$%4.4X",FloppyCpuReg[AktFloppyNr].PC);
        ui->pc_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",FloppyCpuReg[AktFloppyNr].SP + 0x100);
        ui->sp_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",FloppyCpuReg[AktFloppyNr].AC);
        ui->ac_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",FloppyCpuReg[AktFloppyNr].XR);
        ui->xr_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",FloppyCpuReg[AktFloppyNr].YR);
        ui->yr_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",FloppyCpuReg[AktFloppyNr].SR);
        ui->sr_out->setText(QString(str00));
        ui->sr_widget->SetValue(FloppyCpuReg[AktFloppyNr].SR);

        sprintf(str00,"$%4.4X",FloppyCpuReg[AktFloppyNr].IRQ);
        ui->irq_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",FloppyCpuReg[AktFloppyNr].NMI);
        ui->nmi_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",FloppyCpuReg[AktFloppyNr]._0314);
        ui->_0314_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",FloppyCpuReg[AktFloppyNr]._0318);
        ui->_0318_out->setText(QString(str00));

        sprintf(str00,"$%4.4X",FloppyCpuIReg[AktFloppyNr].AktOpcodePC);
        ui->opcode_pc_out->setText(QString(str00));
        sprintf(str00,"$%3.3X",FloppyCpuIReg[AktFloppyNr].AktOpcode);
        ui->opcode_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",FloppyCpuIReg[AktFloppyNr].Adresse);
        ui->adresse_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",FloppyCpuIReg[AktFloppyNr].BranchAdresse);
        ui->branch_adresse_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",FloppyCpuIReg[AktFloppyNr].Pointer);
        ui->pointer_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",FloppyCpuIReg[AktFloppyNr].TMPByte);
        ui->tmp_byte_out->setText(QString(str00));
        sprintf(str00,"%10.10d",(int)FloppyCpuIReg[AktFloppyNr].CycleCounter);
        ui->CycleCounter_Out->setText(QString(str00));

        sprintf(str00,"%3.3d ",FloppyCpuIReg[AktFloppyNr].AktMicroCode);
        ui->micro_code_out->setText(str00 + RWString[MicroCodeRWTable6510[FloppyCpuIReg[AktFloppyNr].AktMicroCode]]);

        ui->MCodeHelp->setText(MicroCodeStringTable6510[FloppyCpuIReg[AktFloppyNr].AktMicroCode]);
        ui->mnemonic_out->setText(QString(CpuOPC).mid(FloppyCpuIReg[AktFloppyNr].AktOpcode*3,3));

        if(FloppyCpuIReg[AktFloppyNr].IRQ) ui->irq_led->setIcon(*iOn);
        else ui->irq_led->setIcon(*iOff);
        if(!FloppyCpuIReg[AktFloppyNr].RESET) ui->reset_led->setIcon(*iOn);
        else ui->reset_led->setIcon(*iOff);
    }
    else
    {
        /// C64 ///
        sprintf(str00,"$%4.4X",C64CpuReg.PC);
        ui->pc_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",C64CpuReg.SP + 0x100);
        ui->sp_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",C64CpuReg.AC);
        ui->ac_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",C64CpuReg.XR);
        ui->xr_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",C64CpuReg.YR);
        ui->yr_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",C64CpuReg.SR);
        ui->sr_out->setText(QString(str00));
        ui->sr_widget->SetValue(C64CpuReg.SR);

        sprintf(str00,"$%4.4X",C64CpuReg.IRQ);
        ui->irq_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",C64CpuReg.NMI);
        ui->nmi_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",C64CpuReg._0314);
        ui->_0314_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",C64CpuReg._0318);
        ui->_0318_out->setText(QString(str00));

        sprintf(str00,"$%4.4X",C64CpuIReg.AktOpcodePC);
        ui->opcode_pc_out->setText(QString(str00));
        sprintf(str00,"$%3.3X",C64CpuIReg.AktOpcode);
        ui->opcode_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",C64CpuIReg.Adresse);
        ui->adresse_out->setText(QString(str00));
        sprintf(str00,"$%4.4X",C64CpuIReg.BranchAdresse);
        ui->branch_adresse_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",C64CpuIReg.Pointer);
        ui->pointer_out->setText(QString(str00));
        sprintf(str00,"$%2.2X",C64CpuIReg.TMPByte);
        ui->tmp_byte_out->setText(QString(str00));
        sprintf(str00,"%10.10d",(int)C64CpuIReg.CycleCounter);
        ui->CycleCounter_Out->setText(QString(str00));

        sprintf(str00,"%3.3d ",C64CpuIReg.AktMicroCode);
        ui->micro_code_out->setText(str00 + RWString[MicroCodeRWTable6510[C64CpuIReg.AktMicroCode]]);

        ui->MCodeHelp->setText(MicroCodeStringTable6510[C64CpuIReg.AktMicroCode]);
        ui->mnemonic_out->setText(QString(CpuOPC).mid(C64CpuIReg.AktOpcode*3,3));

        if(C64CpuIReg.CpuWait) ui->wait_led->setIcon(*iOn);
        else ui->wait_led->setIcon(*iOff);

        if(!ui->man_lines->checkState())
        {
            if(C64CpuIReg.IRQ) ui->irq_led->setIcon(*iOn);
            else ui->irq_led->setIcon(*iOff);
            if(C64CpuIReg.NMI) ui->nmi_led->setIcon(*iOn);
            else ui->nmi_led->setIcon(*iOff);
            if(C64CpuIReg.RDY) ui->rdy_led->setIcon(*iOn);
            else ui->rdy_led->setIcon(*iOff);
        }
        if(!C64CpuIReg.RESET) ui->reset_led->setIcon(*iOn);
        else ui->reset_led->setIcon(*iOff);
        if(C64CpuIReg.EXROM) ui->exrom_led->setIcon(*iOn);
        else ui->exrom_led->setIcon(*iOff);
        if(C64CpuIReg.GAME) ui->game_led->setIcon(*iOn);
        else ui->game_led->setIcon(*iOff);
    }
}

void DebuggerWindow::onShowContextMenu(const QPoint& pos)
{
    // for most widgets
    QPoint globalPos = ui->DisAssTable->mapToGlobal(pos);
    // for QAbstractScrollArea and derived classes you would use:
    // QPoint globalPos = myWidget->viewport()->mapToGlobal(pos);

    QMenu myMenu;
    myMenu.addAction(tr("Test1"));
    // ...

    QAction* selectedItem = myMenu.exec(globalPos);
    if (selectedItem)
    {
        // something was chosen, do stuff
    }
    else
    {
        // nothing was chosen
    }

}

void DebuggerWindow::onSr_widget_ValueChange(unsigned char value)
{
    on_AnimationStop_clicked();

    ui->EingabeFeld->hide();

    REG_STRUCT CpuReg;
    CpuReg.REG_MASK = REG_MASK_SR;
    CpuReg.SR = value;

    char str00[50];

    sprintf(str00,"$%2.2X",value);
    ui->sr_out->setText(str00);

    if(AktSource > 0)
    {
        c64->floppy[AktFloppyNr]->SetCpuReg(&CpuReg);
    }
    else c64->cpu->SetRegister(&CpuReg);
}

void DebuggerWindow::onReg_label_clicked(LabelWidgetMod* label)
{
    on_AnimationStop_clicked();

    AktEditReg = -1;

    if(label->objectName() == "pc_out") AktEditReg = 0;
    if(label->objectName() == "sp_out") AktEditReg = 1;
    if(label->objectName() == "ac_out") AktEditReg = 2;
    if(label->objectName() == "xr_out") AktEditReg = 3;
    if(label->objectName() == "yr_out") AktEditReg = 4;
    if(label->objectName() == "sr_out") AktEditReg = 5;

    ui->EingabeFeld->setGeometry(label->geometry());

    ui->EingabeFeld->setText(label->text());
    ui->EingabeFeld->selectAll();
    ui->EingabeFeld->show();
    ui->EingabeFeld->setFocus();
}

void DebuggerWindow::on_EingabeFeld_returnPressed()
{
    if(AktEditReg == -1)
    {
        ui->EingabeFeld->hide();
        return;
    }

    bool ok;
    QString in_str = ui->EingabeFeld->text();
    unsigned short value;

    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt

    value = in_str.toUShort(&ok,0);                //
    if(!ok)
    {
        QMessageBox::warning(this,tr("Eingabefehler..."),tr("Es wurde kein gültiges Zahlenformat benutzt !"));
        return;
    }

    REG_STRUCT CpuReg;

    switch(AktEditReg)
    {
    case 0:
        CpuReg.REG_MASK = REG_MASK_PC;
        CpuReg.PC = value;
        break;
    case 1:
        if(value > 0x100) value -= 0x100;
        if(value > 0xFF)
        {
            QMessageBox::warning(this,tr("Eingabefehler..."),tr("Der Wert muss zwischen 0 und 511 liegen !"));
            return;
        }
        CpuReg.REG_MASK = REG_MASK_SP;
        CpuReg.SP = value;
        break;
    case 2:
        if(value > 0xFF)
        {
            QMessageBox::warning(this,tr("Eingabefehler..."),tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        CpuReg.REG_MASK = REG_MASK_AC;
        CpuReg.AC = value;
        break;
    case 3:
        if(value > 0xFF)
        {
            QMessageBox::warning(this,tr("Eingabefehler..."),tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        CpuReg.REG_MASK = REG_MASK_XR;
        CpuReg.XR = value;
        break;
    case 4:
        if(value > 0xFF)
        {
            QMessageBox::warning(this,tr("Eingabefehler..."),tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        CpuReg.REG_MASK = REG_MASK_YR;
        CpuReg.YR = value;
        break;
    case 5:
        if(value > 0xFF)
        {
            QMessageBox::warning(this,tr("Eingabefehler..."),tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        CpuReg.REG_MASK = REG_MASK_SR;
        CpuReg.SR = value;
        break;
    }

    if(AktSource > 0)
    {
        c64->floppy[AktFloppyNr]->SetCpuReg(&CpuReg);
        c64->floppy[AktFloppyNr]->GetCpuReg(&FloppyCpuReg[AktFloppyNr],&FloppyCpuIReg[AktFloppyNr]);
    }
    else
    {
        c64->cpu->SetRegister(&CpuReg);
        c64->cpu->GetRegister(&C64CpuReg);
    }

    AktEditReg = -1;
    ui->EingabeFeld->hide();
    UpdateRegister();
}

void DebuggerWindow::on_OneZyklus_clicked()
{
    ui->EingabeFeld->hide();
    ClearAllBreakpointBackcolors();
    c64->OneZyklus();
}

void DebuggerWindow::on_OneOpcode_clicked()
{
    ui->EingabeFeld->hide();
    ClearAllBreakpointBackcolors();
    c64->OneOpcode(AktSource);
}


void DebuggerWindow::on_CycleCounterReset_clicked()
{
    if(AktSource > 0)
    {
        c64->floppy[AktFloppyNr]->ResetCycleCounter();
    }
    else
    {
        c64->ResetC64CycleCounter();
    }
    ui->CycleCounter_Out->setText("0000000000");
}

bool DebuggerWindow::getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt)
{
   if (fileName == NULL)      // "parent" is allowed to be NULL!
      return false;

   QFileDialog saveDialog(parent);
   saveDialog.setWindowTitle(caption);
   saveDialog.setAcceptMode(QFileDialog::AcceptSave);
   saveDialog.setConfirmOverwrite(false);
   //saveDialog.setFilter(filter);
   saveDialog.selectFile(*fileName);

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
      /*
      extension = saveDialog.selectedFilter();
      extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
      extension = extension.left(extension.indexOf(")"));
      extension = extension.simplified();
      */
      // If the filter specifies more than one extension, choose the first one
      if (extension.indexOf(" ") != -1)
         extension = extension.left(extension.indexOf(" "));

      tmpFileName = tmpFileName + QString(".") + extension;
      fileInfo.setFile(tmpFileName);
   }

   // Does the file already exist?
   if (QFile::exists(tmpFileName)) {
       /*
       extension = saveDialog.selectedFilter();
       extension = extension.right(extension.size() - extension.indexOf("*.") - 2);
       extension = extension.left(extension.indexOf(")"));
       extension = extension.simplified();
       */
      int result = QMessageBox::question(parent, QObject::tr("Überschreiben?"),
         QObject::tr("Soll die Datei \"%1\" überschrieben werden?").arg(fileInfo.fileName()),
         QMessageBox::Yes,
         QMessageBox::No | QMessageBox::Default,
         QMessageBox::Cancel | QMessageBox::Escape);
      if (result == QMessageBox::Cancel)
         return false;
      else if (result == QMessageBox::No) {
         // Next chance for the user to select a filename
         if (!getSaveFileName(parent, caption, filter, &tmpFileName, &extension))
            // User decided to cancel, exit function here
            return false;
      // User clicked "Yes", so process the execution
      fileInfo.setFile(tmpFileName);
      }
   }

   *fileName = tmpFileName;
   *fileExt = extension;
   return true;
}

void DebuggerWindow::FillDisassemblerList(unsigned short adresse,bool new_refresh)
{
    unsigned short PC = adresse;
    char str00[50];
    unsigned short tmp;
    unsigned short word;
    char tmp1;
    bool find = false;
    int akt_make_idx = 0;

    if(AktSource > 0)
    {
        if(!c64->floppy[AktSource-1]->GetEnableFloppy()) return;
    }

    if(!new_refresh)
    {
        for(int i=0;i<DisAssZeilen;i++)
        {
            if(ViewCodeAdressen[i] == adresse)
            {
                find = true;
                akt_make_idx = i;
            }
        }
    }

    if(find)
    {
        DisAssPC[old_make_idx]->setBackgroundColor(TableBackColor);
        DisAssMem[old_make_idx]->setBackgroundColor(TableBackColor);
        DisAssMenmo[old_make_idx]->setBackgroundColor(TableBackColor);
        DisAssAdr[old_make_idx]->setBackgroundColor(TableBackColor);

        DisAssPC[akt_make_idx]->setBackgroundColor(TablePosColor);
        DisAssMem[akt_make_idx]->setBackgroundColor(TablePosColor);
        DisAssMenmo[akt_make_idx]->setBackgroundColor(TablePosColor);
        DisAssAdr[akt_make_idx]->setBackgroundColor(TablePosColor);

        old_make_idx = akt_make_idx;
        PC = old_adresse;
    }
    else
    {
        DisAssPC[old_make_idx]->setBackgroundColor(TableBackColor);
        DisAssMem[old_make_idx]->setBackgroundColor(TableBackColor);
        DisAssMenmo[old_make_idx]->setBackgroundColor(TableBackColor);
        DisAssAdr[old_make_idx]->setBackgroundColor(TableBackColor);

        DisAssPC[0]->setBackgroundColor(TablePosColor);
        DisAssMem[0]->setBackgroundColor(TablePosColor);
        DisAssMenmo[0]->setBackgroundColor(TablePosColor);
        DisAssAdr[0]->setBackgroundColor(TablePosColor);

        old_make_idx = akt_make_idx = 0;
        PC = old_adresse = adresse;
    }

    unsigned char ram0;
    unsigned char ram1;
    unsigned char ram2;

    for(int i=0;i<DisAssZeilen;i++)
    {
        ViewCodeAdressen[i] = PC;
        sprintf(str00,"$%4.4X",PC);
        DisAssPC[i]->setText(str00);

        if(AktSource > 0)
        {
            AktFloppyNr = AktSource - 1;
            ram0 = c64->floppy[AktFloppyNr]->ReadByte(PC+0);
            ram1 = c64->floppy[AktFloppyNr]->ReadByte(PC+1);
            ram2 = c64->floppy[AktFloppyNr]->ReadByte(PC+2);
        }
        else
        {
            ram0 = c64->ReadC64Byte(PC+0);
            ram1 = c64->ReadC64Byte(PC+1);
            ram2 = c64->ReadC64Byte(PC+2);
        }

        DisAssMenmo[i]->setText(QString(CpuOPC).mid(ram0*3,3));

        tmp = CpuOPCInfo[ram0];

        if(!(tmp&8))
        {
            DisAssMenmo[i]->setTextColor(QColor(0,0,0));
        }
        else
        {
            DisAssMenmo[i]->setTextColor(QColor(255,0,0));
        }

        tmp >>= 4;
        tmp &= 15;
        switch(tmp)
        {
        case 0:
            DisAssAdr[i]->setText("");

            sprintf(str00,"%2.2X -- --",ram0);
            DisAssMem[i]->setText(str00);
            PC++;
            break;
        case 1:
            sprintf(str00,"#$%2.2X",ram1);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            DisAssMem[i]->setText(str00);
            PC+=2;
            break;
        case 2:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"$%4.4X",word);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            DisAssMem[i]->setText(str00);
            PC+=3;
            break;
        case 3:
            sprintf(str00,"$%2.2X",ram1);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            DisAssMem[i]->setText(str00);
            PC+=2;
            break;
        case 4:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"$%4.4X,X",word);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            DisAssMem[i]->setText(str00);
            PC+=3;
            break;
        case 5:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"$%4.4X,Y",word);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            DisAssMem[i]->setText(str00);
            PC+=3;
            break;
        case 6:
            sprintf(str00,"$%2.2X,X",ram1);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            DisAssMem[i]->setText(str00);
            PC+=2;
            break;
        case 7:
            sprintf(str00,"($%2.2X,X)",ram1);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            DisAssMem[i]->setText(str00);
            PC+=2;
            break;
        case 8:
            sprintf(str00,"($%2.2X),Y",ram1);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            DisAssMem[i]->setText(str00);
            PC+=2;
            break;
        case 9:
            tmp1 = ram1;
            word = (PC+2)+tmp1;
            sprintf(str00,"$%4.4X",word);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            DisAssMem[i]->setText(str00);
            PC+=2;
            break;
        case 10:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"($%4.4X)",word);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            DisAssMem[i]->setText(str00);
            PC+=3;
            break;
        case 11:
            sprintf(str00,"$%2.2X,Y",ram1);
            DisAssAdr[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            DisAssMem[i]->setText(str00);
            PC+=2;
            break;
        }
    }
}

void DebuggerWindow::FillHistoryList(unsigned char index)
{
    char str00[10];
    unsigned char hp;

    if(AktSource > 0)
    {
        if(!c64->floppy[AktFloppyNr]->GetEnableFloppy()) return;
        hp = c64->floppy[AktFloppyNr]->HistoryPointer;
        for(int i=0;i<HistoryZeilen;i++)
        {
            sprintf(str00,"$%4.4X",c64->floppy[AktFloppyNr]->History[hp--]);
            ui->HistoryList->item(i)->setText(QString(str00));
        }
    }
    else
    {
        hp = c64->C64HistoryPointer - index;
        for(int i=0;i<HistoryZeilen;i++)
        {
            sprintf(str00,"$%4.4X",c64->C64History[hp--]);
            ui->HistoryList->item(i)->setText(QString(str00));
        }
    }
}

void DebuggerWindow::on_ChangeSource_currentIndexChanged(int index)
{
    AktSource = index;
    if(AktSource > 0)
    {
        AktFloppyNr = AktSource - 1;
        ui->BreakpointTree->clear();

        if(c64 == 0) return;
        BreakPointUpdateEnable = false;
        int anz = c64->floppy[AktFloppyNr]->GetBreakGroupAnz();
        for(int i=0;i<anz;i++)
        {
            BREAK_GROUP *bg = c64->floppy[AktFloppyNr]->GetBreakGroup(i);
            AddBreakpointTreeRoot(bg->Name,bg);
        }
        BreakPointUpdateEnable = true;
    }
    else
    {
        /// Alle Haltepunke ins TreeWidget einfügen ///

        ui->BreakpointTree->clear();

        if(c64 == 0) return;
        BreakPointUpdateEnable = false;
        int anz = c64->GetBreakGroupAnz();
        for(int i=0;i<anz;i++)
        {
            BREAK_GROUP *bg = c64->GetBreakGroup(i);
            AddBreakpointTreeRoot(bg->Name,bg);
        }
        BreakPointUpdateEnable = true;
    }

    ui->AssAdresseIn->setText("");
    ui->AssMnemonicIn->setText("");
    ui->AssAdressierungIn->setText("");
    memory_window->ChangeSource(AktSource);
    RefreshGUI();
}

void DebuggerWindow::on_AssAdresseIn_returnPressed()
{
    bool ok;
    unsigned short value;
    QString in_str = ui->AssAdresseIn->text();

    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    value = in_str.toUShort(&ok,0);                //
    if(!ok)
    {
        QMessageBox::warning(this,tr("Eingabefehler..."),tr("Es wurde kein gültiges Zahlenformat benutzt !"));
        return;
    }

    char out_str[10];
    sprintf(out_str,"$%4.4X",value);

    ui->AssAdresseIn->setText(out_str);
    ui->AssMnemonicIn->clear();
    ui->AssAdressierungIn->clear();
    ui->AssMnemonicIn->setFocus();
}

void DebuggerWindow::on_AssMnemonicIn_textChanged(const QString &arg1)
{
    if(arg1.length() == 3)
    {
        if(FindMnemonic(arg1))
        {
            ui->AssMnemonicIn->setText(ui->AssMnemonicIn->text().toUpper());
            ui->AssAdressierungIn->clear();
            ui->AssAdressierungIn->setFocus();
        }
        else
        {
            QMessageBox::warning(this,"Eingabefehler...","Unbekanntes Mnemonic !");
            ui->AssMnemonicIn->setFocus();
            ui->AssMnemonicIn->selectAll();
        }
    }
}

bool DebuggerWindow::FindMnemonic(QString mnemonic, unsigned char *opcode, int *opcode_anzahl)
{
    bool found = false;
    int count = 0;

    for(int i=0;i<255;i++)
    {
        if(mnemonic.toUpper() == QString(CpuOPC).mid(i*3,3))
        {
            found = true;
            if(opcode != 0) opcode[count++] = i;
        }
    }
    if(opcode_anzahl != 0) *opcode_anzahl = count;
    return found;
}

bool DebuggerWindow::FindAdressierung(QString adr_string, unsigned char *adr_typ, unsigned short *adr_wert)
{
    unsigned int integer;
    bool ok;

    QString in_str = adr_string.toUpper();

    if(in_str == "")
    {
        *adr_typ = 0;
        *adr_wert = 0;
        return true;
    }


    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    integer = in_str.toUShort(&ok,0);
    if(ok)
    {
        if(integer <= 0xFF)
        {
            *adr_typ = 3;
            *adr_wert = (unsigned short)integer;
            return true;
        }

        if(integer <= 0xFFFF)
        {
            *adr_typ = 2;
            *adr_wert = (unsigned short)integer;
            return true;
        }
    }

    if(in_str.left(1) == "#")
    {
        in_str.remove(0,1);
        if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
        integer = in_str.toUShort(&ok,0);
        if(ok)
        {
            if(integer <= 0xFF)
            {
                *adr_typ = 1;
                *adr_wert = (unsigned short)integer;
                return true;
            }
            else return false;
        }
        else return false;
    }

    if(in_str.right(2) == ",X")
    {
        QString tmp_str = in_str;
        tmp_str.remove(tmp_str.length()-2,2);
        if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
        integer = tmp_str.toUShort(&ok,0);
        if(ok)
        {
            if(integer <= 0xFF)
            {
                *adr_typ = 6;
                *adr_wert = (unsigned short)integer;
                return true;
            }

            if(integer <= 0xFFFF)
            {
                *adr_typ = 4;
                *adr_wert = (unsigned short)integer;
                return true;
            }
            return false;
        }
    }

    if(in_str.right(2) == ",Y")
    {
        QString tmp_str = in_str;
        tmp_str.remove(tmp_str.length()-2,2);
        if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
        integer = tmp_str.toUShort(&ok,0);
        if(ok)
        {
            if(integer <= 0xFF)
            {
                *adr_typ = 11;
                *adr_wert = (unsigned short)integer;
                return true;
            }

            if(integer <= 0xFFFF)
            {
                *adr_typ = 5;
                *adr_wert = (unsigned short)integer;
                return true;
            }
            return false;
        }
    }

    if((in_str.left(1) == "(") && (in_str.right(3) == ",X)"))
    {
        QString tmp_str = in_str;
        tmp_str = tmp_str.mid(1,tmp_str.length()-4);
        if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
        integer = tmp_str.toUShort(&ok,0);
        if(ok)
        {
            if(integer <= 0xFF)
            {
                *adr_typ = 7;
                *adr_wert = (unsigned short)integer;
                return true;
            }
            else return false;
        }
        else return false;
    }

    if((in_str.left(1) == "(") && (in_str.right(3) == "),Y"))
    {
        QString tmp_str = in_str;
        tmp_str = tmp_str.mid(1,tmp_str.length()-4);
        if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
        integer = tmp_str.toUShort(&ok,0);
        if(ok)
        {
            if(integer <= 0xFF)
            {
                *adr_typ = 8;
                *adr_wert = (unsigned short)integer;
                return true;
            }
            else return false;
        }
        else return false;
    }

    if((in_str.left(1) == "(") && (in_str.right(1) == ")"))
    {
        QString tmp_str = in_str;
        tmp_str = tmp_str.mid(1,tmp_str.length()-2);
        if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
        integer = tmp_str.toUShort(&ok,0);
        if(ok)
        {
            if(integer <= 0xFFFF)
            {
                *adr_typ = 10;
                *adr_wert = (unsigned short)integer;
                return true;
            }
            else return false;
        }
        else return false;
    }
    return false;
}

void DebuggerWindow::on_AssAdressierungIn_returnPressed()
{
    unsigned short ass_adress;
    unsigned short new_adress;

    if(Assemble(ui->AssAdresseIn->text(),ui->AssMnemonicIn->text(),ui->AssAdressierungIn->text(),&ass_adress,&new_adress))
    {
        char out_str[10];
        sprintf(out_str,"$%4.4X",new_adress);
        ui->AssAdresseIn->setText(QString(out_str));
        ui->AssMnemonicIn->clear();
        ui->AssAdressierungIn->clear();
        ui->AssMnemonicIn->setFocus();

        FillDisassemblerList(ass_adress,false);
        memory_window->update();
    }
    else
    {
        switch(new_adress)
        {
        case 0:
            QMessageBox::warning(this,tr("Fehler...!"),tr("Fehlerhafte Adresse."));
            ui->AssAdresseIn->selectAll();
            ui->AssAdresseIn->setFocus();
            break;
        case 1:
            QMessageBox::warning(this,tr("Fehler...!"),tr("Unbekanntes Mnemonic."));
            ui->AssMnemonicIn->selectAll();
            ui->AssMnemonicIn->setFocus();
            break;
        case 2:
            QMessageBox::warning(this,tr("Fehler...!"),tr("Unbekannte Adressierung."));
            ui->AssAdressierungIn->selectAll();
            ui->AssAdressierungIn->setFocus();
            break;
        case 3:
            QMessageBox::warning(this,tr("Fehler...!"),tr("Spungweite für Branchbefehl ist zu groß."));
            ui->AssAdressierungIn->selectAll();
            ui->AssAdressierungIn->setFocus();
            break;
        case 4:
            QMessageBox::warning(this,tr("Fehler...!"),tr("Dieser Opcode unterstützt nicht diese Adressierung."));
            ui->AssAdressierungIn->selectAll();
            ui->AssAdressierungIn->setFocus();
            break;
        }
    }
}

bool DebuggerWindow::Assemble(QString adresse, QString mnemonic, QString adressierung, unsigned short *ass_adress, unsigned short *new_adress)
{
    unsigned int start_adr;
    unsigned char opc[16];
    int opc_anz;
    unsigned char adr_typ;
    unsigned short adr_wert;

    bool ok;

    QString in_str = adresse;
    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    start_adr = in_str.toUShort(&ok,0);
    if(!ok)
    {
        *new_adress = 0;
        return false;
    }

    if(!FindMnemonic(mnemonic,opc,&opc_anz))
    {
        *new_adress = 1;
        return false;
    }

    if(!FindAdressierung(adressierung,&adr_typ,&adr_wert))
    {
        *new_adress = 2;
        return false;
    }

    *ass_adress = start_adr;

    if((opc[0]==0x90)||(opc[0]==0xB0)||(opc[0]==0xF0)||(opc[0]==0x30)||(opc[0]==0xD0)||(opc[0]==0x10)||(opc[0]==0x50)||(opc[0]==0x70))
    {
        if((adr_typ == 2) || (adr_typ == 3))
        {
            signed short jmp_adress = adr_wert - (unsigned short)start_adr -2;
            if((jmp_adress < -128) || (jmp_adress > 127))
            {
                *new_adress = 3;
                return false;
            }

            if(AktSource > 0)
            {
                c64->floppy[AktFloppyNr]->WriteByte(start_adr,opc[0]);
                c64->floppy[AktFloppyNr]->WriteByte(start_adr+1,(unsigned char)jmp_adress);
            }
            else
            {
                c64->WriteC64Byte(start_adr,opc[0]);
                c64->WriteC64Byte(start_adr+1,(unsigned char)jmp_adress);
            }

            *new_adress = start_adr + 2;
            return true;
        }
    }

    for(int i=0;i<opc_anz;i++)
    {
        if((CpuOPCInfo[opc[i]]>>4) == adr_typ)
        {
            switch (adr_typ)
            {
            case 0:
                if(AktSource > 0)
                {
                    c64->floppy[AktFloppyNr]->WriteByte(start_adr,opc[i]);
                }
                else
                {
                    c64->WriteC64Byte(start_adr,opc[i]);
                }
                *new_adress = start_adr+1;
                break;
            case 1: case 3: case 6: case 7: case 8: case 11:
                if(AktSource > 0)
                {
                    c64->floppy[AktFloppyNr]->WriteByte(start_adr,opc[i]);
                    c64->floppy[AktFloppyNr]->WriteByte(start_adr+1,(unsigned char)adr_wert);
                }
                else
                {
                    c64->WriteC64Byte(start_adr,opc[i]);
                    c64->WriteC64Byte(start_adr+1,(unsigned char)adr_wert);
                }
                *new_adress = start_adr+2;
                break;
            case 2: case 4: case 5: case 10:
                if(AktSource > 0)
                {
                    c64->floppy[AktFloppyNr]->WriteByte(start_adr,opc[i]);
                    c64->floppy[AktFloppyNr]->WriteByte(start_adr+1,(unsigned char)adr_wert);
                    c64->floppy[AktFloppyNr]->WriteByte(start_adr+2,(unsigned char)(adr_wert>>8));
                }
                else
                {
                    c64->WriteC64Byte(start_adr,opc[i]);
                    c64->WriteC64Byte(start_adr+1,(unsigned char)adr_wert);
                    c64->WriteC64Byte(start_adr+2,(unsigned char)(adr_wert>>8));
                }
                *new_adress = start_adr+3;
                break;
            }
            return true;
        }
    }

    *new_adress = 4;
    return false;
}

void DebuggerWindow::on_AnimationStart_clicked()
{
    ClearAllBreakpointBackcolors();

    ui->OneOpcode->setEnabled(false);
    ui->OneZyklus->setEnabled(false);
    ui->AnimationStart->setEnabled(false);
    ui->AnimationStop->setEnabled(true);
    ui->DisAssScroll->setEnabled(false);

    ui->AssAdresseIn->setEnabled(false);
    ui->AssMnemonicIn->setEnabled(false);
    ui->AssAdressierungIn->setEnabled(false);
    ui->EingabeFeld->setVisible(false);

    c64->SetDebugAnimation(true);
}

void DebuggerWindow::on_AnimationStop_clicked()
{
    ui->OneOpcode->setEnabled(true);
    ui->OneZyklus->setEnabled(true);
    ui->AnimationStart->setEnabled(true);
    ui->AnimationStop->setEnabled(false);
    ui->DisAssScroll->setEnabled(true);

    ui->AssAdresseIn->setEnabled(true);
    ui->AssMnemonicIn->setEnabled(true);
    ui->AssAdressierungIn->setEnabled(true);

    c64->SetDebugAnimation(false);
}

void DebuggerWindow::on_AnimationSpeed_valueChanged(int value)
{
    if(value == 1) ui->anispeed_out->setText(QVariant(value).toString() + tr(" Zyklus/Sek"));
    else ui->anispeed_out->setText(QVariant(value).toString() + tr(" Zyklen/Sek"));
    c64->SetDebugAnimationSpeed(value);
}

void DebuggerWindow::on_DisAssTable_doubleClicked(const QModelIndex &index)
{
   ui->AssAdresseIn->setText(ui->DisAssTable->item(index.row(),0)->text());
   ui->AssMnemonicIn->setText(ui->DisAssTable->item(index.row(),2)->text());
   ui->AssAdressierungIn->setText(ui->DisAssTable->item(index.row(),3)->text());
}

void DebuggerWindow::on_DisAssScroll_valueChanged(int value)
{
    FillDisassemblerList(value,false);
}

void DebuggerWindow::on_AddBreakpoint_clicked()
{
    static int auto_num[FloppyAnzahl+1] = {1,1,1,1,1};
    BREAK_GROUP *bg;
    int index;

    if(AktSource > 0)
    {
        index = c64->floppy[AktFloppyNr]->AddBreakGroup();
    }
    else index = c64->AddBreakGroup();

    if(index > -1)
    {
        if(AktSource > 0) bg = c64->floppy[AktFloppyNr]->GetBreakGroup(index);
        else bg = c64->GetBreakGroup(index);

        QString Name = tr("Haltepunkt (") + QVariant(auto_num[AktSource]++).toString() + ")";
        strcpy(bg->Name,Name.toLocal8Bit().constData());
        bg->Enable = true;

        AddBreakpointTreeRoot(Name,bg);
    }
}

void DebuggerWindow::on_DelBreakpoint_clicked()
{
    int bg_index;
    QString Name;

    if(ui->BreakpointTree->selectedItems().count() == 1)
    {
        QTreeWidgetItem *item = ui->BreakpointTree->selectedItems()[0];

        if(item->parent() == 0)
        {
            bg_index = ui->BreakpointTree->indexOfTopLevelItem(item);
            Name = item->text(0);
        }
        else
        {
            bg_index = ui->BreakpointTree->indexOfTopLevelItem(item->parent());
            Name = item->parent()->text(0);
        }

        if(QMessageBox::Yes == QMessageBox::question(this,tr("Haltepunkt löschen..."),tr("Möchten Sie den folgenden Haltepunkt löschen?\n") + ">> " + Name + " <<",QMessageBox::Yes | QMessageBox::No))
        {
            ui->BreakpointTree->takeTopLevelItem(bg_index);
            if(AktSource > 0) c64->floppy[AktFloppyNr]->DelBreakGroup(bg_index);
            else c64->DelBreakGroup(bg_index);
        }
    }
}

void DebuggerWindow::AddBreakpointTreeRoot(QString name,BREAK_GROUP *bg)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(0);
    item->setText(0,name);
    item->setForeground(0,QBrush(QColor(200,0,0)));
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    item->setCheckState(0,Qt::Checked);
    item->setDisabled(false);
    item->setToolTip(0,tr("Der Name des Haltepunkts kann frei gewählt werden."));
    ui->BreakpointTree->addTopLevelItem(item);

    BreakPointUpdateEnable = false;
    AddBreakpointTreeChild(item,bg->iPC,bg->bPC,tr("Wenn der Wert gleich dem Programm Counter (PC) ist."));
    AddBreakpointTreeChild(item,bg->iAC,bg->bAC,tr("Wenn der Wert gleich dem Accu Register (AC) ist."));
    AddBreakpointTreeChild(item,bg->iXR,bg->bXR,tr("Wenn der Wert gleich dem X Register (XR) ist."));
    AddBreakpointTreeChild(item,bg->iYR,bg->bYR,tr("Wenn der Wert gleich dem Y Register (YR) ist."));
    AddBreakpointTreeChild(item,bg->iRAdresse,bg->bRAdresse,tr("Wenn ein Lesezugriff an dieser Adresse statt findet."));
    AddBreakpointTreeChild(item,bg->iWAdresse,bg->bWAdresse,tr("Wenn ein Schreibzugriff an dieser Adresse statt findet."));
    AddBreakpointTreeChild(item,bg->iRWert,bg->bRWert,tr("Wenn aus einer Adresse dieser Wert ausgelesen wird."));
    AddBreakpointTreeChild(item,bg->iWWert,bg->bWWert,tr("Wenn in einer Adresse dieser Wert geschrieben wird."));
    if(AktSource == 0)
    {
        AddBreakpointTreeChild(item,bg->iRZ,bg->bRZ,tr("Wenn der Wert gleich der Aktuellen Rasterzeile ist."));
        AddBreakpointTreeChild(item,bg->iRZZyklus,bg->bRZZyklus,tr("Wenn der Wert gleich dem Aktuellen Zyklus in einer Rasterzeile ist."));
    }
    BreakPointUpdateEnable = true;
    c64->UpdateBreakGroup();
}

void DebuggerWindow::AddBreakpointTreeChild(QTreeWidgetItem *parent, unsigned short value, unsigned char checked, QString tooltip)
{
    QTreeWidgetItem *item = new QTreeWidgetItem(parent);
    item->setText(1,QVariant(value).toString());
    item->setForeground(0,QBrush(QColor(0,0,0)));
    item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsSelectable | Qt::ItemIsEditable);
    if(checked) item->setCheckState(0,Qt::Checked);
    else item->setCheckState(0,Qt::Unchecked);
    item->setDisabled(false);
    item->setToolTip(0,tooltip);
    item->setToolTip(1,tr("Doppelklick um Werte zu verändern."));
    item->setBackgroundColor(1,QColor(200,200,255));
    item->setForeground(1,QColor(200,0,0));
    parent->addChild(item);
}

void DebuggerWindow::on_BreakpointTree_itemChanged(QTreeWidgetItem *item, int column)
{
    int bg_index;
    int child_index;

    if(item->parent() == 0)
    {
        /// TopLevel Item (Breakpointname) ///

        bg_index = ui->BreakpointTree->indexOfTopLevelItem(item);
        BREAK_GROUP *bg;
        if(AktSource > 0)
        {
            bg = c64->floppy[AktFloppyNr]->GetBreakGroup(bg_index);
        }
        else bg = c64->GetBreakGroup(bg_index);

        strcpy(bg->Name,item->text(0).toLocal8Bit().constData());
        bg->Enable = item->checkState(0);
        c64->UpdateBreakGroup();
    }
    else
    {
        /// Child Item ///
        bg_index = ui->BreakpointTree->indexOfTopLevelItem(item->parent());
        child_index = item->parent()->indexOfChild(item);

        BREAK_GROUP *bg;
        if(AktSource > 0)
        {
            bg = c64->floppy[AktFloppyNr]->GetBreakGroup(bg_index);
        }
        else bg = c64->GetBreakGroup(bg_index);

        if(column == 0)
        {
            QString tmp_str = item->text(1);
            bool ok;
            unsigned short integer;
            char str_00[10];

            switch(child_index)
            {
            case 0:
                item->setText(0,"PC:");

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        bg->iPC = integer;
                        sprintf(str_00,"$%4.4X",integer);
                        item->setText(1,QString(str_00));
                        item->setBackgroundColor(1,QColor(200,200,255));
                        item->setForeground(1,QColor(200,0,0));
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bPC = item->checkState(0);

                break;
            case 1:
                item->setText(0,"AC:");

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        if(integer <= 0xff)
                        {
                            bg->iAC = integer;
                            sprintf(str_00,"$%2.2X",integer);
                            item->setText(1,QString(str_00));
                            item->setBackgroundColor(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackgroundColor(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bAC = item->checkState(0);
                break;
            case 2:
                item->setText(0,"XR:");

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        if(integer <= 0xff)
                        {
                            bg->iXR = integer;
                            sprintf(str_00,"$%2.2X",integer);
                            item->setText(1,QString(str_00));
                            item->setBackgroundColor(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackgroundColor(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bXR = item->checkState(0);
                break;
            case 3:
                item->setText(0,"YR:");

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        if(integer <= 0xff)
                        {
                            bg->iYR = integer;
                            sprintf(str_00,"$%2.2X",integer);
                            item->setText(1,QString(str_00));
                            item->setBackgroundColor(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackgroundColor(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bYR = item->checkState(0);
                break;
            case 4:
                item->setText(0,tr("Lesen von Adresse:"));

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        bg->iRAdresse = integer;
                        sprintf(str_00,"$%4.4X",integer);
                        item->setText(1,QString(str_00));
                        item->setBackgroundColor(1,QColor(200,200,255));
                        item->setForeground(1,QColor(200,0,0));
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bRAdresse = item->checkState(0);
                break;
            case 5:
                item->setText(0,tr("Schreiben in Adresse:"));

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        bg->iWAdresse = integer;
                        sprintf(str_00,"$%4.4X",integer);
                        item->setText(1,QString(str_00));
                        item->setBackgroundColor(1,QColor(200,200,255));
                        item->setForeground(1,QColor(200,0,0));
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bWAdresse = item->checkState(0);
                break;
            case 6:
                item->setText(0,tr("Lesen von Wert:"));

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        if(integer <= 0xff)
                        {
                            bg->iRWert = integer;
                            sprintf(str_00,"$%2.2X",integer);
                            item->setText(1,QString(str_00));
                            item->setBackgroundColor(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackgroundColor(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bRWert = item->checkState(0);
                break;
            case 7:
                item->setText(0,tr("Schreiben von Wert:"));

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        if(integer <= 0xff)
                        {
                            bg->iWWert = integer;
                            sprintf(str_00,"$%2.2X",integer);
                            item->setText(1,QString(str_00));
                            item->setBackgroundColor(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackgroundColor(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bWWert = item->checkState(0);
                break;
            case 8:
                item->setText(0,tr("Rasterzeile:"));

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        if(integer <= 311)  // PAL 0 - 311
                        {
                            bg->iRZ = integer;
                            sprintf(str_00,"%d",integer);
                            item->setText(1,QString(str_00));
                            item->setBackgroundColor(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackgroundColor(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bRZ = item->checkState(0);
                break;
            case 9:
                item->setText(0,tr("Zyklus:"));

                if(tmp_str != "")
                {
                    if(tmp_str.left(1) == "$") tmp_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
                    integer = tmp_str.toUShort(&ok,0);
                    if(ok)
                    {
                        if((integer > 0) && (integer <= 63))    // PAL 1 - 63
                        {
                            bg->iRZZyklus = integer;
                            sprintf(str_00,"%d",integer);
                            item->setText(1,QString(str_00));
                            item->setBackgroundColor(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackgroundColor(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackgroundColor(1,QColor(200,0,0));
                        item->setForeground(1,QColor(200,200,200));
                    }
                }
                else item->setCheckState(0,Qt::Unchecked);
                bg->bRZZyklus = item->checkState(0);
                break;
            default:
                item->setText(0,"???");
                break;
            }
        }
        if(BreakPointUpdateEnable)c64->UpdateBreakGroup();
    }
}

void DebuggerWindow::ClearAllBreakpointBackcolors(void)
{
    for(int i=0;i<ui->BreakpointTree->topLevelItemCount();i++)
    {
        ui->BreakpointTree->topLevelItem(i)->setBackgroundColor(0,QColor(255,255,255));
        ui->BreakpointTree->topLevelItem(i)->setBackgroundColor(1,QColor(255,255,255));
        ui->BreakpointTree->topLevelItem(i)->setText(1,"");
    }
}

void DebuggerWindow::BreakpointProc()
{
    NewBreakpointfound = true;
}

void DebuggerWindow::on_LoadBreakpoints_clicked()
{
    if(ui->BreakpointTree->topLevelItemCount() > 0)
        if(QMessageBox::No == QMessageBox::question(this,tr("Haltepunkte laden..."),tr("Es werden alle Haltepunkte gelöscht !\nMöchten Sie fortfahren?"),QMessageBox::Yes | QMessageBox::No))
            return;

    ui->BreakpointTree->clear();

    if(AktSource > 0) c64->floppy[AktFloppyNr]->DeleteAllBreakGroups();
    else c64->DeleteAllBreakGroups();

    QString filename = QFileDialog::getOpenFileName(this,tr("Haltepunkte öffnen"),"",tr("Emu64 Haltepunkt Datei ") + "(*.bpt)");
    if(filename != "")
    {
       int ret;
       if(AktSource > 0) ret = c64->floppy[AktFloppyNr]->LoadBreakGroups(filename.toLatin1().data());
       else ret = c64->LoadBreakGroups(filename.toLatin1().data());

       if(ret != 0)
       {
            switch(ret)
            {
            case -1:
                QMessageBox::warning(this,tr("Fehler..."),tr("Die Datei konnte nicht geöffnet werden."));
                break;
            case -2:
                QMessageBox::warning(this,tr("Fehler..."),tr("Es handelt sich nicht um eine Emu64 Datei."));
                break;
            case -3:
                QMessageBox::warning(this,tr("Fehler..."),tr("Diese Datei wurde mit einer neueren Emu64 Version erstellt."));
                break;
            case -4:
                QMessageBox::warning(this,tr("Fehler..."),tr("Diese Datei enthält keine Haltepunkte."));
                break;
            }
       }
       else
       {
           /// Alle Haltepunke ins TreeWidget einfügen ///
           if(AktSource > 0)
           {
               int anz = c64->floppy[AktFloppyNr]->GetBreakGroupAnz();
               for(int i=0;i<anz;i++)
               {
                   BREAK_GROUP *bg = c64->floppy[AktFloppyNr]->GetBreakGroup(i);
                   AddBreakpointTreeRoot(bg->Name,bg);
               }
           }
           else
           {
               int anz = c64->GetBreakGroupAnz();
               for(int i=0;i<anz;i++)
               {
                   BREAK_GROUP *bg = c64->GetBreakGroup(i);
                   AddBreakpointTreeRoot(bg->Name,bg);
               }
           }
       }
    }
}

void DebuggerWindow::on_SaveBreakpoints_clicked()
{
    if(ui->BreakpointTree->topLevelItemCount() == 0) return;

    QString filename;
    QString fileext;
    if(!getSaveFileName(this,tr("Haltepunkte speichern"),tr("Emu64 Haltepunkt Datei ") + "(*.bpt)",&filename,&fileext))
    {
        return;
    }

    if(AktSource > 0)
    {
        if(!c64->floppy[AktFloppyNr]->SaveBreakGroups(filename.toLatin1().data()))
            QMessageBox::warning(this,tr("Fehler..."),tr("Die Haltepunkte konnten nicht gespeichert werden."));
    }
    else
    {
        if(!c64->SaveBreakGroups(filename.toLatin1().data()))
          QMessageBox::warning(this,tr("Fehler..."),tr("Die Haltepunkte konnten nicht gespeichert werden."));
    }
}

void DebuggerWindow::on_DelAllBreakpoints_clicked()
{
    if(ui->BreakpointTree->topLevelItemCount() > 0)
        if(QMessageBox::Yes == QMessageBox::question(this,tr("Achtung..."),tr("Es werden alle Haltepunkte gelöscht !\nMöchten Sie fortfahren?"),QMessageBox::Yes | QMessageBox::No))
        {
            if(AktSource > 0) c64->floppy[AktFloppyNr]->DeleteAllBreakGroups();
            else c64->DeleteAllBreakGroups();
            ui->BreakpointTree->clear();
        }
}

void DebuggerWindow::on_HistoryScroll_valueChanged(int value)
{
    FillHistoryList(value);
}

void DebuggerWindow::on_ExportDisAss_clicked()
{
    bool ok;
    QString in_str;
    unsigned short value;
    unsigned short start,end;

    in_str = ui->ExportStartIn->text();
    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    value = in_str.toUShort(&ok,0);
    if(!ok)
    {
        QMessageBox::warning(this,tr("Eingabefehler..."),tr("Es wurde kein gültiges Zahlenformat benutzt ! [Export Von:]"));
        return;
    }
    start = value;

    in_str = ui->ExportEndIn->text();
    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    value = in_str.toUShort(&ok,0);
    if(!ok)
    {
        QMessageBox::warning(this,tr("Eingabefehler..."),tr("Es wurde kein gültiges Zahlenformat benutzt ! [Export Bis:]"));
        return;
    }
    end = value;

    if(start >= end)
    {
        QMessageBox::warning(this,tr("Fehler..."),tr("Startadresse muss kleiner sein als Endadresse."));
        return;
    }

    QString filename;
    QString fileext;
    if(!getSaveFileName(this,tr("Export..."),tr("Disassembler Listing") + "(*.txt);;" + tr("C64 Programm Datei") + "(*.prg)",&filename,&fileext))
    {
        return;
    }

    fileext = fileext.toUpper();
    if(fileext == "TXT")
    {
        if(!c64->ExportASM(filename.toLatin1().data(),start,end,AktSource))
            QMessageBox::warning(this,tr("Fehler..."),tr("Fehler beim speichern der Disassembler Datei."));
    }

    if(fileext == "PRG")
    {
        if((AktSource > 0) && (end > 0x07FF))
        {
            QMessageBox::warning(this,tr("Fehler..."),tr("Floppy RAM geht nur von $0000 - $07FF (2KB)."));
            return;
        }
        if(!c64->ExportPRG(filename.toLatin1().data(),start,end,AktSource))
            QMessageBox::warning(this,tr("Fehler..."),tr("Fehler beim speichern der Programm Datei."));
    }
}

void DebuggerWindow::onChangeFloppyStatus(void)
{
    RefreshGUI();
}

void DebuggerWindow::RefreshGUI(void)
{
    if(c64 == 0) return;

    C64CpuReg.REG_MASK = REG_MASK_ALL;
    c64->GetC64CpuReg(&C64CpuReg,&C64CpuIReg);

    if(AktSource > 0)
    {
        /// Floppy's ///

        FloppyCpuReg[AktFloppyNr].REG_MASK = REG_MASK_ALL;
        c64->floppy[AktFloppyNr]->GetCpuReg(&FloppyCpuReg[AktFloppyNr],&FloppyCpuIReg[AktFloppyNr]);

        /// Register Group ///
        ui->RegisterGroup->setTitle(tr("CPU 6502 Register"));
        ui->label_nmi->setVisible(false);
        ui->label_0314->setVisible(false);
        ui->label_0318->setVisible(false);
        ui->nmi_out->setVisible(false);
        ui->_0314_out->setVisible(false);
        ui->_0318_out->setVisible(false);

        /// IRegister Group ///
        ui->IRegisterGroup->setTitle(tr("CPU 6502 Interne Register (Emu64 Modell)"));
        ui->label_cpu_wait->setVisible(false);
        ui->wait_led->setVisible(false);

        /// Costum Chips ///
        ui->VIC->setVisible(false);
        ui->SID->setVisible(false);
        ui->CIA1->setVisible(false);
        ui->CIA2->setVisible(false);
        ui->VIA1->setVisible(true);
        ui->VIA2->setVisible(true);

        /// Leitungen ///
        ui->label_nmi_2->setVisible(false);
        ui->label_ba->setVisible(false);
        ui->label_exrom->setVisible(false);
        ui->label_game->setVisible(false);
        ui->nmi_led->setVisible(false);
        ui->rdy_led->setVisible(false);
        ui->exrom_led->setVisible(false);
        ui->game_led->setVisible(false);

        if(c64->floppy[AktSource-1]->GetEnableFloppy())
        {
            ui->label_floppy_off->setVisible(false);
            ui->RegisterGroup->setEnabled(true);
            ui->IRegisterGroup->setEnabled(true);
            ui->VIA1->setEnabled(true);
            ui->VIA2->setEnabled(true);
            ui->LeitungenGroup->setEnabled(true);
            ui->CycleGroup->setEnabled(true);
            ui->VerlaufGroup->setEnabled(true);
            ui->DisassGroup->setEnabled(true);
            ui->ExportGroup->setEnabled(true);
            ui->BreakGroup->setEnabled(true);

            /// Alle Haltepunke ins TreeWidget einfügen ///
            FillDisassemblerList(FloppyCpuIReg[AktFloppyNr].AktOpcodePC,false);
        }
        else
        {
            ui->label_floppy_off->setVisible(true);
            ui->RegisterGroup->setEnabled(false);
            ui->pc_out->setText("");
            ui->ac_out->setText("");
            ui->xr_out->setText("");
            ui->yr_out->setText("");
            ui->sp_out->setText("");
            ui->sr_out->setText("");
            ui->irq_out->setText("");
            ui->_0314_out->setText("");
            ui->sr_widget->SetValue(0);

            ui->IRegisterGroup->setEnabled(false);
            ui->opcode_pc_out->setText("");
            ui->adresse_out->setText("");
            ui->branch_adresse_out->setText("");
            ui->opcode_out->setText("");
            ui->pointer_out->setText("");
            ui->tmp_byte_out->setText("");
            ui->mnemonic_out->setText("");
            ui->micro_code_out->setText("");
            ui->MCodeHelp->setText("");

            ui->VIA1->setEnabled(false);
            ui->VIA2->setEnabled(false);

            ui->LeitungenGroup->setEnabled(false);
            ui->irq_led->setIcon(*iOff);
            ui->reset_led->setIcon(*iOff);

            ui->CycleGroup->setEnabled(false);
            ui->CycleCounter_Out->setText("");

            ui->VerlaufGroup->setEnabled(false);
            for(int i=0;i<HistoryZeilen;i++) ui->HistoryList->item(i)->setText("");

            ui->DisassGroup->setEnabled(false);
            for(int i=0;i<DisAssZeilen;i++)
            {
                DisAssPC[i]->setText("");
                DisAssPC[i]->setBackgroundColor(TableBackColor);
                DisAssMem[i]->setText("");
                DisAssMem[i]->setBackgroundColor(TableBackColor);
                DisAssMenmo[i]->setText("");
                DisAssMenmo[i]->setBackgroundColor(TableBackColor);
                DisAssAdr[i]->setText("");
                DisAssAdr[i]->setBackgroundColor(TableBackColor);
            }
            ui->ExportGroup->setEnabled(false);

            ui->BreakGroup->setEnabled(false);
        }
    }
    else
    {
        /// C64 ///

        /// Register Group ///
        ui->RegisterGroup->setTitle(tr("CPU 6510 Register"));
        ui->label_nmi->setVisible(true);
        ui->label_0314->setVisible(true);
        ui->label_0318->setVisible(true);
        ui->nmi_out->setVisible(true);
        ui->_0314_out->setVisible(true);
        ui->_0318_out->setVisible(true);

        /// IRegister Group ///
        ui->IRegisterGroup->setTitle(tr("CPU 6510 Interne Register (Emu64 Modell)"));
        ui->label_cpu_wait->setVisible(true);
        ui->wait_led->setVisible(true);

        /// Costum Chips ///
        ui->VIC->setVisible(true);
        ui->SID->setVisible(true);
        ui->CIA1->setVisible(true);
        ui->CIA2->setVisible(true);
        ui->VIA1->setVisible(false);
        ui->VIA2->setVisible(false);

        /// Leitungen ///
        ui->label_nmi_2->setVisible(true);
        ui->label_ba->setVisible(true);
        ui->label_exrom->setVisible(true);
        ui->label_game->setVisible(true);
        ui->nmi_led->setVisible(true);
        ui->rdy_led->setVisible(true);
        ui->exrom_led->setVisible(true);
        ui->game_led->setVisible(true);

        /// All Enable ///
        ui->label_floppy_off->setVisible(false);
        ui->RegisterGroup->setEnabled(true);
        ui->IRegisterGroup->setEnabled(true);
        ui->LeitungenGroup->setEnabled(true);
        ui->CycleGroup->setEnabled(true);
        ui->VerlaufGroup->setEnabled(true);
        ui->DisassGroup->setEnabled(true);
        ui->ExportDisAss->setEnabled(true);
        ui->BreakGroup->setEnabled(true);
        FillDisassemblerList(C64CpuIReg.AktOpcodePC,false);
    }

    ui->EingabeFeld->hide();
    UpdateRegister();
    FillHistoryList(ui->HistoryScroll->value());
    memory_window->UpdateMemoryList();
    vic_window->UpdateOutputList();

    on_AnimationSpeed_valueChanged(ui->AnimationSpeed->value());
}

void DebuggerWindow::on_MemEdit_clicked()
{
    if(memory_window->isHidden())
    {
        memory_window->show();
        memory_window->UpdateMemoryList();
    }
    else memory_window->hide();
}

void DebuggerWindow::on_VIC_clicked()
{
    if(vic_window->isHidden())
    {
        vic_window->show();
        vic_window->UpdateOutputList();
    }
    else vic_window->hide();
}

void DebuggerWindow::on_man_lines_clicked(bool checked)
{
    c64->SetCpuExtLines(checked);

    if(!checked)
    {
        if(C64CpuIReg.IRQ) ui->irq_led->setIcon(*iOn);
        else ui->irq_led->setIcon(*iOff);
        if(C64CpuIReg.NMI) ui->nmi_led->setIcon(*iOn);
        else ui->nmi_led->setIcon(*iOff);
        if(C64CpuIReg.RDY) ui->rdy_led->setIcon(*iOn);
        else ui->rdy_led->setIcon(*iOff);
    }
    else
    {
        ui->rdy_led->setChecked(true);
        ui->rdy_led->setIcon(*iOn);
        c64->SetExtRDY(true);

        ui->irq_led->setChecked(false);
        ui->irq_led->setIcon(*iOff);

        ui->nmi_led->setChecked(false);
        ui->nmi_led->setIcon(*iOff);
    }
}

void DebuggerWindow::on_rdy_led_clicked(bool checked)
{
    if(ui->man_lines->checkState())
    {
        c64->SetExtRDY(checked);
        if(checked) ui->rdy_led->setIcon(*iOn);
        else ui->rdy_led->setIcon(*iOff);
    }
}

void DebuggerWindow::on_irq_led_clicked(bool checked)
{
    if(ui->man_lines->checkState())
    {
        if(checked) c64->cpu->TriggerInterrupt(EXT_IRQ);
        else c64->cpu->ClearInterrupt(EXT_IRQ);
        if(checked) ui->irq_led->setIcon(*iOn);
        else ui->irq_led->setIcon(*iOff);
    }
}

void DebuggerWindow::on_nmi_led_clicked(bool checked)
{
    if(ui->man_lines->checkState())
    {
        if(checked) c64->cpu->TriggerInterrupt(EXT_NMI);
        else c64->cpu->ClearInterrupt(EXT_NMI);
        if(checked) ui->nmi_led->setIcon(*iOn);
        else ui->nmi_led->setIcon(*iOff);
    }
}
