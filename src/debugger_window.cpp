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
// Letzte Änderung am 01.04.2020                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QStyle>

#include "./debugger_window.h"
#include "./ui_debugger_window.h"
#include "./micro_code_tbl_6510.h"
#include "./micro_code_string_tbl_6510.h"

DebuggerWindow::DebuggerWindow(QWidget* parent, QSettings* ini) :
    QDialog(parent),
    ui(new Ui::DebuggerWindow),
    memory_window(nullptr),
    vic_window(nullptr),
    iec_window(nullptr)
{    
    this->ini = ini;
    c64 = nullptr;

    current_source = 0;

    table_back_color = QColor(255,255,255);
    table_position_color = QColor(255,200,200);

    old_adresse = 0;
    old_make_idx = 0;

    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

    memory_window = new MemoryWindow(this);
    memory_window->ChangeSource(0);

    vic_window = new DebuggerVicWindow(this);
    iec_window = new DebuggerIECWindow(this);

    new_refresh = false;
    timer1 = new QTimer(this);
    connect(timer1, SIGNAL(timeout()), this, SLOT(onTimerAnimationRefresh()));
    timer1->setInterval(10);

    icon_off = new QIcon(":/grafik/blue_led_off.png");
    icon_on = new QIcon(":/grafik/blue_led_on.png");
    ui->EingabeFeld->hide();

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/lucon.ttf");
    QFont font1("Lucida Console",8);

    ui->MCodeHelp->setFont(font1);

    ui->DisAssTable->setColumnCount(4);
    ui->DisAssTable->setColumnWidth(0, 44);
    ui->DisAssTable->setColumnWidth(1, 70);
    ui->DisAssTable->setColumnWidth(2, 30);
    ui->DisAssTable->setColumnWidth(3, 57);

    ui->DisAssTable->setFont(font1);
    ui->DisAssTable->setRowCount(DISASS_ROW);

    ui->AssAdresseIn->setFont(font1);
    ui->AssMnemonicIn->setFont(font1);
    ui->AssAdressierungIn->setFont(font1);

    ui->BreakpointTree->setColumnCount(2);
    ui->BreakpointTree->setColumnWidth(0, 175);
    ui->BreakpointTree->setColumnWidth(1, 50);
    new_breakpoint_found = false;

    ui->HistoryList->setFont(font1);
    for(int i=0; i<HISTORY_ROW; i++)
    {
        QListWidgetItem *item = new QListWidgetItem(ui->HistoryList);
        item->setText(QVariant(i).toString());
        ui->HistoryList->addItem(item);
    }

    for(int i=0; i<DISASS_ROW; i++)
    {
        view_code_address[i] = 0;
        disass_pc[i] = new QTableWidgetItem();
        disass_pc[i]->setBackground(table_back_color);
        ui->DisAssTable->setItem(i, 0, disass_pc[i]);
        disass_memory[i] = new QTableWidgetItem();
        disass_memory[i]->setBackground(table_back_color);
        ui->DisAssTable->setItem(i, 1, disass_memory[i]);
        disass_mnemonic[i] = new QTableWidgetItem();
        disass_mnemonic[i]->setBackground(table_back_color);
        ui->DisAssTable->setItem(i, 2, disass_mnemonic[i]);
        disass_addressing[i] = new QTableWidgetItem();
        disass_addressing[i]->setBackground(table_back_color);
        ui->DisAssTable->setItem(i, 3, disass_addressing[i]);
    }

    connect(ui->sr_widget, SIGNAL(ChangeValue(uint8_t)), this, SLOT(onSr_widget_ValueChange(uint8_t)));
    connect(ui->pc_out, SIGNAL(clicked(LabelWidgetMod*)), this, SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->sp_out, SIGNAL(clicked(LabelWidgetMod*)), this, SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->ac_out, SIGNAL(clicked(LabelWidgetMod*)), this, SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->xr_out, SIGNAL(clicked(LabelWidgetMod*)), this, SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->yr_out, SIGNAL(clicked(LabelWidgetMod*)), this, SLOT(onReg_label_clicked(LabelWidgetMod*)));
    connect(ui->sr_out, SIGNAL(clicked(LabelWidgetMod*)), this, SLOT(onReg_label_clicked(LabelWidgetMod*)));

    current_edit_reg = -1;

    rw_string = QStringList() << "[READ]" << "[WRITE]";

    connect(ui->DisAssTable, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(onShowContextMenu(QPoint)));

    ////////// Load from INI ///////////
    if(ini != nullptr)
    {
        ini->beginGroup("DebuggerWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        ini->endGroup();
    }
    ////////////////////////////////////

    timer1->start();
}

DebuggerWindow::~DebuggerWindow()
{
    ////////// Save to INI ///////////
    if(ini != nullptr)
    {
        ini->beginGroup("DebuggerWindow");
        if(is_one_showed) ini->setValue("Geometry", saveGeometry());
        if(isHidden()) ini->setValue("Show", false);
        else ini->setValue("Show", true);
        ini->endGroup();
    }
    ////////////////////////////////////

    delete icon_off;
    delete icon_on;
    delete timer1;
    delete ui;

    if(memory_window != nullptr) delete memory_window;
    if(vic_window != nullptr) delete vic_window;
    if(iec_window != nullptr) delete iec_window;
}

void DebuggerWindow::AnimationRefreshProc()
{
    new_refresh = true;
}

void DebuggerWindow::onTimerAnimationRefresh()
{
    if(new_refresh)
    {
        new_refresh = false;
        if(c64 != nullptr)
        {
            if(current_source > 0)
            {
                floppy_cpu_reg[currnet_floppy_nr].reg_mask = REG_MASK_ALL;
                c64->floppy1541[currnet_floppy_nr]->GetCpuReg(&floppy_cpu_reg[currnet_floppy_nr], &floppy_cpu_ireg[currnet_floppy_nr]);
                FillDisassemblyList(floppy_cpu_ireg[currnet_floppy_nr].current_opcode_pc, false);
            }
            else
            {
                c64_cpu_reg.reg_mask = REG_MASK_ALL;
                c64->GetC64CpuReg(&c64_cpu_reg,&c64_cpu_ireg);
                FillDisassemblyList(c64_cpu_ireg.current_opcode_pc,false);
            }
        }

        UpdateRegister();
        FillHistoryList(static_cast<uint8_t>(ui->HistoryScroll->value()));
        memory_window->UpdateMemoryList();
        vic_window->UpdateOutputList();
        iec_window->UpdateSignals();
    }

    if(new_breakpoint_found)
    {
        new_breakpoint_found = false;
        on_AnimationStop_clicked();

        for(int i=0;i<c64->GetBreakGroupCount();i++)
        {
            BREAK_GROUP *bg = c64->GetBreakGroup(i);
            if(bg->bTrue)
            {
                ui->ChangeSource->setCurrentIndex(0);
                if(i < ui->BreakpointTree->topLevelItemCount())
                {
                    ui->BreakpointTree->topLevelItem(i)->setBackground(0, QColor(0, 255, 0));
                    ui->BreakpointTree->topLevelItem(i)->setBackground(1, QColor(1, 255, 0));
                    ui->BreakpointTree->topLevelItem(i)->setText(1, tr("ERFÜLLT !"));
                }
            }
        }

        for(int i=0; i<MAX_FLOPPY_NUM; i++)
        {
            if(c64->floppy1541[i]->GetEnableFloppy())
            {
                for(int ii=0; ii<c64->floppy1541[i]->GetBreakGroupCount(); ii++)
                {
                    BREAK_GROUP *bg = c64->floppy1541[i]->GetBreakGroup(ii);
                    if(bg->bTrue)
                    {
                        ui->ChangeSource->setCurrentIndex(i+1);
                        if(ii < ui->BreakpointTree->topLevelItemCount())
                        {
                            ui->BreakpointTree->topLevelItem(ii)->setBackground(0, QColor(0, 255, 0));
                            ui->BreakpointTree->topLevelItem(ii)->setBackground(1, QColor(1, 255, 0));
                            ui->BreakpointTree->topLevelItem(ii)->setText(1, tr("ERFÜLLT !"));
                        }
                    }
                }
            }
        }
        show();
    }
}

void DebuggerWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    ui->sr_widget->RetranslateUi();

    this->update();
    this->UpdateRegister();

    vic_window->RetranslateUi();
    memory_window->RetranslateUi();
    iec_window->RetranslateUi();
}

void DebuggerWindow::SetC64Pointer(C64Class *c64)
{
    this->c64 = c64;
    memory_window->SetC64Pointer(c64);
    vic_window->SetC64Pointer(c64);
    iec_window->SetC64Pointer(c64);
    this->c64->AnimationRefreshProc = bind(&DebuggerWindow::AnimationRefreshProc, this);
    this->c64->BreakpointProc = bind(&DebuggerWindow::BreakpointProc, this);
}

void DebuggerWindow::showEvent(QShowEvent*)
{
    is_one_showed = true;
    c64->SetDebugMode(true);
    RefreshGUI();
}

void DebuggerWindow::hideEvent(QHideEvent*)
{
    /// HIDE ///

    memory_window->hide();
    vic_window->hide();
    iec_window->hide();

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
    if(c64 == nullptr) return;

    char str00[1024];

    if(current_source > 0)
    {
        /// Floppy's ///

        if(!c64->floppy1541[currnet_floppy_nr]->GetEnableFloppy()) return;

        sprintf(str00,"$%4.4X", floppy_cpu_reg[currnet_floppy_nr].pc);
        ui->pc_out->setText(QString(str00));
        sprintf(str00,"$%2.2X", floppy_cpu_reg[currnet_floppy_nr].sp + 0x100);
        ui->sp_out->setText(QString(str00));
        sprintf(str00,"$%2.2X", floppy_cpu_reg[currnet_floppy_nr].ac);
        ui->ac_out->setText(QString(str00));
        sprintf(str00,"$%2.2X", floppy_cpu_reg[currnet_floppy_nr].xr);
        ui->xr_out->setText(QString(str00));
        sprintf(str00,"$%2.2X", floppy_cpu_reg[currnet_floppy_nr].yr);
        ui->yr_out->setText(QString(str00));
        sprintf(str00,"$%2.2X", floppy_cpu_reg[currnet_floppy_nr].sr);
        ui->sr_out->setText(QString(str00));
        ui->sr_widget->SetValue(floppy_cpu_reg[currnet_floppy_nr].sr);

        sprintf(str00,"$%4.4X", floppy_cpu_reg[currnet_floppy_nr].irq);
        ui->irq_out->setText(QString(str00));
        sprintf(str00,"$%4.4X", floppy_cpu_reg[currnet_floppy_nr].nmi);
        ui->nmi_out->setText(QString(str00));
        sprintf(str00,"$%4.4X", floppy_cpu_reg[currnet_floppy_nr]._0314);
        ui->_0314_out->setText(QString(str00));
        sprintf(str00,"$%4.4X", floppy_cpu_reg[currnet_floppy_nr]._0318);
        ui->_0318_out->setText(QString(str00));

        sprintf(str00,"$%4.4X", floppy_cpu_ireg[currnet_floppy_nr].current_opcode_pc);
        ui->opcode_pc_out->setText(QString(str00));
        sprintf(str00,"$%3.3X", floppy_cpu_ireg[currnet_floppy_nr].current_opcode);
        ui->opcode_out->setText(QString(str00));
        sprintf(str00,"$%4.4X", floppy_cpu_ireg[currnet_floppy_nr].address);
        ui->adresse_out->setText(QString(str00));
        sprintf(str00,"$%4.4X", floppy_cpu_ireg[currnet_floppy_nr].branch_address);
        ui->branch_adresse_out->setText(QString(str00));
        sprintf(str00,"$%2.2X", floppy_cpu_ireg[currnet_floppy_nr].pointer);
        ui->pointer_out->setText(QString(str00));
        sprintf(str00,"$%2.2X", floppy_cpu_ireg[currnet_floppy_nr].tmp_byte);
        ui->tmp_byte_out->setText(QString(str00));
        sprintf(str00,"%10.10d", floppy_cpu_ireg[currnet_floppy_nr].cycle_counter);
        ui->CycleCounter_Out->setText(QString(str00));

        sprintf(str00,"%3.3d ", floppy_cpu_ireg[currnet_floppy_nr].current_micro_code);
        ui->micro_code_out->setText(str00 + rw_string[micro_code_rw_table_6510[floppy_cpu_ireg[currnet_floppy_nr].current_micro_code]]);

        ui->MCodeHelp->setText(micro_code_string_table_6510[floppy_cpu_ireg[currnet_floppy_nr].current_micro_code]);
        ui->mnemonic_out->setText(QString(CpuOPC).mid(floppy_cpu_ireg[currnet_floppy_nr].current_opcode * 3, 3));

        if(floppy_cpu_ireg[currnet_floppy_nr].irq) ui->irq_led->setIcon(*icon_on);
        else ui->irq_led->setIcon(*icon_off);
        if(!floppy_cpu_ireg[currnet_floppy_nr].reset) ui->reset_led->setIcon(*icon_on);
        else ui->reset_led->setIcon(*icon_off);
    }
    else
    {
        /// C64 ///
        sprintf(str00, "$%4.4X", c64_cpu_reg.pc);
        ui->pc_out->setText(QString(str00));
        sprintf(str00, "$%2.2X", c64_cpu_reg.sp + 0x100);
        ui->sp_out->setText(QString(str00));
        sprintf(str00, "$%2.2X", c64_cpu_reg.ac);
        ui->ac_out->setText(QString(str00));
        sprintf(str00, "$%2.2X", c64_cpu_reg.xr);
        ui->xr_out->setText(QString(str00));
        sprintf(str00, "$%2.2X", c64_cpu_reg.yr);
        ui->yr_out->setText(QString(str00));
        sprintf(str00, "$%2.2X", c64_cpu_reg.sr);
        ui->sr_out->setText(QString(str00));
        ui->sr_widget->SetValue(c64_cpu_reg.sr);

        sprintf(str00, "$%4.4X", c64_cpu_reg.irq);
        ui->irq_out->setText(QString(str00));
        sprintf(str00, "$%4.4X", c64_cpu_reg.nmi);
        ui->nmi_out->setText(QString(str00));
        sprintf(str00, "$%4.4X", c64_cpu_reg._0314);
        ui->_0314_out->setText(QString(str00));
        sprintf(str00, "$%4.4X", c64_cpu_reg._0318);
        ui->_0318_out->setText(QString(str00));

        sprintf(str00, "$%4.4X", c64_cpu_ireg.current_opcode_pc);
        ui->opcode_pc_out->setText(QString(str00));
        sprintf(str00, "$%3.3X", c64_cpu_ireg.current_opcode);
        ui->opcode_out->setText(QString(str00));
        sprintf(str00, "$%4.4X", c64_cpu_ireg.address);
        ui->adresse_out->setText(QString(str00));
        sprintf(str00, "$%4.4X", c64_cpu_ireg.branch_address);
        ui->branch_adresse_out->setText(QString(str00));
        sprintf(str00, "$%2.2X", c64_cpu_ireg.pointer);
        ui->pointer_out->setText(QString(str00));
        sprintf(str00, "$%2.2X", c64_cpu_ireg.tmp_byte);
        ui->tmp_byte_out->setText(QString(str00));
        sprintf(str00,"%10.10d", c64_cpu_ireg.cycle_counter);
        ui->CycleCounter_Out->setText(QString(str00));

        sprintf(str00, "%3.3d ", c64_cpu_ireg.current_micro_code);
        ui->micro_code_out->setText(str00 + rw_string[micro_code_rw_table_6510[c64_cpu_ireg.current_micro_code]]);

        ui->MCodeHelp->setText(micro_code_string_table_6510[c64_cpu_ireg.current_micro_code]);
        ui->mnemonic_out->setText(QString(CpuOPC).mid(c64_cpu_ireg.current_opcode * 3, 3));

        if(c64_cpu_ireg.cpu_wait) ui->wait_led->setIcon(*icon_on);
        else ui->wait_led->setIcon(*icon_off);

        if(!ui->man_lines->checkState())
        {
            if(c64_cpu_ireg.irq) ui->irq_led->setIcon(*icon_on);
            else ui->irq_led->setIcon(*icon_off);
            if(c64_cpu_ireg.nmi) ui->nmi_led->setIcon(*icon_on);
            else ui->nmi_led->setIcon(*icon_off);
            if(c64_cpu_ireg.rdy) ui->rdy_led->setIcon(*icon_on);
            else ui->rdy_led->setIcon(*icon_off);
        }
        if(!c64_cpu_ireg.reset) ui->reset_led->setIcon(*icon_on);
        else ui->reset_led->setIcon(*icon_off);
        if(c64_cpu_ireg.exrom) ui->exrom_led->setIcon(*icon_on);
        else ui->exrom_led->setIcon(*icon_off);
        if(c64_cpu_ireg.game) ui->game_led->setIcon(*icon_on);
        else ui->game_led->setIcon(*icon_off);
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

void DebuggerWindow::onSr_widget_ValueChange(uint8_t value)
{
    on_AnimationStop_clicked();

    ui->EingabeFeld->hide();

    REG_STRUCT cpu_reg;
    cpu_reg.reg_mask = REG_MASK_SR;
    cpu_reg.sr = value;

    char str00[50];

    sprintf(str00,"$%2.2X",value);
    ui->sr_out->setText(str00);

    if(current_source > 0)
    {
        c64->floppy1541[currnet_floppy_nr]->SetCpuReg(&cpu_reg);
    }
    else c64->cpu->SetRegister(&cpu_reg);
}

void DebuggerWindow::onReg_label_clicked(LabelWidgetMod* label)
{
    on_AnimationStop_clicked();

    current_edit_reg = -1;

    if(label->objectName() == "pc_out") current_edit_reg = 0;
    if(label->objectName() == "sp_out") current_edit_reg = 1;
    if(label->objectName() == "ac_out") current_edit_reg = 2;
    if(label->objectName() == "xr_out") current_edit_reg = 3;
    if(label->objectName() == "yr_out") current_edit_reg = 4;
    if(label->objectName() == "sr_out") current_edit_reg = 5;

    ui->EingabeFeld->setGeometry(label->geometry());

    ui->EingabeFeld->setText(label->text());
    ui->EingabeFeld->selectAll();
    ui->EingabeFeld->show();
    ui->EingabeFeld->setFocus();
}

void DebuggerWindow::on_EingabeFeld_returnPressed()
{
    if(current_edit_reg == -1)
    {
        ui->EingabeFeld->hide();
        return;
    }

    bool ok;
    QString in_str = ui->EingabeFeld->text();
    uint16_t value;

    if(in_str.left(1) == "$") in_str.replace(0, 1, "0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt

    value = in_str.toUShort(&ok, 0);

    if(!ok)
    {
        QMessageBox::warning(this, tr("Eingabefehler..."), tr("Es wurde kein gültiges Zahlenformat benutzt !"));
        return;
    }

    REG_STRUCT cpu_reg;

    switch(current_edit_reg)
    {
    case 0:
        cpu_reg.reg_mask = REG_MASK_PC;
        cpu_reg.pc = value;
        break;
    case 1:
        if(value > 0x100) value -= 0x100;
        if(value > 0xFF)
        {
            QMessageBox::warning(this, tr("Eingabefehler..."), tr("Der Wert muss zwischen 0 und 511 liegen !"));
            return;
        }
        cpu_reg.reg_mask = REG_MASK_SP;
        cpu_reg.sp = static_cast<uint8_t>(value);
        break;
    case 2:
        if(value > 0xFF)
        {
            QMessageBox::warning(this, tr("Eingabefehler..."), tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        cpu_reg.reg_mask = REG_MASK_AC;
        cpu_reg.ac = static_cast<uint8_t>(value);
        break;
    case 3:
        if(value > 0xFF)
        {
            QMessageBox::warning(this, tr("Eingabefehler..."), tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        cpu_reg.reg_mask = REG_MASK_XR;
        cpu_reg.xr = static_cast<uint8_t>(value);
        break;
    case 4:
        if(value > 0xFF)
        {
            QMessageBox::warning(this, tr("Eingabefehler..."), tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        cpu_reg.reg_mask = REG_MASK_YR;
        cpu_reg.yr = static_cast<uint8_t>(value);
        break;
    case 5:
        if(value > 0xFF)
        {
            QMessageBox::warning(this, tr("Eingabefehler..."), tr("Der Wert muss zwischen 0 und 255 liegen !"));
            return;
        }
        cpu_reg.reg_mask = REG_MASK_SR;
        cpu_reg.sr = static_cast<uint8_t>(value);
        break;
    }

    if(current_source > 0)
    {
        c64->floppy1541[currnet_floppy_nr]->SetCpuReg(&cpu_reg);
        c64->floppy1541[currnet_floppy_nr]->GetCpuReg(&floppy_cpu_reg[currnet_floppy_nr], &floppy_cpu_ireg[currnet_floppy_nr]);
    }
    else
    {
        c64->cpu->SetRegister(&cpu_reg);
        c64->cpu->GetRegister(&c64_cpu_reg);
    }

    current_edit_reg = -1;
    ui->EingabeFeld->hide();
    UpdateRegister();
}

void DebuggerWindow::on_OneZyklus_clicked()
{
    ui->EingabeFeld->hide();
    ClearAllBreakpointBackcolors();
    c64->OneCycle();
}

void DebuggerWindow::on_OneOpcode_clicked()
{
    ui->EingabeFeld->hide();
    ClearAllBreakpointBackcolors();
    c64->OneOpcode(current_source);
}

void DebuggerWindow::on_CycleCounterReset_clicked()
{
    if(current_source > 0)
    {
        c64->floppy1541[currnet_floppy_nr]->ResetCycleCounter();
    }
    else
    {
        c64->ResetC64CycleCounter();
    }
    ui->CycleCounter_Out->setText("0000000000");
}

void DebuggerWindow::FillDisassemblyList(uint16_t address, bool new_refresh)
{
    uint16_t pc = address;
    char str00[50];
    uint16_t tmp;
    uint16_t word;
    uint8_t tmp1;
    bool find = false;
    int akt_make_idx = 0;

    if(current_source > 0)
    {
        if(!c64->floppy1541[current_source-1]->GetEnableFloppy()) return;
    }

    if(!new_refresh)
    {
        for(int i=0;i<DISASS_ROW;i++)
        {
            if(view_code_address[i] == address)
            {
                find = true;
                akt_make_idx = i;
            }
        }
    }

    if(find)
    {
        disass_pc[old_make_idx]->setBackground(table_back_color);
        disass_memory[old_make_idx]->setBackground(table_back_color);
        disass_mnemonic[old_make_idx]->setBackground(table_back_color);
        disass_addressing[old_make_idx]->setBackground(table_back_color);

        disass_pc[akt_make_idx]->setBackground(table_position_color);
        disass_memory[akt_make_idx]->setBackground(table_position_color);
        disass_mnemonic[akt_make_idx]->setBackground(table_position_color);
        disass_addressing[akt_make_idx]->setBackground(table_position_color);

        old_make_idx = akt_make_idx;
        pc = old_adresse;
    }
    else
    {
        disass_pc[old_make_idx]->setBackground(table_back_color);
        disass_memory[old_make_idx]->setBackground(table_back_color);
        disass_mnemonic[old_make_idx]->setBackground(table_back_color);
        disass_addressing[old_make_idx]->setBackground(table_back_color);

        disass_pc[0]->setBackground(table_position_color);
        disass_memory[0]->setBackground(table_position_color);
        disass_mnemonic[0]->setBackground(table_position_color);
        disass_addressing[0]->setBackground(table_position_color);

        old_make_idx = akt_make_idx = 0;
        pc = old_adresse = address;
    }

    uint8_t ram0;
    uint8_t ram1;
    uint8_t ram2;

    for(int i=0; i<DISASS_ROW; i++)
    {
        view_code_address[i] = pc;
        sprintf(str00, "$%4.4X", pc);
        disass_pc[i]->setText(str00);

        if(current_source > 0)
        {
            currnet_floppy_nr = current_source - 1;
            ram0 = c64->floppy1541[currnet_floppy_nr]->ReadByte(pc+0);
            ram1 = c64->floppy1541[currnet_floppy_nr]->ReadByte(pc+1);
            ram2 = c64->floppy1541[currnet_floppy_nr]->ReadByte(pc+2);
        }
        else
        {
            ram0 = c64->ReadC64Byte(pc+0);
            ram1 = c64->ReadC64Byte(pc+1);
            ram2 = c64->ReadC64Byte(pc+2);
        }

        disass_mnemonic[i]->setText(QString(CpuOPC).mid(ram0*3, 3));

        tmp = CpuOPCInfo[ram0];

        if(!(tmp&8))
        {
            disass_mnemonic[i]->setForeground(QColor(0,0,0));
        }
        else
        {
            disass_mnemonic[i]->setForeground(QColor(255,0,0));
        }

        tmp >>= 4;
        tmp &= 15;
        switch(tmp)
        {
        case 0:
            disass_addressing[i]->setText("");

            sprintf(str00,"%2.2X -- --",ram0);
            disass_memory[i]->setText(str00);
            pc++;
            break;
        case 1:
            sprintf(str00,"#$%2.2X",ram1);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            disass_memory[i]->setText(str00);
            pc+=2;
            break;
        case 2:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"$%4.4X",word);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            disass_memory[i]->setText(str00);
            pc+=3;
            break;
        case 3:
            sprintf(str00,"$%2.2X",ram1);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            disass_memory[i]->setText(str00);
            pc+=2;
            break;
        case 4:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"$%4.4X,X",word);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            disass_memory[i]->setText(str00);
            pc+=3;
            break;
        case 5:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"$%4.4X,Y",word);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            disass_memory[i]->setText(str00);
            pc+=3;
            break;
        case 6:
            sprintf(str00,"$%2.2X,X",ram1);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            disass_memory[i]->setText(str00);
            pc+=2;
            break;
        case 7:
            sprintf(str00,"($%2.2X,X)",ram1);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            disass_memory[i]->setText(str00);
            pc+=2;
            break;
        case 8:
            sprintf(str00,"($%2.2X),Y",ram1);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            disass_memory[i]->setText(str00);
            pc+=2;
            break;
        case 9:
            tmp1 = ram1;
            word = (pc+2)+tmp1;
            sprintf(str00,"$%4.4X",word);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            disass_memory[i]->setText(str00);
            pc+=2;
            break;
        case 10:
            word=ram1;
            word|=ram2<<8;
            sprintf(str00,"($%4.4X)",word);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X %2.2X",ram0,ram1,ram2);
            disass_memory[i]->setText(str00);
            pc+=3;
            break;
        case 11:
            sprintf(str00,"$%2.2X,Y",ram1);
            disass_addressing[i]->setText(str00);

            sprintf(str00,"%2.2X %2.2X --",ram0,ram1);
            disass_memory[i]->setText(str00);
            pc+=2;
            break;
        }
    }
}

void DebuggerWindow::FillHistoryList(uint8_t index)
{
    char str00[10];
    uint8_t hp;

    if(current_source > 0)
    {
        if(!c64->floppy1541[currnet_floppy_nr]->GetEnableFloppy()) return;
        hp = c64->floppy1541[currnet_floppy_nr]->HistoryPointer;
        for(int i=0; i<HISTORY_ROW; i++)
        {
            sprintf(str00, "$%4.4X", c64->floppy1541[currnet_floppy_nr]->History[hp--]);
            ui->HistoryList->item(i)->setText(QString(str00));
        }
    }
    else
    {
        hp = c64->cpu_pc_history_pos - index;
        for(int i=0; i<HISTORY_ROW; i++)
        {
            sprintf(str00, "$%4.4X", c64->cpu_pc_history[hp--]);
            ui->HistoryList->item(i)->setText(QString(str00));
        }
    }
}

void DebuggerWindow::on_ChangeSource_currentIndexChanged(int index)
{
    current_source = index;
    if(current_source > 0)
    {
        currnet_floppy_nr = current_source - 1;
        ui->BreakpointTree->clear();

        if(c64 == nullptr) return;
        break_point_update_enable = false;
        int anz = c64->floppy1541[currnet_floppy_nr]->GetBreakGroupCount();
        for(int i=0;i<anz;i++)
        {
            BREAK_GROUP *bg = c64->floppy1541[currnet_floppy_nr]->GetBreakGroup(i);
            AddBreakpointTreeRoot(bg->Name, bg);
        }
        break_point_update_enable = true;
    }
    else
    {
        /// Alle Haltepunke ins TreeWidget einfügen ///

        ui->BreakpointTree->clear();

        if(c64 == nullptr) return;
        break_point_update_enable = false;
        int anz = c64->GetBreakGroupCount();
        for(int i=0;i<anz;i++)
        {
            BREAK_GROUP *bg = c64->GetBreakGroup(i);
            AddBreakpointTreeRoot(bg->Name, bg);
        }
        break_point_update_enable = true;
    }

    ui->AssAdresseIn->setText("");
    ui->AssMnemonicIn->setText("");
    ui->AssAdressierungIn->setText("");
    memory_window->ChangeSource(current_source);
    RefreshGUI();
}

void DebuggerWindow::on_AssAdresseIn_returnPressed()
{
    bool ok;
    uint16_t value;
    QString in_str = ui->AssAdresseIn->text();

    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    value = in_str.toUShort(&ok,0);
    if(!ok)
    {
        QMessageBox::warning(this, tr("Eingabefehler..."), tr("Es wurde kein gültiges Zahlenformat benutzt !"));
        return;
    }

    char out_str[10];
    sprintf(out_str, "$%4.4X", value);

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

bool DebuggerWindow::FindMnemonic(QString mnemonic, uint8_t *opcode, uint8_t *opcode_count)
{
    bool found = false;
    uint8_t count = 0;

    for(int i=0; i<256; i++)
    {
        if(mnemonic.toUpper() == QString(CpuOPC).mid(i*3,3))
        {
            found = true;
            if(opcode != nullptr) opcode[count++] = static_cast<uint8_t>(i);
        }
    }
    if(opcode_count != nullptr) *opcode_count = count;
    return found;
}

bool DebuggerWindow::FindAddressing(QString address_string, uint8_t *address_type, uint16_t *address_value)
{
    uint32_t integer;
    bool ok;

    QString in_str = address_string.toUpper();

    if(in_str == "")
    {
        *address_type = 0;
        *address_value = 0;
        return true;
    }


    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    integer = in_str.toUShort(&ok,0);
    if(ok)
    {
        if(integer <= 0xFF)
        {
            *address_type = 3;
            *address_value = static_cast<uint16_t>(integer);
            return true;
        }

        if(integer <= 0xFFFF)
        {
            *address_type = 2;
            *address_value = static_cast<uint16_t>(integer);
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
                *address_type = 1;
                *address_value = static_cast<uint16_t>(integer);
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
                *address_type = 6;
                *address_value = static_cast<uint16_t>(integer);
                return true;
            }

            if(integer <= 0xFFFF)
            {
                *address_type = 4;
                *address_value = static_cast<uint16_t>(integer);
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
                *address_type = 11;
                *address_value = static_cast<uint16_t>(integer);
                return true;
            }

            if(integer <= 0xFFFF)
            {
                *address_type = 5;
                *address_value = static_cast<uint16_t>(integer);
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
                *address_type = 7;
                *address_value = static_cast<uint16_t>(integer);
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
                *address_type = 8;
                *address_value = static_cast<uint16_t>(integer);
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
                *address_type = 10;
                *address_value = static_cast<uint16_t>(integer);
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
    uint16_t ass_adress;
    uint16_t new_adress;

    if(Assemble(ui->AssAdresseIn->text(), ui->AssMnemonicIn->text(), ui->AssAdressierungIn->text(), &ass_adress, &new_adress))
    {
        char out_str[10];
        sprintf(out_str, "$%4.4X", new_adress);
        ui->AssAdresseIn->setText(QString(out_str));
        ui->AssMnemonicIn->clear();
        ui->AssAdressierungIn->clear();
        ui->AssMnemonicIn->setFocus();

        FillDisassemblyList(ass_adress, false);
        memory_window->update();
    }
    else
    {
        switch(new_adress)
        {
        case 0:
            QMessageBox::warning(this, tr("Fehler...!"), tr("Fehlerhafte Adresse."));
            ui->AssAdresseIn->selectAll();
            ui->AssAdresseIn->setFocus();
            break;
        case 1:
            QMessageBox::warning(this, tr("Fehler...!"), tr("Unbekanntes Mnemonic."));
            ui->AssMnemonicIn->selectAll();
            ui->AssMnemonicIn->setFocus();
            break;
        case 2:
            QMessageBox::warning(this, tr("Fehler...!"), tr("Unbekannte Adressierung."));
            ui->AssAdressierungIn->selectAll();
            ui->AssAdressierungIn->setFocus();
            break;
        case 3:
            QMessageBox::warning(this, tr("Fehler...!"), tr("Spungweite für Branchbefehl ist zu groß."));
            ui->AssAdressierungIn->selectAll();
            ui->AssAdressierungIn->setFocus();
            break;
        case 4:
            QMessageBox::warning(this, tr("Fehler...!"), tr("Dieser Opcode unterstützt nicht diese Adressierung."));
            ui->AssAdressierungIn->selectAll();
            ui->AssAdressierungIn->setFocus();
            break;
        }
    }
}

bool DebuggerWindow::Assemble(QString address, QString mnemonic, QString addressing, uint16_t *ass_address, uint16_t *new_adress)
{
    uint32_t start_address;
    uint8_t opcode[32];
    uint8_t opcode_count;
    uint8_t address_type;
    uint16_t address_value;

    bool ok;

    QString in_str = address;
    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    start_address = in_str.toUShort(&ok,0);
    if(!ok)
    {
        *new_adress = 0;
        return false;
    }

    if(!FindMnemonic(mnemonic,opcode,&opcode_count))
    {
        *new_adress = 1;
        return false;
    }

    if(!FindAddressing(addressing,&address_type,&address_value))
    {
        *new_adress = 2;
        return false;
    }

    *ass_address = static_cast<uint16_t>(start_address);

    if((opcode[0]==0x90)||(opcode[0]==0xB0)||(opcode[0]==0xF0)||(opcode[0]==0x30)||(opcode[0]==0xD0)||(opcode[0]==0x10)||(opcode[0]==0x50)||(opcode[0]==0x70))
    {
        if((address_type == 2) || (address_type == 3))
        {
            int16_t jmp_adress = static_cast<int16_t>(address_value) - static_cast<int16_t>(start_address) - 2;
            if((jmp_adress < -128) || (jmp_adress > 127))
            {
                *new_adress = 3;
                return false;
            }

            if(current_source > 0)
            {
                c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address), opcode[0]);
                c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address)+1, static_cast<uint8_t>(jmp_adress));
            }
            else
            {
                c64->WriteC64Byte(static_cast<uint16_t>(start_address), opcode[0]);
                c64->WriteC64Byte(static_cast<uint16_t>(start_address)+1, static_cast<uint8_t>(jmp_adress));
            }

            *new_adress = static_cast<uint16_t>(start_address) + 2;
            return true;
        }
    }

    for(int i=0;i<opcode_count;i++)
    {
        if((CpuOPCInfo[opcode[i]]>>4) == address_type)
        {
            switch (address_type)
            {
            case 0:
                if(current_source > 0)
                {
                    c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address), opcode[i]);
                }
                else
                {
                    c64->WriteC64Byte(static_cast<uint16_t>(start_address), opcode[i]);
                }
                *new_adress = static_cast<uint16_t>(start_address)+1;
                break;
            case 1: case 3: case 6: case 7: case 8: case 11:
                if(current_source > 0)
                {
                    c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address), opcode[i]);
                    c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address)+1, static_cast<uint8_t>(address_value));
                }
                else
                {
                    c64->WriteC64Byte(static_cast<uint16_t>(start_address), opcode[i]);
                    c64->WriteC64Byte(static_cast<uint16_t>(start_address)+1, static_cast<uint8_t>(address_value));
                }
                *new_adress = static_cast<uint16_t>(start_address)+2;
                break;
            case 2: case 4: case 5: case 10:
                if(current_source > 0)
                {
                    c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address), opcode[i]);
                    c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address)+1, static_cast<uint8_t>(address_value));
                    c64->floppy1541[currnet_floppy_nr]->WriteByte(static_cast<uint16_t>(start_address)+2, static_cast<uint8_t>(address_value>>8));
                }
                else
                {
                    c64->WriteC64Byte(static_cast<uint16_t>(start_address), opcode[i]);
                    c64->WriteC64Byte(static_cast<uint16_t>(start_address)+1, static_cast<uint8_t>(address_value));
                    c64->WriteC64Byte(static_cast<uint16_t>(start_address)+2, static_cast<uint8_t>(address_value>>8));
                }
                *new_adress = static_cast<uint16_t>(start_address)+3;
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
    FillDisassemblyList(static_cast<uint16_t>(value), false);
}

void DebuggerWindow::on_AddBreakpoint_clicked()
{
    static int auto_num[MAX_FLOPPY_NUM+1] = {1,1,1,1,1};
    BREAK_GROUP *bg;
    int index;

    if(current_source > 0)
    {
        index = c64->floppy1541[currnet_floppy_nr]->AddBreakGroup();
    }
    else index = c64->AddBreakGroup();

    if(index > -1)
    {
        if(current_source > 0) bg = c64->floppy1541[currnet_floppy_nr]->GetBreakGroup(index);
        else bg = c64->GetBreakGroup(index);

        QString Name = tr("Haltepunkt (") + QVariant(auto_num[current_source]++).toString() + ")";
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

        if(item->parent() == nullptr)
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
            if(current_source > 0) c64->floppy1541[currnet_floppy_nr]->DelBreakGroup(bg_index);
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

    break_point_update_enable = false;
    AddBreakpointTreeChild(item,bg->iPC,bg->bPC,tr("Wenn der Wert gleich dem Programm Counter (PC) ist."));
    AddBreakpointTreeChild(item,bg->iAC,bg->bAC,tr("Wenn der Wert gleich dem Accu Register (AC) ist."));
    AddBreakpointTreeChild(item,bg->iXR,bg->bXR,tr("Wenn der Wert gleich dem X Register (XR) ist."));
    AddBreakpointTreeChild(item,bg->iYR,bg->bYR,tr("Wenn der Wert gleich dem Y Register (YR) ist."));
    AddBreakpointTreeChild(item,bg->iRAdresse,bg->bRAdresse,tr("Wenn ein Lesezugriff an dieser Adresse statt findet."));
    AddBreakpointTreeChild(item,bg->iWAdresse,bg->bWAdresse,tr("Wenn ein Schreibzugriff an dieser Adresse statt findet."));
    AddBreakpointTreeChild(item,bg->iRWert,bg->bRWert,tr("Wenn aus einer Adresse dieser Wert ausgelesen wird."));
    AddBreakpointTreeChild(item,bg->iWWert,bg->bWWert,tr("Wenn in einer Adresse dieser Wert geschrieben wird."));
    if(current_source == 0)
    {
        AddBreakpointTreeChild(item,bg->iRZ,bg->bRZ,tr("Wenn der Wert gleich der Aktuellen Rasterzeile ist."));
        AddBreakpointTreeChild(item,bg->iRZZyklus,bg->bRZZyklus,tr("Wenn der Wert gleich dem Aktuellen Zyklus in einer Rasterzeile ist."));
    }
    break_point_update_enable = true;
    c64->UpdateBreakGroup();
}

void DebuggerWindow::AddBreakpointTreeChild(QTreeWidgetItem *parent, uint16_t value, uint8_t checked, QString tooltip)
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
    item->setBackground(1,QColor(200,200,255));
    item->setForeground(1,QColor(200,0,0));
    parent->addChild(item);
}

void DebuggerWindow::on_BreakpointTree_itemChanged(QTreeWidgetItem *item, int column)
{
    int bg_index;
    int child_index;

    if(item->parent() == nullptr)
    {
        /// TopLevel Item (Breakpointname) ///

        bg_index = ui->BreakpointTree->indexOfTopLevelItem(item);
        BREAK_GROUP *bg;
        if(current_source > 0)
        {
            bg = c64->floppy1541[currnet_floppy_nr]->GetBreakGroup(bg_index);
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
        if(current_source > 0)
        {
            bg = c64->floppy1541[currnet_floppy_nr]->GetBreakGroup(bg_index);
        }
        else bg = c64->GetBreakGroup(bg_index);

        if(column == 0 || 1)
        {
            QString tmp_str = item->text(1);
            bool ok;
            uint16_t integer;
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
                        item->setBackground(1,QColor(200,200,255));
                        item->setForeground(1,QColor(200,0,0));
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                            item->setBackground(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackground(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                            item->setBackground(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackground(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                            item->setBackground(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackground(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                        item->setBackground(1,QColor(200,200,255));
                        item->setForeground(1,QColor(200,0,0));
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                        item->setBackground(1,QColor(200,200,255));
                        item->setForeground(1,QColor(200,0,0));
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                            item->setBackground(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackground(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                            item->setBackground(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackground(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                            item->setBackground(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackground(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
                            item->setBackground(1,QColor(200,200,255));
                            item->setForeground(1,QColor(200,0,0));
                        }
                        else
                        {
                            item->setCheckState(0,Qt::Unchecked);
                            item->setBackground(1,QColor(200,0,0));
                            item->setForeground(1,QColor(200,200,200));
                        }
                    }
                    else
                    {
                        item->setCheckState(0,Qt::Unchecked);
                        item->setBackground(1,QColor(200,0,0));
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
        if(break_point_update_enable)c64->UpdateBreakGroup();
    }
}

void DebuggerWindow::ClearAllBreakpointBackcolors()
{
    for(int i=0;i<ui->BreakpointTree->topLevelItemCount();i++)
    {
        ui->BreakpointTree->topLevelItem(i)->setBackground(0,QColor(255,255,255));
        ui->BreakpointTree->topLevelItem(i)->setBackground(1,QColor(255,255,255));
        ui->BreakpointTree->topLevelItem(i)->setText(1,"");
    }
}

void DebuggerWindow::BreakpointProc()
{
    new_breakpoint_found = true;
}

void DebuggerWindow::on_LoadBreakpoints_clicked()
{
    if(ui->BreakpointTree->topLevelItemCount() > 0)
        if(QMessageBox::No == QMessageBox::question(this, tr("Haltepunkte laden..."), tr("Es werden alle Haltepunkte gelöscht !\nMöchten Sie fortfahren?"),QMessageBox::Yes | QMessageBox::No))
            return;

    ui->BreakpointTree->clear();

    if(current_source > 0) c64->floppy1541[currnet_floppy_nr]->DeleteAllBreakGroups();
    else c64->DeleteAllBreakGroups();

	QString filename = QFileDialog::getOpenFileName(this, tr("Haltepunkte öffnen"), nullptr, tr("Emu64 Haltepunkt Datei ") + "(*.bpt)", nullptr, QFileDialog::DontUseNativeDialog);
    if(filename != "")
    {
       int ret;
       if(current_source > 0) ret = c64->floppy1541[currnet_floppy_nr]->LoadBreakGroups(filename.toLocal8Bit());
       else ret = c64->LoadBreakGroups(filename.toLocal8Bit());

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
           if(current_source > 0)
           {
               int count = c64->floppy1541[currnet_floppy_nr]->GetBreakGroupCount();
               for(int i=0; i<count; i++)
               {
                   BREAK_GROUP *bg = c64->floppy1541[currnet_floppy_nr]->GetBreakGroup(i);
                   AddBreakpointTreeRoot(bg->Name,bg);
               }
           }
           else
           {
               int count = c64->GetBreakGroupCount();
               for(int i=0; i<count; i++)
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

    QStringList filters;
    filters << tr("Emu64 Haltepunkt Datei (*.bpt)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this, tr("Haltepunkte speichern"), filters, &filename, &fileext))
    {
        return;
    }

    if(current_source > 0)
    {
        if(!c64->floppy1541[currnet_floppy_nr]->SaveBreakGroups(filename.toLocal8Bit()))
            QMessageBox::warning(this,tr("Fehler..."),tr("Die Haltepunkte konnten nicht gespeichert werden."));
    }
    else
    {
        if(!c64->SaveBreakGroups(filename.toLocal8Bit()))
          QMessageBox::warning(this,tr("Fehler..."),tr("Die Haltepunkte konnten nicht gespeichert werden."));
    }
}

void DebuggerWindow::on_DelAllBreakpoints_clicked()
{
    if(ui->BreakpointTree->topLevelItemCount() > 0)
        if(QMessageBox::Yes == QMessageBox::question(this, tr("Achtung..."), tr("Es werden alle Haltepunkte gelöscht !\nMöchten Sie fortfahren?"),QMessageBox::Yes | QMessageBox::No))
        {
            if(current_source > 0) c64->floppy1541[currnet_floppy_nr]->DeleteAllBreakGroups();
            else c64->DeleteAllBreakGroups();
            ui->BreakpointTree->clear();
        }
}

void DebuggerWindow::on_HistoryScroll_valueChanged(int value)
{
    FillHistoryList(static_cast<uint8_t>(value));
}

void DebuggerWindow::on_ExportDisAss_clicked()
{
    bool ok;
    QString in_str;
    uint16_t value;
    uint16_t start, end;

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

    QStringList filters;
    filters << tr("Disassembler Listing (*.txt)")
            << tr("C64 Programm Datei (*.prg)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this,tr("Export..."), filters, &filename, &fileext))
    {
        return;
    }

    fileext = fileext.toUpper();
    if(fileext == "TXT")
    {
        if(!c64->ExportASM(filename.toLocal8Bit(),start,end,current_source))
            QMessageBox::warning(this,tr("Fehler..."),tr("Fehler beim speichern der Disassembler Datei."));
    }

    if(fileext == "PRG")
    {
        if((current_source > 0) && (end > 0x07FF))
        {
            QMessageBox::warning(this,tr("Fehler..."),tr("Floppy RAM geht nur von $0000 - $07FF (2KB)."));
            return;
        }
        if(!c64->ExportPRG(filename.toLocal8Bit(),start,end,current_source))
            QMessageBox::warning(this,tr("Fehler..."),tr("Fehler beim speichern der Programm Datei."));
    }
}

void DebuggerWindow::onChangeFloppyStatus(void)
{
    RefreshGUI();
}

void DebuggerWindow::RefreshGUI(void)
{
    if(c64 == nullptr) return;

    c64_cpu_reg.reg_mask = REG_MASK_ALL;
    c64->GetC64CpuReg(&c64_cpu_reg,&c64_cpu_ireg);

    if(current_source > 0)
    {
        /// Floppy's ///
        floppy_cpu_reg[currnet_floppy_nr].reg_mask = REG_MASK_ALL;
        c64->floppy1541[currnet_floppy_nr]->GetCpuReg(&floppy_cpu_reg[currnet_floppy_nr],&floppy_cpu_ireg[currnet_floppy_nr]);

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

        if(c64->floppy1541[current_source-1]->GetEnableFloppy())
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
            FillDisassemblyList(floppy_cpu_ireg[currnet_floppy_nr].current_opcode_pc,false);
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
            ui->irq_led->setIcon(*icon_off);
            ui->reset_led->setIcon(*icon_off);

            ui->CycleGroup->setEnabled(false);
            ui->CycleCounter_Out->setText("");

            ui->VerlaufGroup->setEnabled(false);
            for(int i=0;i<HISTORY_ROW;i++) ui->HistoryList->item(i)->setText("");

            ui->DisassGroup->setEnabled(false);
            for(int i=0;i<DISASS_ROW;i++)
            {
                disass_pc[i]->setText("");
                disass_pc[i]->setBackground(table_back_color);
                disass_memory[i]->setText("");
                disass_memory[i]->setBackground(table_back_color);
                disass_mnemonic[i]->setText("");
                disass_mnemonic[i]->setBackground(table_back_color);
                disass_addressing[i]->setText("");
                disass_addressing[i]->setBackground(table_back_color);
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
        FillDisassemblyList(c64_cpu_ireg.current_opcode_pc,false);
    }

    ui->EingabeFeld->hide();
    UpdateRegister();
    FillHistoryList(static_cast<uint8_t>(ui->HistoryScroll->value()));
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

void DebuggerWindow::on_IEC_clicked()
{
    if(iec_window->isHidden())
    {
        iec_window->show();
        iec_window->UpdateSignals();
    }
    else iec_window->hide();
}

void DebuggerWindow::on_man_lines_clicked(bool checked)
{
    c64->SetCpuExtLines(checked);

    if(!checked)
    {
        if(c64_cpu_ireg.irq) ui->irq_led->setIcon(*icon_on);
        else ui->irq_led->setIcon(*icon_off);
        if(c64_cpu_ireg.nmi) ui->nmi_led->setIcon(*icon_on);
        else ui->nmi_led->setIcon(*icon_off);
        if(c64_cpu_ireg.rdy) ui->rdy_led->setIcon(*icon_on);
        else ui->rdy_led->setIcon(*icon_off);
    }
    else
    {
        ui->rdy_led->setChecked(true);
        ui->rdy_led->setIcon(*icon_on);
        c64->SetExtRDY(true);

        ui->irq_led->setChecked(false);
        ui->irq_led->setIcon(*icon_off);

        ui->nmi_led->setChecked(false);
        ui->nmi_led->setIcon(*icon_off);
    }
}

void DebuggerWindow::on_rdy_led_clicked(bool checked)
{
    if(ui->man_lines->checkState())
    {
        c64->SetExtRDY(checked);
        if(checked) ui->rdy_led->setIcon(*icon_on);
        else ui->rdy_led->setIcon(*icon_off);
    }
}

void DebuggerWindow::on_irq_led_clicked(bool checked)
{
    if(ui->man_lines->checkState())
    {
        if(checked) c64->cpu->TriggerInterrupt(EXT_IRQ);
        else c64->cpu->ClearInterrupt(EXT_IRQ);
        if(checked) ui->irq_led->setIcon(*icon_on);
        else ui->irq_led->setIcon(*icon_off);
    }
}

void DebuggerWindow::on_nmi_led_clicked(bool checked)
{
    if(ui->man_lines->checkState())
    {
        if(checked) c64->cpu->TriggerInterrupt(EXT_NMI);
        else c64->cpu->ClearInterrupt(EXT_NMI);
        if(checked) ui->nmi_led->setIcon(*icon_on);
        else ui->nmi_led->setIcon(*icon_off);
    }
}
