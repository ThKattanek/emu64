//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: debugger_window.h                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 02.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef DEBUGGER_WINDOW_H
#define DEBUGGER_WINDOW_H

#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QSettings>

#include "c64_class.h"
#include "label_widget_mod.h"
#include "cpu_info.h"
#include "memory_window.h"
#include "debugger_vic_window.h"
#include "debugger_iec_window.h"

#define DisAssZeilen 20
#define HistoryZeilen 7

namespace Ui {
    class DebuggerWindow;
}

class DebuggerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DebuggerWindow(QWidget *parent = 0, QSettings *ini = 0);
    ~DebuggerWindow();
    void showEvent(QShowEvent *event);
    void hideEvent(QHideEvent *event);
    void RetranslateUi();
    void SetC64Pointer(C64Class *c64);
    void AnimationRefreshProc(void);
    void BreakpointProc(void);

    REG_STRUCT C64CpuReg;
    IREG_STRUCT C64CpuIReg;

    REG_STRUCT FloppyCpuReg[FLOPPY_COUNT];
    IREG_STRUCT FloppyCpuIReg[FLOPPY_COUNT];

private slots:
    void onShowContextMenu(const QPoint& pos);
    void onSr_widget_ValueChange(unsigned char value);
    void onReg_label_clicked(LabelWidgetMod* label);
    void on_OneOpcode_clicked();
    void on_EingabeFeld_returnPressed();
    void on_OneZyklus_clicked();
    void on_CycleCounterReset_clicked();
    void on_ChangeSource_currentIndexChanged(int index);
    void on_AssAdresseIn_returnPressed();
    void on_AssMnemonicIn_textChanged(const QString &arg1);
    void on_AssAdressierungIn_returnPressed();
    void on_AnimationStart_clicked();
    void on_AnimationStop_clicked();
    void onTimerAnimationRefresh(void);
    void on_AnimationSpeed_valueChanged(int value);
    void on_DisAssTable_doubleClicked(const QModelIndex &index);
    void on_DisAssScroll_valueChanged(int value);
    void on_AddBreakpoint_clicked();
    void on_BreakpointTree_itemChanged(QTreeWidgetItem *item, int column);
    void on_DelBreakpoint_clicked();
    void on_LoadBreakpoints_clicked();
    void on_SaveBreakpoints_clicked();
    void on_DelAllBreakpoints_clicked();
    void on_HistoryScroll_valueChanged(int value);
    void on_ExportDisAss_clicked();
    void onChangeFloppyStatus(void);
    void on_MemEdit_clicked();
    void on_VIC_clicked();
    void on_IEC_clicked();
    void on_man_lines_clicked(bool checked);
    void on_rdy_led_clicked(bool checked);
    void on_irq_led_clicked(bool checked);
    void on_nmi_led_clicked(bool checked);

private:

    void UpdateRegister(void);
    bool getSaveFileName(QWidget *parent, QString caption, QString filter, QString *fileName, QString *fileExt);
    void FillDisassemblerList(unsigned short adresse,bool new_refresh);
    void FillHistoryList(unsigned char index);
    bool FindMnemonic(QString mnemonic, unsigned char *opcode = 0, int *opcode_anzahl = 0);
    bool FindAdressierung(QString adr_string, unsigned char *adr_typ, unsigned short *adr_wert);
    bool Assemble(QString adresse, QString mnemonic, QString adressierung, unsigned short *ass_adress, unsigned short *new_adress);
    void AddBreakpointTreeRoot(QString name,BREAK_GROUP *bg);
    void AddBreakpointTreeChild(QTreeWidgetItem *parent, unsigned short value, unsigned char checked, QString tooltip);
    void ClearAllBreakpointBackcolors(void);
    void FillMicroCodeStringTable(void);
    void RefreshGUI(void);

    C64Class *c64;
    Ui::DebuggerWindow *ui;
    MemoryWindow *memory_window;
    DebuggerVicWindow *vic_window;
    DebuggerIECWindow *iec_window;
    QSettings *ini;
    QStringList MicroCodeStringTable6510;
    QStringList RWString;
    bool isOneShowed;
    int AktEditReg;
    QIcon *iOff;
    QIcon *iOn;
    QMenu *ContextDisAssList;
    QTableWidgetItem *DisAssPC[DisAssZeilen];
    QTableWidgetItem *DisAssMem[DisAssZeilen];
    QTableWidgetItem *DisAssMenmo[DisAssZeilen];
    QTableWidgetItem *DisAssAdr[DisAssZeilen];
    QColor TableBackColor;
    QColor TablePosColor;
    unsigned short ViewCodeAdressen[DisAssZeilen];
    unsigned short old_adresse;
    int old_make_idx;
    QTimer *timer1;
    bool NewRefresh;
    bool BreakPointUpdateEnable;
    bool NewBreakpointfound;
    int AktSource;
    int AktFloppyNr;
};

#endif // DEBUGGER_WINDOW_H
