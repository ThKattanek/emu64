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
// Letzte Änderung am 18.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef DEBUGGER_WINDOW_H
#define DEBUGGER_WINDOW_H

#include <QTreeWidgetItem>
#include <QTableWidgetItem>
#include <QListWidget>
#include <QSettings>
#include <QMenu>
#include <QTimer>
#include <QFontDatabase>

#include "./c64_class.h"
#include "./custom_save_file_dialog.h"
#include "./label_widget_mod.h"
#include "./cpu_info.h"
#include "./memory_window.h"
#include "./debugger_vic_window.h"
#include "./debugger_iec_window.h"
#include "./input_box_window.h"

#define MAX_DISASS_ROW 255
//#define HISTORY_ROW 7

class QNewListWidget : public QListWidget
{
    Q_OBJECT
public:

    QNewListWidget( QWidget* Parent = nullptr ) :
        QListWidget( Parent )
    {
    }

signals:
    void resize(int width, int height);

protected:

    virtual void resizeEvent( QResizeEvent* e )
    {
      (void)e;
      emit resize(this->width(), this->height());
    }
};

class QNewTableWidget : public QTableWidget
{
    Q_OBJECT
public:

    QNewTableWidget( QWidget* Parent = nullptr ) :
        QTableWidget( Parent )
    {
    }

signals:
    void resize(int width, int height);

protected:

    virtual void resizeEvent( QResizeEvent* e )
    {
      (void)e;
      emit resize(this->width(), this->height());
    }
};

namespace Ui {
    class DebuggerWindow;
}

class DebuggerWindow : public QDialog
{
    Q_OBJECT

public:
    explicit DebuggerWindow(QWidget* parent = nullptr, QSettings* ini = nullptr);
    ~DebuggerWindow();
    void showEvent(QShowEvent* event);
    void hideEvent(QHideEvent* event);
    void RetranslateUi();
    void SetC64Pointer(C64Class* c64);
    void AnimationRefreshProc();
    void BreakpointProc();

    REG_STRUCT c64_cpu_reg;     // C64 CPU Register
    IREG_STRUCT c64_cpu_ireg;   // Interne C64 CPU Hilfs Register

    REG_STRUCT floppy_cpu_reg[MAX_FLOPPY_NUM];    // Floppy CPU Register
    IREG_STRUCT floppy_cpu_ireg[MAX_FLOPPY_NUM];  // Interne Floppy CPU Hilfs Register

private slots:
    void on_OneOpcode_clicked();
    void on_OneZyklus_clicked();
    void on_CycleCounterReset_clicked();
    void on_ChangeSource_currentIndexChanged(int index);
    void on_AssAdresseIn_returnPressed();
    void on_AssMnemonicIn_textChanged(const QString &arg1);
    void on_AssAdressierungIn_returnPressed();
    void on_AnimationStart_clicked();
    void on_AnimationStop_clicked();
    void on_AnimationSpeed_valueChanged(int value);
    void on_DisAssTable_doubleClicked(const QModelIndex &index);
    void on_DisAssScroll_valueChanged(int value);
    void on_AddBreakpoint_clicked();
    void on_BreakpointTree_itemChanged(QTreeWidgetItem* item, int column);
    void on_DelBreakpoint_clicked();
    void on_LoadBreakpoints_clicked();
    void on_SaveBreakpoints_clicked();
    void on_DelAllBreakpoints_clicked();
    void on_HistoryScroll_valueChanged(int value);
    void on_ExportDisAss_clicked();
    void on_MemEdit_clicked();
    void on_VIC_clicked();
    void on_IEC_clicked();
    void on_man_lines_clicked(bool checked);
    void on_rdy_led_clicked(bool checked);
    void on_irq_led_clicked(bool checked);
    void on_nmi_led_clicked(bool checked);

    void onShowContextMenu(const QPoint& pos);
    void onSr_widget_ValueChange(uint8_t value);
    void onReg_label_clicked(LabelWidgetMod* label);
    void onChangeFloppyStatus();
    void onTimerAnimationRefresh();
    void onResizeHistoryList(int weidth, int height);
    void onResizeDisassList(int weidth, int height);

    void on_HistoryList_doubleClicked(const QModelIndex &index);

private:

    void UpdateRegister(void);
    void FillDisassemblyList(uint16_t address, bool new_refresh);
    void FillHistoryList(uint8_t index);
    bool FindMnemonic(QString mnemonic, uint8_t* opcode = nullptr, uint8_t *opcode_count = nullptr);
    bool FindAddressing(QString address_string, uint8_t* address_type, uint16_t* address_value);
    bool Assemble(QString address, QString mnemonic, QString addressing, uint16_t* ass_address, uint16_t* new_adress);
    void AddBreakpointTreeRoot(QString name, BREAK_GROUP* bg);
    void AddBreakpointTreeChild(QTreeWidgetItem* parent, uint16_t value1, uint16_t value2, uint8_t checked, QString tooltip);
    void ClearAllBreakpointBackcolors();
    void RefreshGUI();

    C64Class *c64;
    Ui::DebuggerWindow *ui;
    InputBoxWindow *input_window;
    MemoryWindow *memory_window;
    DebuggerVicWindow *vic_window;
    DebuggerIECWindow *iec_window;
    QSettings *ini;
    QStringList rw_string;
    bool is_one_showed;
    int32_t current_edit_reg;
    QIcon *icon_off;
    QIcon *icon_on;
    QMenu *context_diss_assList;
    QTableWidgetItem *disass_pc[MAX_DISASS_ROW];
    QTableWidgetItem *disass_memory[MAX_DISASS_ROW];
    QTableWidgetItem *disass_mnemonic[MAX_DISASS_ROW];
    QTableWidgetItem *disass_addressing[MAX_DISASS_ROW];
    QColor table_back_color;
    QColor table_position_color;
    uint16_t view_code_address[MAX_DISASS_ROW];
    uint16_t old_adresse;
    int32_t old_make_idx;
    QTimer *timer1;
    bool new_refresh;
    bool break_point_update_enable;
    bool new_breakpoint_found;
    int32_t current_source;
    int32_t currnet_floppy_nr;
    int16_t history_rows;
    int16_t disass_rows;
};

#endif // DEBUGGER_WINDOW_H
