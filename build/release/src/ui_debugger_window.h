/********************************************************************************
** Form generated from reading UI file 'debugger_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGGER_WINDOW_H
#define UI_DEBUGGER_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QListWidget>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QTableWidget>
#include <QtGui/QTreeWidget>
#include "label_widget_mod.h"
#include "widget_sr_edit.h"

QT_BEGIN_NAMESPACE

class Ui_DebuggerWindow
{
public:
    QGroupBox *RegisterGroup;
    QLabel *label_pc;
    QLabel *label_sp;
    QLabel *label_ac;
    QLabel *label_xr;
    QLabel *label_yr;
    QLabel *label_sr;
    LabelWidgetMod *pc_out;
    LabelWidgetMod *sp_out;
    LabelWidgetMod *ac_out;
    LabelWidgetMod *xr_out;
    LabelWidgetMod *yr_out;
    QLabel *label_irq;
    QLabel *label_0314;
    QLabel *label_nmi;
    QLabel *label_0318;
    QLabel *irq_out;
    QLabel *_0314_out;
    QLabel *nmi_out;
    QLabel *_0318_out;
    WidgetSREdit *sr_widget;
    LabelWidgetMod *sr_out;
    QLineEdit *EingabeFeld;
    QComboBox *ChangeSource;
    QGroupBox *IRegisterGroup;
    QLabel *MCodeHelp;
    QLabel *label_opcode_pc;
    QLabel *label_adresse;
    QLabel *label_branch_adr;
    QLabel *label_micro_code;
    QLabel *opcode_pc_out;
    QLabel *adresse_out;
    QLabel *branch_adresse_out;
    QLabel *micro_code_out;
    QLabel *label_opcode;
    QLabel *label_pointer;
    QLabel *label_tmp_byte;
    QLabel *opcode_out;
    QLabel *pointer_out;
    QLabel *tmp_byte_out;
    QLabel *label_mnemonic;
    QLabel *label_cpu_wait;
    QPushButton *wait_led;
    QLabel *mnemonic_out;
    QGroupBox *groupBox_3;
    QPushButton *OneZyklus;
    QPushButton *OneOpcode;
    QPushButton *AnimationStart;
    QPushButton *AnimationStop;
    QScrollBar *AnimationSpeed;
    QLabel *label_anispeed;
    QLabel *anispeed_out;
    QGroupBox *CycleGroup;
    QLabel *CycleCounter_Out;
    QPushButton *CycleCounterReset;
    QPushButton *VIC;
    QPushButton *SID;
    QPushButton *CIA1;
    QPushButton *CIA2;
    QGroupBox *LeitungenGroup;
    QLabel *label_irq_2;
    QLabel *label_nmi_2;
    QLabel *label_ba;
    QPushButton *irq_led;
    QPushButton *nmi_led;
    QPushButton *rdy_led;
    QLabel *label_reset;
    QLabel *label_exrom;
    QLabel *label_game;
    QPushButton *reset_led;
    QPushButton *exrom_led;
    QPushButton *game_led;
    QPushButton *IEC;
    QCheckBox *man_lines;
    QGroupBox *DisassGroup;
    QScrollBar *DisAssScroll;
    QTableWidget *DisAssTable;
    QLineEdit *AssAdresseIn;
    QLineEdit *AssMnemonicIn;
    QLineEdit *AssAdressierungIn;
    QLabel *label;
    QLabel *label_floppy_off;
    QGroupBox *BreakGroup;
    QPushButton *AddBreakpoint;
    QPushButton *DelBreakpoint;
    QPushButton *LoadBreakpoints;
    QPushButton *SaveBreakpoints;
    QTreeWidget *BreakpointTree;
    QPushButton *DelAllBreakpoints;
    QGroupBox *VerlaufGroup;
    QScrollBar *HistoryScroll;
    QListWidget *HistoryList;
    QGroupBox *ExportGroup;
    QLabel *label_export_von;
    QLineEdit *ExportStartIn;
    QLabel *label_export_bis;
    QLineEdit *ExportEndIn;
    QPushButton *ExportDisAss;
    QPushButton *MemEdit;
    QPushButton *VIA1;
    QPushButton *VIA2;

    void setupUi(QDialog *DebuggerWindow)
    {
        if (DebuggerWindow->objectName().isEmpty())
            DebuggerWindow->setObjectName(QString::fromUtf8("DebuggerWindow"));
        DebuggerWindow->resize(520, 675);
        DebuggerWindow->setMinimumSize(QSize(520, 675));
        DebuggerWindow->setMaximumSize(QSize(520, 675));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(9);
        DebuggerWindow->setFont(font);
        DebuggerWindow->setContextMenuPolicy(Qt::CustomContextMenu);
        RegisterGroup = new QGroupBox(DebuggerWindow);
        RegisterGroup->setObjectName(QString::fromUtf8("RegisterGroup"));
        RegisterGroup->setGeometry(QRect(7, 35, 183, 127));
        RegisterGroup->setAutoFillBackground(true);
        RegisterGroup->setTitle(QString::fromUtf8(""));
        RegisterGroup->setFlat(false);
        RegisterGroup->setCheckable(false);
        label_pc = new QLabel(RegisterGroup);
        label_pc->setObjectName(QString::fromUtf8("label_pc"));
        label_pc->setGeometry(QRect(7, 21, 22, 16));
        label_pc->setFont(font);
        label_pc->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_pc->setText(QString::fromUtf8("PC:"));
        label_sp = new QLabel(RegisterGroup);
        label_sp->setObjectName(QString::fromUtf8("label_sp"));
        label_sp->setGeometry(QRect(7, 35, 22, 16));
        label_sp->setFont(font);
        label_sp->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_sp->setText(QString::fromUtf8("SP:"));
        label_ac = new QLabel(RegisterGroup);
        label_ac->setObjectName(QString::fromUtf8("label_ac"));
        label_ac->setGeometry(QRect(7, 49, 22, 16));
        label_ac->setFont(font);
        label_ac->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_ac->setText(QString::fromUtf8("AC:"));
        label_xr = new QLabel(RegisterGroup);
        label_xr->setObjectName(QString::fromUtf8("label_xr"));
        label_xr->setGeometry(QRect(7, 63, 22, 16));
        label_xr->setFont(font);
        label_xr->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_xr->setText(QString::fromUtf8("XR:"));
        label_yr = new QLabel(RegisterGroup);
        label_yr->setObjectName(QString::fromUtf8("label_yr"));
        label_yr->setGeometry(QRect(7, 77, 22, 16));
        label_yr->setFont(font);
        label_yr->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_yr->setText(QString::fromUtf8("YR:"));
        label_sr = new QLabel(RegisterGroup);
        label_sr->setObjectName(QString::fromUtf8("label_sr"));
        label_sr->setGeometry(QRect(7, 91, 22, 16));
        label_sr->setFont(font);
        label_sr->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_sr->setText(QString::fromUtf8("SR:"));
        pc_out = new LabelWidgetMod(RegisterGroup);
        pc_out->setObjectName(QString::fromUtf8("pc_out"));
        pc_out->setGeometry(QRect(35, 21, 43, 16));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setPointSize(9);
        font1.setBold(true);
        font1.setWeight(75);
        pc_out->setFont(font1);
        pc_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);\n"
"background-color: rgb(220,220, 255);"));
        pc_out->setText(QString::fromUtf8("$FFFF"));
        sp_out = new LabelWidgetMod(RegisterGroup);
        sp_out->setObjectName(QString::fromUtf8("sp_out"));
        sp_out->setGeometry(QRect(35, 35, 43, 16));
        sp_out->setFont(font1);
        sp_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);\n"
"background-color: rgb(220,220, 255);"));
        sp_out->setText(QString::fromUtf8("$0100"));
        ac_out = new LabelWidgetMod(RegisterGroup);
        ac_out->setObjectName(QString::fromUtf8("ac_out"));
        ac_out->setGeometry(QRect(35, 49, 43, 16));
        ac_out->setFont(font1);
        ac_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);\n"
"background-color: rgb(220,220, 255);"));
        ac_out->setText(QString::fromUtf8("$00"));
        xr_out = new LabelWidgetMod(RegisterGroup);
        xr_out->setObjectName(QString::fromUtf8("xr_out"));
        xr_out->setGeometry(QRect(35, 63, 43, 16));
        xr_out->setFont(font1);
        xr_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);\n"
"background-color: rgb(220,220, 255);"));
        xr_out->setText(QString::fromUtf8("$00"));
        yr_out = new LabelWidgetMod(RegisterGroup);
        yr_out->setObjectName(QString::fromUtf8("yr_out"));
        yr_out->setGeometry(QRect(35, 77, 43, 16));
        yr_out->setFont(font1);
        yr_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);\n"
"background-color: rgb(220,220, 255);"));
        yr_out->setText(QString::fromUtf8("$00"));
        label_irq = new QLabel(RegisterGroup);
        label_irq->setObjectName(QString::fromUtf8("label_irq"));
        label_irq->setGeometry(QRect(84, 21, 36, 16));
        label_irq->setFont(font);
        label_irq->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_irq->setText(QString::fromUtf8("IRQ:"));
        label_0314 = new QLabel(RegisterGroup);
        label_0314->setObjectName(QString::fromUtf8("label_0314"));
        label_0314->setGeometry(QRect(84, 35, 36, 16));
        label_0314->setFont(font);
        label_0314->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_0314->setText(QString::fromUtf8("0314:"));
        label_nmi = new QLabel(RegisterGroup);
        label_nmi->setObjectName(QString::fromUtf8("label_nmi"));
        label_nmi->setGeometry(QRect(84, 49, 36, 16));
        label_nmi->setFont(font);
        label_nmi->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_nmi->setText(QString::fromUtf8("NMI:"));
        label_0318 = new QLabel(RegisterGroup);
        label_0318->setObjectName(QString::fromUtf8("label_0318"));
        label_0318->setGeometry(QRect(84, 63, 36, 16));
        label_0318->setFont(font);
        label_0318->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_0318->setText(QString::fromUtf8("0318:"));
        irq_out = new QLabel(RegisterGroup);
        irq_out->setObjectName(QString::fromUtf8("irq_out"));
        irq_out->setGeometry(QRect(126, 21, 43, 16));
        irq_out->setFont(font1);
        irq_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        irq_out->setText(QString::fromUtf8("$FFFF"));
        _0314_out = new QLabel(RegisterGroup);
        _0314_out->setObjectName(QString::fromUtf8("_0314_out"));
        _0314_out->setGeometry(QRect(126, 35, 43, 16));
        _0314_out->setFont(font1);
        _0314_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        _0314_out->setText(QString::fromUtf8("$FFFF"));
        nmi_out = new QLabel(RegisterGroup);
        nmi_out->setObjectName(QString::fromUtf8("nmi_out"));
        nmi_out->setGeometry(QRect(126, 49, 43, 16));
        nmi_out->setFont(font1);
        nmi_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        nmi_out->setText(QString::fromUtf8("$FFFF"));
        _0318_out = new QLabel(RegisterGroup);
        _0318_out->setObjectName(QString::fromUtf8("_0318_out"));
        _0318_out->setGeometry(QRect(126, 63, 43, 16));
        _0318_out->setFont(font1);
        _0318_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        _0318_out->setText(QString::fromUtf8("$FFFF"));
        sr_widget = new WidgetSREdit(RegisterGroup);
        sr_widget->setObjectName(QString::fromUtf8("sr_widget"));
        sr_widget->setGeometry(QRect(35, 93, 120, 29));
        sr_out = new LabelWidgetMod(RegisterGroup);
        sr_out->setObjectName(QString::fromUtf8("sr_out"));
        sr_out->setGeometry(QRect(147, 105, 29, 16));
        sr_out->setFont(font1);
        sr_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);\n"
"background-color: rgb(220,220, 255);"));
        sr_out->setText(QString::fromUtf8("$00"));
        EingabeFeld = new QLineEdit(RegisterGroup);
        EingabeFeld->setObjectName(QString::fromUtf8("EingabeFeld"));
        EingabeFeld->setEnabled(true);
        EingabeFeld->setGeometry(QRect(7, 105, 16, 14));
        QFont font2;
        font2.setPointSize(8);
        font2.setBold(true);
        font2.setWeight(75);
        EingabeFeld->setFont(font2);
        EingabeFeld->setStyleSheet(QString::fromUtf8("border: 1px solid rgb(0,0,0);\n"
"background-color: rgb(255, 255, 255);"));
        EingabeFeld->setReadOnly(false);
        ChangeSource = new QComboBox(DebuggerWindow);
        ChangeSource->setObjectName(QString::fromUtf8("ChangeSource"));
        ChangeSource->setGeometry(QRect(7, 7, 183, 22));
        IRegisterGroup = new QGroupBox(DebuggerWindow);
        IRegisterGroup->setObjectName(QString::fromUtf8("IRegisterGroup"));
        IRegisterGroup->setGeometry(QRect(196, 35, 316, 127));
        IRegisterGroup->setAutoFillBackground(true);
        IRegisterGroup->setTitle(QString::fromUtf8(""));
        MCodeHelp = new QLabel(IRegisterGroup);
        MCodeHelp->setObjectName(QString::fromUtf8("MCodeHelp"));
        MCodeHelp->setGeometry(QRect(7, 84, 302, 37));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        MCodeHelp->setFont(font3);
        MCodeHelp->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 64, 0);\n"
"color: rgb(85, 255, 0);\n"
"border: 1px solid white;\n"
"border-top-color: rgb(150, 150,150);\n"
"border-left-color: rgb(150, 150,150);\n"
"border-bottom-color: rgb(255, 255, 255);\n"
"border-right-color: rgb(255, 255, 255);"));
        MCodeHelp->setText(QString::fromUtf8("<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
"<html><head><meta name=\"qrichtext\" content=\"1\" /><style type=\"text/css\">\n"
"p, li { white-space: pre-wrap; }\n"
"</style></head><body style=\" font-family:'MS Shell Dlg 2'; font-size:8.25pt; font-weight:400; font-style:normal;\">\n"
"<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\"><span style=\" font-size:8pt;\">TMPByte von Adresse lesen // AC - TMPByte (AC wird nicht ver\303\244ndert) // if(idxReg&lt;Adresse Lo) MCT++</span></p></body></html>"));
        MCodeHelp->setTextFormat(Qt::RichText);
        MCodeHelp->setScaledContents(false);
        MCodeHelp->setAlignment(Qt::AlignLeading|Qt::AlignLeft|Qt::AlignTop);
        MCodeHelp->setWordWrap(true);
        MCodeHelp->setOpenExternalLinks(false);
        label_opcode_pc = new QLabel(IRegisterGroup);
        label_opcode_pc->setObjectName(QString::fromUtf8("label_opcode_pc"));
        label_opcode_pc->setGeometry(QRect(7, 21, 71, 16));
        label_opcode_pc->setFont(font);
        label_opcode_pc->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_opcode_pc->setText(QString::fromUtf8("Opcode PC:"));
        label_adresse = new QLabel(IRegisterGroup);
        label_adresse->setObjectName(QString::fromUtf8("label_adresse"));
        label_adresse->setGeometry(QRect(7, 35, 71, 16));
        label_adresse->setFont(font);
        label_adresse->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_adresse->setText(QString::fromUtf8("Adresse:"));
        label_branch_adr = new QLabel(IRegisterGroup);
        label_branch_adr->setObjectName(QString::fromUtf8("label_branch_adr"));
        label_branch_adr->setGeometry(QRect(7, 49, 71, 16));
        label_branch_adr->setFont(font);
        label_branch_adr->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_branch_adr->setText(QString::fromUtf8("Branch Adr.:"));
        label_micro_code = new QLabel(IRegisterGroup);
        label_micro_code->setObjectName(QString::fromUtf8("label_micro_code"));
        label_micro_code->setGeometry(QRect(7, 70, 71, 16));
        label_micro_code->setFont(font);
        label_micro_code->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_micro_code->setText(QString::fromUtf8("Micro Code:"));
        opcode_pc_out = new QLabel(IRegisterGroup);
        opcode_pc_out->setObjectName(QString::fromUtf8("opcode_pc_out"));
        opcode_pc_out->setGeometry(QRect(77, 21, 43, 16));
        opcode_pc_out->setFont(font1);
        opcode_pc_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        opcode_pc_out->setText(QString::fromUtf8("$FFFF"));
        adresse_out = new QLabel(IRegisterGroup);
        adresse_out->setObjectName(QString::fromUtf8("adresse_out"));
        adresse_out->setGeometry(QRect(77, 35, 43, 16));
        adresse_out->setFont(font1);
        adresse_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        adresse_out->setText(QString::fromUtf8("$FFFF"));
        branch_adresse_out = new QLabel(IRegisterGroup);
        branch_adresse_out->setObjectName(QString::fromUtf8("branch_adresse_out"));
        branch_adresse_out->setGeometry(QRect(77, 49, 43, 16));
        branch_adresse_out->setFont(font1);
        branch_adresse_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        branch_adresse_out->setText(QString::fromUtf8("$FFFF"));
        micro_code_out = new QLabel(IRegisterGroup);
        micro_code_out->setObjectName(QString::fromUtf8("micro_code_out"));
        micro_code_out->setGeometry(QRect(77, 70, 99, 16));
        micro_code_out->setFont(font1);
        micro_code_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        micro_code_out->setText(QString::fromUtf8("0"));
        label_opcode = new QLabel(IRegisterGroup);
        label_opcode->setObjectName(QString::fromUtf8("label_opcode"));
        label_opcode->setGeometry(QRect(119, 21, 57, 16));
        label_opcode->setFont(font);
        label_opcode->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_opcode->setText(QString::fromUtf8("Opcode:"));
        label_pointer = new QLabel(IRegisterGroup);
        label_pointer->setObjectName(QString::fromUtf8("label_pointer"));
        label_pointer->setGeometry(QRect(119, 35, 57, 16));
        label_pointer->setFont(font);
        label_pointer->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_pointer->setText(QString::fromUtf8("Pointer:"));
        label_tmp_byte = new QLabel(IRegisterGroup);
        label_tmp_byte->setObjectName(QString::fromUtf8("label_tmp_byte"));
        label_tmp_byte->setGeometry(QRect(119, 49, 57, 16));
        label_tmp_byte->setFont(font);
        label_tmp_byte->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_tmp_byte->setText(QString::fromUtf8("Tmp Byte:"));
        opcode_out = new QLabel(IRegisterGroup);
        opcode_out->setObjectName(QString::fromUtf8("opcode_out"));
        opcode_out->setGeometry(QRect(175, 21, 29, 16));
        opcode_out->setFont(font1);
        opcode_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        opcode_out->setText(QString::fromUtf8("$FFF"));
        pointer_out = new QLabel(IRegisterGroup);
        pointer_out->setObjectName(QString::fromUtf8("pointer_out"));
        pointer_out->setGeometry(QRect(175, 35, 29, 16));
        pointer_out->setFont(font1);
        pointer_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        pointer_out->setText(QString::fromUtf8("$FF"));
        tmp_byte_out = new QLabel(IRegisterGroup);
        tmp_byte_out->setObjectName(QString::fromUtf8("tmp_byte_out"));
        tmp_byte_out->setGeometry(QRect(175, 49, 29, 16));
        tmp_byte_out->setFont(font1);
        tmp_byte_out->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        tmp_byte_out->setText(QString::fromUtf8("$FF"));
        label_mnemonic = new QLabel(IRegisterGroup);
        label_mnemonic->setObjectName(QString::fromUtf8("label_mnemonic"));
        label_mnemonic->setGeometry(QRect(210, 21, 64, 16));
        label_mnemonic->setFont(font);
        label_mnemonic->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_mnemonic->setText(QString::fromUtf8("Mnemonic:"));
        label_cpu_wait = new QLabel(IRegisterGroup);
        label_cpu_wait->setObjectName(QString::fromUtf8("label_cpu_wait"));
        label_cpu_wait->setGeometry(QRect(210, 49, 57, 16));
        label_cpu_wait->setFont(font);
        label_cpu_wait->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_cpu_wait->setText(QString::fromUtf8("CPU Wait:"));
        wait_led = new QPushButton(IRegisterGroup);
        wait_led->setObjectName(QString::fromUtf8("wait_led"));
        wait_led->setGeometry(QRect(280, 50, 12, 12));
        wait_led->setText(QString::fromUtf8(""));
        mnemonic_out = new QLabel(IRegisterGroup);
        mnemonic_out->setObjectName(QString::fromUtf8("mnemonic_out"));
        mnemonic_out->setGeometry(QRect(280, 21, 29, 16));
        mnemonic_out->setFont(font1);
        mnemonic_out->setStyleSheet(QString::fromUtf8("color: rgb(200, 0, 0);"));
        mnemonic_out->setText(QString::fromUtf8("BEQ"));
        groupBox_3 = new QGroupBox(DebuggerWindow);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 200, 181, 96));
        OneZyklus = new QPushButton(groupBox_3);
        OneZyklus->setObjectName(QString::fromUtf8("OneZyklus"));
        OneZyklus->setGeometry(QRect(7, 22, 29, 22));
        OneZyklus->setText(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/grafik/one_zyklus.png"), QSize(), QIcon::Normal, QIcon::Off);
        OneZyklus->setIcon(icon);
        OneOpcode = new QPushButton(groupBox_3);
        OneOpcode->setObjectName(QString::fromUtf8("OneOpcode"));
        OneOpcode->setGeometry(QRect(42, 22, 29, 22));
        OneOpcode->setText(QString::fromUtf8(""));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/grafik/one_opcode.png"), QSize(), QIcon::Normal, QIcon::Off);
        OneOpcode->setIcon(icon1);
        AnimationStart = new QPushButton(groupBox_3);
        AnimationStart->setObjectName(QString::fromUtf8("AnimationStart"));
        AnimationStart->setGeometry(QRect(77, 22, 29, 22));
        AnimationStart->setText(QString::fromUtf8(""));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/grafik/animation.png"), QSize(), QIcon::Normal, QIcon::Off);
        AnimationStart->setIcon(icon2);
        AnimationStop = new QPushButton(groupBox_3);
        AnimationStop->setObjectName(QString::fromUtf8("AnimationStop"));
        AnimationStop->setGeometry(QRect(112, 22, 29, 22));
        AnimationStop->setText(QString::fromUtf8(""));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/grafik/stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        AnimationStop->setIcon(icon3);
        AnimationSpeed = new QScrollBar(groupBox_3);
        AnimationSpeed->setObjectName(QString::fromUtf8("AnimationSpeed"));
        AnimationSpeed->setGeometry(QRect(10, 50, 161, 16));
        AnimationSpeed->setMinimum(1);
        AnimationSpeed->setMaximum(985248);
        AnimationSpeed->setPageStep(1000);
        AnimationSpeed->setOrientation(Qt::Horizontal);
        label_anispeed = new QLabel(groupBox_3);
        label_anispeed->setObjectName(QString::fromUtf8("label_anispeed"));
        label_anispeed->setGeometry(QRect(5, 70, 41, 16));
        anispeed_out = new QLabel(groupBox_3);
        anispeed_out->setObjectName(QString::fromUtf8("anispeed_out"));
        anispeed_out->setGeometry(QRect(50, 70, 126, 16));
        anispeed_out->setText(QString::fromUtf8("TextLabel"));
        CycleGroup = new QGroupBox(DebuggerWindow);
        CycleGroup->setObjectName(QString::fromUtf8("CycleGroup"));
        CycleGroup->setGeometry(QRect(195, 247, 225, 50));
        CycleCounter_Out = new QLabel(CycleGroup);
        CycleCounter_Out->setObjectName(QString::fromUtf8("CycleCounter_Out"));
        CycleCounter_Out->setGeometry(QRect(7, 21, 106, 22));
        QFont font4;
        font4.setFamily(QString::fromUtf8("Arial"));
        font4.setPointSize(12);
        font4.setBold(true);
        font4.setWeight(75);
        font4.setKerning(true);
        CycleCounter_Out->setFont(font4);
        CycleCounter_Out->setStyleSheet(QString::fromUtf8("background-color: rgb(0, 64, 0);\n"
"color: rgb(85, 255, 0);\n"
"border: 1px solid white;\n"
"border-top-color: rgb(150, 150,150);\n"
"border-left-color: rgb(150, 150,150);\n"
"border-bottom-color: rgb(255, 255, 255);\n"
"border-right-color: rgb(255, 255, 255);"));
        CycleCounter_Out->setText(QString::fromUtf8("0000000000"));
        CycleCounter_Out->setAlignment(Qt::AlignCenter);
        CycleCounterReset = new QPushButton(CycleGroup);
        CycleCounterReset->setObjectName(QString::fromUtf8("CycleCounterReset"));
        CycleCounterReset->setGeometry(QRect(120, 20, 99, 22));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Ubuntu"));
        font5.setPointSize(8);
        CycleCounterReset->setFont(font5);
        VIC = new QPushButton(DebuggerWindow);
        VIC->setObjectName(QString::fromUtf8("VIC"));
        VIC->setEnabled(true);
        VIC->setGeometry(QRect(10, 170, 41, 23));
        SID = new QPushButton(DebuggerWindow);
        SID->setObjectName(QString::fromUtf8("SID"));
        SID->setGeometry(QRect(55, 170, 41, 23));
        CIA1 = new QPushButton(DebuggerWindow);
        CIA1->setObjectName(QString::fromUtf8("CIA1"));
        CIA1->setGeometry(QRect(100, 170, 41, 23));
        CIA2 = new QPushButton(DebuggerWindow);
        CIA2->setObjectName(QString::fromUtf8("CIA2"));
        CIA2->setGeometry(QRect(145, 170, 41, 23));
        LeitungenGroup = new QGroupBox(DebuggerWindow);
        LeitungenGroup->setObjectName(QString::fromUtf8("LeitungenGroup"));
        LeitungenGroup->setGeometry(QRect(195, 170, 225, 71));
        label_irq_2 = new QLabel(LeitungenGroup);
        label_irq_2->setObjectName(QString::fromUtf8("label_irq_2"));
        label_irq_2->setGeometry(QRect(37, 21, 29, 16));
        label_irq_2->setFont(font);
        label_irq_2->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_irq_2->setText(QString::fromUtf8("IRQ:"));
        label_nmi_2 = new QLabel(LeitungenGroup);
        label_nmi_2->setObjectName(QString::fromUtf8("label_nmi_2"));
        label_nmi_2->setGeometry(QRect(37, 35, 29, 16));
        label_nmi_2->setFont(font);
        label_nmi_2->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_nmi_2->setText(QString::fromUtf8("NMI:"));
        label_ba = new QLabel(LeitungenGroup);
        label_ba->setObjectName(QString::fromUtf8("label_ba"));
        label_ba->setGeometry(QRect(37, 49, 29, 16));
        label_ba->setFont(font);
        label_ba->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_ba->setText(QString::fromUtf8("RDY:"));
        irq_led = new QPushButton(LeitungenGroup);
        irq_led->setObjectName(QString::fromUtf8("irq_led"));
        irq_led->setGeometry(QRect(65, 22, 12, 12));
        irq_led->setText(QString::fromUtf8(""));
        irq_led->setCheckable(true);
        nmi_led = new QPushButton(LeitungenGroup);
        nmi_led->setObjectName(QString::fromUtf8("nmi_led"));
        nmi_led->setGeometry(QRect(65, 36, 12, 12));
        nmi_led->setText(QString::fromUtf8(""));
        nmi_led->setCheckable(true);
        rdy_led = new QPushButton(LeitungenGroup);
        rdy_led->setObjectName(QString::fromUtf8("rdy_led"));
        rdy_led->setGeometry(QRect(65, 50, 12, 12));
        rdy_led->setText(QString::fromUtf8(""));
        rdy_led->setCheckable(true);
        label_reset = new QLabel(LeitungenGroup);
        label_reset->setObjectName(QString::fromUtf8("label_reset"));
        label_reset->setGeometry(QRect(90, 20, 43, 16));
        label_reset->setFont(font);
        label_reset->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_reset->setText(QString::fromUtf8("RESET:"));
        label_exrom = new QLabel(LeitungenGroup);
        label_exrom->setObjectName(QString::fromUtf8("label_exrom"));
        label_exrom->setGeometry(QRect(154, 21, 50, 16));
        label_exrom->setFont(font);
        label_exrom->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_exrom->setText(QString::fromUtf8("EXROM:"));
        label_game = new QLabel(LeitungenGroup);
        label_game->setObjectName(QString::fromUtf8("label_game"));
        label_game->setGeometry(QRect(154, 35, 50, 16));
        label_game->setFont(font);
        label_game->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_game->setText(QString::fromUtf8("GAME:"));
        reset_led = new QPushButton(LeitungenGroup);
        reset_led->setObjectName(QString::fromUtf8("reset_led"));
        reset_led->setGeometry(QRect(135, 22, 12, 12));
        reset_led->setText(QString::fromUtf8(""));
        exrom_led = new QPushButton(LeitungenGroup);
        exrom_led->setObjectName(QString::fromUtf8("exrom_led"));
        exrom_led->setGeometry(QRect(203, 22, 12, 12));
        exrom_led->setText(QString::fromUtf8(""));
        game_led = new QPushButton(LeitungenGroup);
        game_led->setObjectName(QString::fromUtf8("game_led"));
        game_led->setGeometry(QRect(203, 36, 12, 12));
        game_led->setText(QString::fromUtf8(""));
        IEC = new QPushButton(LeitungenGroup);
        IEC->setObjectName(QString::fromUtf8("IEC"));
        IEC->setGeometry(QRect(90, 45, 56, 23));
        man_lines = new QCheckBox(LeitungenGroup);
        man_lines->setObjectName(QString::fromUtf8("man_lines"));
        man_lines->setEnabled(true);
        man_lines->setGeometry(QRect(10, 20, 21, 21));
        DisassGroup = new QGroupBox(DebuggerWindow);
        DisassGroup->setObjectName(QString::fromUtf8("DisassGroup"));
        DisassGroup->setGeometry(QRect(10, 300, 232, 311));
        DisAssScroll = new QScrollBar(DisassGroup);
        DisAssScroll->setObjectName(QString::fromUtf8("DisAssScroll"));
        DisAssScroll->setGeometry(QRect(212, 21, 16, 261));
        DisAssScroll->setMaximum(65790);
        DisAssScroll->setPageStep(256);
        DisAssScroll->setOrientation(Qt::Vertical);
        DisAssTable = new QTableWidget(DisassGroup);
        DisAssTable->setObjectName(QString::fromUtf8("DisAssTable"));
        DisAssTable->setGeometry(QRect(7, 21, 204, 262));
        DisAssTable->setContextMenuPolicy(Qt::CustomContextMenu);
        DisAssTable->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        DisAssTable->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        DisAssTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
        DisAssTable->setSelectionMode(QAbstractItemView::NoSelection);
        DisAssTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        DisAssTable->setGridStyle(Qt::SolidLine);
        DisAssTable->horizontalHeader()->setVisible(false);
        DisAssTable->horizontalHeader()->setMinimumSectionSize(23);
        DisAssTable->verticalHeader()->setVisible(false);
        DisAssTable->verticalHeader()->setDefaultSectionSize(13);
        DisAssTable->verticalHeader()->setHighlightSections(false);
        DisAssTable->verticalHeader()->setMinimumSectionSize(13);
        AssAdresseIn = new QLineEdit(DisassGroup);
        AssAdresseIn->setObjectName(QString::fromUtf8("AssAdresseIn"));
        AssAdresseIn->setGeometry(QRect(52, 285, 51, 20));
        AssAdresseIn->setFocusPolicy(Qt::ClickFocus);
        AssAdresseIn->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 200);"));
        AssAdresseIn->setInputMethodHints(Qt::ImhNone);
        AssAdresseIn->setText(QString::fromUtf8(""));
        AssAdresseIn->setMaxLength(10);
        AssAdresseIn->setEchoMode(QLineEdit::Normal);
        AssMnemonicIn = new QLineEdit(DisassGroup);
        AssMnemonicIn->setObjectName(QString::fromUtf8("AssMnemonicIn"));
        AssMnemonicIn->setGeometry(QRect(105, 285, 36, 20));
        AssMnemonicIn->setFocusPolicy(Qt::ClickFocus);
        AssMnemonicIn->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 200);"));
        AssMnemonicIn->setInputMethodHints(Qt::ImhNone);
        AssMnemonicIn->setText(QString::fromUtf8(""));
        AssMnemonicIn->setMaxLength(3);
        AssAdressierungIn = new QLineEdit(DisassGroup);
        AssAdressierungIn->setObjectName(QString::fromUtf8("AssAdressierungIn"));
        AssAdressierungIn->setGeometry(QRect(145, 285, 66, 20));
        AssAdressierungIn->setFocusPolicy(Qt::ClickFocus);
        AssAdressierungIn->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 200);"));
        AssAdressierungIn->setInputMethodHints(Qt::ImhNone);
        AssAdressierungIn->setText(QString::fromUtf8(""));
        AssAdressierungIn->setMaxLength(10);
        label = new QLabel(DisassGroup);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 287, 41, 16));
        QFont font6;
        font6.setPointSize(12);
        font6.setBold(true);
        font6.setWeight(75);
        label->setFont(font6);
        label->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 150);"));
        label_floppy_off = new QLabel(DebuggerWindow);
        label_floppy_off->setObjectName(QString::fromUtf8("label_floppy_off"));
        label_floppy_off->setEnabled(true);
        label_floppy_off->setGeometry(QRect(200, 5, 311, 26));
        QFont font7;
        font7.setPointSize(11);
        font7.setBold(true);
        font7.setWeight(75);
        label_floppy_off->setFont(font7);
        label_floppy_off->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        BreakGroup = new QGroupBox(DebuggerWindow);
        BreakGroup->setObjectName(QString::fromUtf8("BreakGroup"));
        BreakGroup->setGeometry(QRect(245, 300, 261, 311));
        AddBreakpoint = new QPushButton(BreakGroup);
        AddBreakpoint->setObjectName(QString::fromUtf8("AddBreakpoint"));
        AddBreakpoint->setGeometry(QRect(5, 277, 30, 30));
        AddBreakpoint->setText(QString::fromUtf8(""));
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/grafik/plus_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        AddBreakpoint->setIcon(icon4);
        AddBreakpoint->setIconSize(QSize(22, 22));
        DelBreakpoint = new QPushButton(BreakGroup);
        DelBreakpoint->setObjectName(QString::fromUtf8("DelBreakpoint"));
        DelBreakpoint->setGeometry(QRect(35, 277, 30, 30));
        DelBreakpoint->setText(QString::fromUtf8(""));
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/grafik/minus_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        DelBreakpoint->setIcon(icon5);
        DelBreakpoint->setIconSize(QSize(22, 22));
        LoadBreakpoints = new QPushButton(BreakGroup);
        LoadBreakpoints->setObjectName(QString::fromUtf8("LoadBreakpoints"));
        LoadBreakpoints->setGeometry(QRect(226, 277, 30, 30));
        LoadBreakpoints->setText(QString::fromUtf8(""));
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/grafik/open_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        LoadBreakpoints->setIcon(icon6);
        LoadBreakpoints->setIconSize(QSize(22, 22));
        SaveBreakpoints = new QPushButton(BreakGroup);
        SaveBreakpoints->setObjectName(QString::fromUtf8("SaveBreakpoints"));
        SaveBreakpoints->setGeometry(QRect(196, 277, 30, 30));
        SaveBreakpoints->setText(QString::fromUtf8(""));
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/grafik/save_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        SaveBreakpoints->setIcon(icon7);
        SaveBreakpoints->setIconSize(QSize(22, 22));
        BreakpointTree = new QTreeWidget(BreakGroup);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setText(0, QString::fromUtf8("1"));
        BreakpointTree->setHeaderItem(__qtreewidgetitem);
        BreakpointTree->setObjectName(QString::fromUtf8("BreakpointTree"));
        BreakpointTree->setGeometry(QRect(5, 25, 251, 251));
        QFont font8;
        font8.setFamily(QString::fromUtf8("Arial"));
        font8.setPointSize(8);
        BreakpointTree->setFont(font8);
        BreakpointTree->setEditTriggers(QAbstractItemView::DoubleClicked);
        BreakpointTree->setTabKeyNavigation(false);
        BreakpointTree->setAnimated(true);
        BreakpointTree->header()->setVisible(false);
        DelAllBreakpoints = new QPushButton(BreakGroup);
        DelAllBreakpoints->setObjectName(QString::fromUtf8("DelAllBreakpoints"));
        DelAllBreakpoints->setGeometry(QRect(65, 277, 30, 30));
        DelAllBreakpoints->setText(QString::fromUtf8(""));
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/grafik/remove_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        DelAllBreakpoints->setIcon(icon8);
        DelAllBreakpoints->setIconSize(QSize(22, 22));
        VerlaufGroup = new QGroupBox(DebuggerWindow);
        VerlaufGroup->setObjectName(QString::fromUtf8("VerlaufGroup"));
        VerlaufGroup->setGeometry(QRect(425, 170, 86, 126));
        HistoryScroll = new QScrollBar(VerlaufGroup);
        HistoryScroll->setObjectName(QString::fromUtf8("HistoryScroll"));
        HistoryScroll->setGeometry(QRect(60, 20, 16, 96));
        HistoryScroll->setMaximum(255);
        HistoryScroll->setOrientation(Qt::Vertical);
        HistoryList = new QListWidget(VerlaufGroup);
        HistoryList->setObjectName(QString::fromUtf8("HistoryList"));
        HistoryList->setGeometry(QRect(10, 20, 51, 99));
        HistoryList->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        HistoryList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        ExportGroup = new QGroupBox(DebuggerWindow);
        ExportGroup->setObjectName(QString::fromUtf8("ExportGroup"));
        ExportGroup->setGeometry(QRect(10, 616, 232, 53));
        label_export_von = new QLabel(ExportGroup);
        label_export_von->setObjectName(QString::fromUtf8("label_export_von"));
        label_export_von->setGeometry(QRect(8, 22, 25, 16));
        ExportStartIn = new QLineEdit(ExportGroup);
        ExportStartIn->setObjectName(QString::fromUtf8("ExportStartIn"));
        ExportStartIn->setGeometry(QRect(36, 20, 50, 20));
        ExportStartIn->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 200);"));
        ExportStartIn->setText(QString::fromUtf8("$0000"));
        label_export_bis = new QLabel(ExportGroup);
        label_export_bis->setObjectName(QString::fromUtf8("label_export_bis"));
        label_export_bis->setGeometry(QRect(92, 22, 21, 16));
        ExportEndIn = new QLineEdit(ExportGroup);
        ExportEndIn->setObjectName(QString::fromUtf8("ExportEndIn"));
        ExportEndIn->setGeometry(QRect(116, 20, 50, 20));
        ExportEndIn->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 200);"));
        ExportEndIn->setText(QString::fromUtf8("$FFFF"));
        ExportDisAss = new QPushButton(ExportGroup);
        ExportDisAss->setObjectName(QString::fromUtf8("ExportDisAss"));
        ExportDisAss->setGeometry(QRect(180, 15, 30, 30));
        ExportDisAss->setText(QString::fromUtf8(""));
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/grafik/export_24x24.png"), QSize(), QIcon::Normal, QIcon::Off);
        ExportDisAss->setIcon(icon9);
        ExportDisAss->setIconSize(QSize(22, 22));
        MemEdit = new QPushButton(DebuggerWindow);
        MemEdit->setObjectName(QString::fromUtf8("MemEdit"));
        MemEdit->setGeometry(QRect(456, 624, 45, 45));
        MemEdit->setText(QString::fromUtf8(""));
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/grafik/memory_icon.png"), QSize(), QIcon::Normal, QIcon::Off);
        MemEdit->setIcon(icon10);
        MemEdit->setIconSize(QSize(32, 32));
        VIA1 = new QPushButton(DebuggerWindow);
        VIA1->setObjectName(QString::fromUtf8("VIA1"));
        VIA1->setGeometry(QRect(10, 170, 41, 23));
        VIA1->setText(QString::fromUtf8("VIA 1"));
        VIA2 = new QPushButton(DebuggerWindow);
        VIA2->setObjectName(QString::fromUtf8("VIA2"));
        VIA2->setGeometry(QRect(55, 170, 41, 23));
        VIA2->setText(QString::fromUtf8("VIA 2"));

        retranslateUi(DebuggerWindow);

        QMetaObject::connectSlotsByName(DebuggerWindow);
    } // setupUi

    void retranslateUi(QDialog *DebuggerWindow)
    {
        DebuggerWindow->setWindowTitle(QApplication::translate("DebuggerWindow", "Debugger / Disassembler ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_pc->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Programm Z\303\244hler an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_sp->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Inhalt des Stackzeiger Registers + $100 an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_ac->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Inhalt des Accu Registers an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_xr->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Inhalt des X Registers an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_yr->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Inhalt des Y Registers an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_sr->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Inhalt des Status Registers an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        pc_out->setToolTip(QApplication::translate("DebuggerWindow", "Um den Wert zu \303\244ndern, bitte drauf klicken und mit RETURN best\303\244tigen.\n"
"Es k\303\266nnen folgende Zahlenformate eingegeben werden.\n"
"\n"
"- ohne Vorzeichen Dezimalsystem (Bsp. 128)\n"
"- $ Hexadezinal System (Bsp. $a7)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        pc_out->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_TOOLTIP
        sp_out->setToolTip(QApplication::translate("DebuggerWindow", "Um den Wert zu \303\244ndern, bitte drauf klicken und mit RETURN best\303\244tigen.\n"
"Es k\303\266nnen folgende Zahlenformate eingegeben werden.\n"
"\n"
"- ohne Vorzeichen Dezimalsystem (Bsp. 128)\n"
"- $ Hexadezinal System (Bsp. $a7)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        ac_out->setToolTip(QApplication::translate("DebuggerWindow", "Um den Wert zu \303\244ndern, bitte drauf klicken und mit RETURN best\303\244tigen.\n"
"Es k\303\266nnen folgende Zahlenformate eingegeben werden.\n"
"\n"
"- ohne Vorzeichen Dezimalsystem (Bsp. 128)\n"
"- $ Hexadezinal System (Bsp. $a7)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        xr_out->setToolTip(QApplication::translate("DebuggerWindow", "Um den Wert zu \303\244ndern, bitte drauf klicken und mit RETURN best\303\244tigen.\n"
"Es k\303\266nnen folgende Zahlenformate eingegeben werden.\n"
"\n"
"- ohne Vorzeichen Dezimalsystem (Bsp. 128)\n"
"- $ Hexadezinal System (Bsp. $a7)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        yr_out->setToolTip(QApplication::translate("DebuggerWindow", "Um den Wert zu \303\244ndern, bitte drauf klicken und mit RETURN best\303\244tigen.\n"
"Es k\303\266nnen folgende Zahlenformate eingegeben werden.\n"
"\n"
"- ohne Vorzeichen Dezimalsystem (Bsp. 128)\n"
"- $ Hexadezinal System (Bsp. $a7)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_irq->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den Inhalt des IRQ Hardware Vektors $FFFE an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_0314->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den Inhalt des IRQ Vektors $0314 an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_nmi->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den Inhalt des NMI Hardware Vektors $FFFA an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_0318->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den Inhalt des NMI Vektors $0318 an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        EingabeFeld->setText(QString());
        ChangeSource->clear();
        ChangeSource->insertItems(0, QStringList()
         << QApplication::translate("DebuggerWindow", "C64", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DebuggerWindow", "Floppy 1541 #08", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DebuggerWindow", "Floppy 1541 #09", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DebuggerWindow", "Floppy 1541 #10", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("DebuggerWindow", "Floppy 1541 #11", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        MCodeHelp->setToolTip(QApplication::translate("DebuggerWindow", "Hier steht was der n\303\244chste Micro Code macht.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_opcode_pc->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt die Position, an welche sich der aktuelle Opcode befindet.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_adresse->setToolTip(QApplication::translate("DebuggerWindow", "Adresse ist ein internes Hilfsregister.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_branch_adr->setToolTip(QApplication::translate("DebuggerWindow", "Branch Adresse ist ein internes Hilfsregister.\n"
"Hier wird die Sprungadresse der Brachbefehle gebildet.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_micro_code->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt an, welcher Micro Code als n\303\244chstes abgearbeitet wird.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_opcode->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Opcode an der gerade abgearbeitet wird.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_pointer->setToolTip(QApplication::translate("DebuggerWindow", "Pointer ist ein internes Hilfsregister.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_tmp_byte->setToolTip(QApplication::translate("DebuggerWindow", "Temp Byte ist ein internes Hilfsregister.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_mnemonic->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Opcode als Menmonic an der gerade abgearbeitet wird.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_cpu_wait->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt an ob die CPU gerade im \"Schlafmodus\" ist.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        groupBox_3->setTitle(QApplication::translate("DebuggerWindow", "Debugger Steuerung", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        OneZyklus->setToolTip(QApplication::translate("DebuggerWindow", "Es wird genau ein C64 Zyklus abgearbeitet.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        OneOpcode->setToolTip(QApplication::translate("DebuggerWindow", "Es wird genau ein Opcode abgearbeitet.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        AnimationStart->setToolTip(QApplication::translate("DebuggerWindow", "Die Programmausf\303\274hrung wird animiert. \n"
"Die Geschwindigkeit ist Einstellbar.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        AnimationStop->setToolTip(QApplication::translate("DebuggerWindow", "Die animierte Programmausf\303\274hrung wird gestoppt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        AnimationSpeed->setToolTip(QApplication::translate("DebuggerWindow", "Hiermit stellen sie die Animationsgeschwindigkeit ein.\n"
"In Zyklen pro Sekunde.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_anispeed->setText(QApplication::translate("DebuggerWindow", "Speed:", 0, QApplication::UnicodeUTF8));
        CycleGroup->setTitle(QApplication::translate("DebuggerWindow", "Zyklen Z\303\244hler", 0, QApplication::UnicodeUTF8));
        CycleCounterReset->setText(QApplication::translate("DebuggerWindow", "Auf Null stellen", 0, QApplication::UnicodeUTF8));
        VIC->setText(QApplication::translate("DebuggerWindow", "VIC", 0, QApplication::UnicodeUTF8));
        SID->setText(QApplication::translate("DebuggerWindow", "SID", 0, QApplication::UnicodeUTF8));
        CIA1->setText(QApplication::translate("DebuggerWindow", "CIA 1", 0, QApplication::UnicodeUTF8));
        CIA2->setText(QApplication::translate("DebuggerWindow", "CIA 2", 0, QApplication::UnicodeUTF8));
        LeitungenGroup->setTitle(QApplication::translate("DebuggerWindow", "Leitungen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label_irq_2->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Programm Z\303\244hler an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_nmi_2->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Programm Z\303\244hler an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_ba->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Programm Z\303\244hler an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_reset->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Programm Z\303\244hler an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_exrom->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Programm Z\303\244hler an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        label_game->setToolTip(QApplication::translate("DebuggerWindow", "Zeigt den aktuellen Programm Z\303\244hler an.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        IEC->setText(QApplication::translate("DebuggerWindow", "IEC BUS", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        man_lines->setToolTip(QApplication::translate("DebuggerWindow", "Ist hier ein Haken gesetzt, so werden alle 3 Leitungen\n"
"IRQ;NMI;RDY zu den Customchips getrennt. So hat man die\n"
"m\303\266glichkeit die Leitungen manuell zu setzen. Dies dient nur zu\n"
"Testzwecken.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        man_lines->setText(QString());
        DisassGroup->setTitle(QApplication::translate("DebuggerWindow", "Disassembler / Assembler", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DebuggerWindow", "ASM", 0, QApplication::UnicodeUTF8));
        label_floppy_off->setText(QApplication::translate("DebuggerWindow", "Diese Floppy ist ausgeschaltet.", 0, QApplication::UnicodeUTF8));
        BreakGroup->setTitle(QApplication::translate("DebuggerWindow", "Haltepunkte", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        AddBreakpoint->setToolTip(QApplication::translate("DebuggerWindow", "F\303\274gt ein Haltepunkt hinzu.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        DelBreakpoint->setToolTip(QApplication::translate("DebuggerWindow", "Entfernt ein Haltepunkt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        LoadBreakpoints->setToolTip(QApplication::translate("DebuggerWindow", "L\303\244dt Haltepunkte aus einer Datei.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        SaveBreakpoints->setToolTip(QApplication::translate("DebuggerWindow", "Speichert alle erstellten Haltepunkte in einer Datei ab.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        BreakpointTree->setToolTip(QApplication::translate("DebuggerWindow", "Liste aller Haltepunkte.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        DelAllBreakpoints->setToolTip(QApplication::translate("DebuggerWindow", "L\303\266scht alle Haltepunkte.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        VerlaufGroup->setTitle(QApplication::translate("DebuggerWindow", "Verlauf", 0, QApplication::UnicodeUTF8));
        ExportGroup->setTitle(QApplication::translate("DebuggerWindow", "Export", 0, QApplication::UnicodeUTF8));
        label_export_von->setText(QApplication::translate("DebuggerWindow", "Von:", 0, QApplication::UnicodeUTF8));
        label_export_bis->setText(QApplication::translate("DebuggerWindow", "Bis:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ExportDisAss->setToolTip(QApplication::translate("DebuggerWindow", "Export als Disassembler-Listing oder PRG", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        ExportDisAss->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_TOOLTIP
        MemEdit->setToolTip(QApplication::translate("DebuggerWindow", "\303\226ffnet das Speicher Edit Fenster.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
    } // retranslateUi

};

namespace Ui {
    class DebuggerWindow: public Ui_DebuggerWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGGER_WINDOW_H
