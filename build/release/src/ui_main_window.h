/********************************************************************************
** Form generated from reading UI file 'main_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAIN_WINDOW_H
#define UI_MAIN_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QMainWindow>
#include <QtGui/QMenu>
#include <QtGui/QMenuBar>
#include <QtGui/QStatusBar>
#include <QtGui/QTableWidget>
#include <QtGui/QToolBar>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QAction *menu_main_info;
    QAction *actionFloppy_1541_II;
    QAction *actionBandlaufwerk_1530;
    QAction *actionSteckmodule;
    QAction *actionAutostart;
    QAction *actionC64_Programme_direkt_laden;
    QAction *actionSoftreset;
    QAction *actionHardreset;
    QAction *actionBeenden;
    QAction *actionQuick_C64_Snapshot;
    QAction *actionScreenshot;
    QAction *actionSID_Dump;
    QAction *actionSID_Player;
    QAction *actionDebugger_Disassembler;
    QAction *actionC64_Geschwindigkeit;
    QAction *actionCross_Assembler;
    QAction *actionLan64;
    QAction *actionVirtuelle_C64_Tastatur;
    QAction *actionVideo_Capture;
    QAction *actionEmu64_Einstellungen;
    QAction *actionTV_Video_Einstellungen;
    QAction *actionVollbild;
    QAction *actionC64_Tastaturbelegung_ndern;
    QAction *actionC64_Tastaturbelegung_zur_cksetzen;
    QAction *actionFenster_Optionen;
    QAction *actionHandbuch;
    QAction *actionGameBase64;
    QAction *actionREU_einstecken;
    QAction *actionREU_entfernen;
    QAction *actionREU_laden;
    QAction *actionREU_speichern;
    QAction *actionREU_loeschen;
    QAction *actionGEO_einstecken;
    QAction *actionGEO_entfernen;
    QAction *actionGEO_laden;
    QAction *actionGEO_speichern;
    QAction *actionGEO_loeschen;
    QAction *actionC64_Tastenbelegung_Show;
    QWidget *centralWidget;
    QTableWidget *FloppyTabel;
    QStatusBar *statusBar;
    QToolBar *toolBar;
    QMenuBar *menuBar;
    QMenu *menu_main_help;
    QMenu *menuC64_Dateien;
    QMenu *menuReset;
    QMenu *menuTools;
    QMenu *menuREU_1764_16MB;
    QMenu *menuGEO_NEO_RAM_2MB;
    QMenu *menuEinstellungen;
    QMenu *menuSprache;
    QMenu *menuDatei;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->setWindowModality(Qt::NonModal);
        MainWindow->resize(482, 204);
        QFont font;
        font.setPointSize(8);
        MainWindow->setFont(font);
        MainWindow->setMouseTracking(false);
        MainWindow->setContextMenuPolicy(Qt::NoContextMenu);
        MainWindow->setWindowTitle(QString::fromUtf8(""));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/grafik/emu64.ico"), QSize(), QIcon::Normal, QIcon::Off);
        MainWindow->setWindowIcon(icon);
        MainWindow->setWindowOpacity(1);
        MainWindow->setAutoFillBackground(false);
        MainWindow->setStyleSheet(QString::fromUtf8("#playButton{\n"
"background-image: url(:/Grafik/play_blue_transprent.png);\n"
"background-repeat: no-repeat;\n"
"background-position: center\n"
"}\n"
"\n"
"#stopButton{\n"
"background-image: url(:/Grafik/stop_blue_transprent.png);\n"
"background-repeat: no-repeat;\n"
"background-position: center\n"
"}"));
        MainWindow->setToolButtonStyle(Qt::ToolButtonTextOnly);
        MainWindow->setDockNestingEnabled(false);
        MainWindow->setUnifiedTitleAndToolBarOnMac(false);
        menu_main_info = new QAction(MainWindow);
        menu_main_info->setObjectName(QString::fromUtf8("menu_main_info"));
        menu_main_info->setCheckable(false);
        menu_main_info->setEnabled(true);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/grafik/toolbar32/info.png"), QSize(), QIcon::Normal, QIcon::Off);
        menu_main_info->setIcon(icon1);
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setBold(false);
        font1.setWeight(50);
        menu_main_info->setFont(font1);
        menu_main_info->setIconVisibleInMenu(true);
        actionFloppy_1541_II = new QAction(MainWindow);
        actionFloppy_1541_II->setObjectName(QString::fromUtf8("actionFloppy_1541_II"));
        actionFloppy_1541_II->setEnabled(true);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/grafik/toolbar32/floppy.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionFloppy_1541_II->setIcon(icon2);
        actionFloppy_1541_II->setFont(font1);
        actionFloppy_1541_II->setShortcut(QString::fromUtf8("Alt+D"));
        actionFloppy_1541_II->setIconVisibleInMenu(true);
        actionBandlaufwerk_1530 = new QAction(MainWindow);
        actionBandlaufwerk_1530->setObjectName(QString::fromUtf8("actionBandlaufwerk_1530"));
        actionBandlaufwerk_1530->setEnabled(true);
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/grafik/toolbar32/datasette.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBandlaufwerk_1530->setIcon(icon3);
        actionBandlaufwerk_1530->setFont(font1);
        actionBandlaufwerk_1530->setShortcut(QString::fromUtf8("Alt+K"));
        actionBandlaufwerk_1530->setIconVisibleInMenu(true);
        actionSteckmodule = new QAction(MainWindow);
        actionSteckmodule->setObjectName(QString::fromUtf8("actionSteckmodule"));
        actionSteckmodule->setEnabled(true);
        QIcon icon4;
        icon4.addFile(QString::fromUtf8(":/grafik/toolbar32/crt.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSteckmodule->setIcon(icon4);
        actionSteckmodule->setFont(font1);
        actionSteckmodule->setShortcut(QString::fromUtf8("Alt+M"));
        actionSteckmodule->setIconVisibleInMenu(true);
        actionAutostart = new QAction(MainWindow);
        actionAutostart->setObjectName(QString::fromUtf8("actionAutostart"));
        actionAutostart->setEnabled(true);
        actionAutostart->setFont(font1);
        actionAutostart->setShortcut(QString::fromUtf8("Alt+R"));
        actionAutostart->setIconVisibleInMenu(true);
        actionC64_Programme_direkt_laden = new QAction(MainWindow);
        actionC64_Programme_direkt_laden->setObjectName(QString::fromUtf8("actionC64_Programme_direkt_laden"));
        actionC64_Programme_direkt_laden->setCheckable(false);
        actionC64_Programme_direkt_laden->setChecked(false);
        actionC64_Programme_direkt_laden->setEnabled(true);
        actionC64_Programme_direkt_laden->setFont(font1);
        actionC64_Programme_direkt_laden->setShortcut(QString::fromUtf8("Alt+L"));
        actionC64_Programme_direkt_laden->setIconVisibleInMenu(true);
        actionSoftreset = new QAction(MainWindow);
        actionSoftreset->setObjectName(QString::fromUtf8("actionSoftreset"));
        actionSoftreset->setEnabled(true);
        QIcon icon5;
        icon5.addFile(QString::fromUtf8(":/grafik/toolbar32/sreset.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionSoftreset->setIcon(icon5);
        actionSoftreset->setFont(font1);
        actionSoftreset->setShortcut(QString::fromUtf8("F12"));
        actionSoftreset->setIconVisibleInMenu(true);
        actionHardreset = new QAction(MainWindow);
        actionHardreset->setObjectName(QString::fromUtf8("actionHardreset"));
        actionHardreset->setEnabled(true);
        QIcon icon6;
        icon6.addFile(QString::fromUtf8(":/grafik/toolbar32/hreset.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionHardreset->setIcon(icon6);
        actionHardreset->setFont(font1);
        actionHardreset->setShortcut(QString::fromUtf8("Shift+F12"));
        actionHardreset->setIconVisibleInMenu(true);
        actionBeenden = new QAction(MainWindow);
        actionBeenden->setObjectName(QString::fromUtf8("actionBeenden"));
        actionBeenden->setEnabled(true);
        QIcon icon7;
        icon7.addFile(QString::fromUtf8(":/grafik/toolbar32/aus.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionBeenden->setIcon(icon7);
        actionBeenden->setFont(font1);
        actionBeenden->setShortcut(QString::fromUtf8("Alt+X"));
        actionBeenden->setIconVisibleInMenu(true);
        actionQuick_C64_Snapshot = new QAction(MainWindow);
        actionQuick_C64_Snapshot->setObjectName(QString::fromUtf8("actionQuick_C64_Snapshot"));
        actionQuick_C64_Snapshot->setEnabled(false);
        actionQuick_C64_Snapshot->setFont(font1);
        actionQuick_C64_Snapshot->setShortcut(QString::fromUtf8(""));
        actionQuick_C64_Snapshot->setIconVisibleInMenu(true);
        actionScreenshot = new QAction(MainWindow);
        actionScreenshot->setObjectName(QString::fromUtf8("actionScreenshot"));
        actionScreenshot->setEnabled(true);
        QIcon icon8;
        icon8.addFile(QString::fromUtf8(":/grafik/toolbar32/screenshot.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionScreenshot->setIcon(icon8);
        actionScreenshot->setFont(font1);
        actionScreenshot->setIconVisibleInMenu(true);
        actionSID_Dump = new QAction(MainWindow);
        actionSID_Dump->setObjectName(QString::fromUtf8("actionSID_Dump"));
        actionSID_Dump->setEnabled(false);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(9);
        font2.setBold(false);
        font2.setWeight(50);
        actionSID_Dump->setFont(font2);
        actionSID_Dump->setIconVisibleInMenu(true);
        actionSID_Player = new QAction(MainWindow);
        actionSID_Player->setObjectName(QString::fromUtf8("actionSID_Player"));
        actionSID_Player->setEnabled(false);
        actionSID_Player->setFont(font1);
        actionSID_Player->setIconVisibleInMenu(true);
        actionDebugger_Disassembler = new QAction(MainWindow);
        actionDebugger_Disassembler->setObjectName(QString::fromUtf8("actionDebugger_Disassembler"));
        actionDebugger_Disassembler->setEnabled(true);
        QIcon icon9;
        icon9.addFile(QString::fromUtf8(":/grafik/toolbar32/debug.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionDebugger_Disassembler->setIcon(icon9);
        actionDebugger_Disassembler->setFont(font1);
        actionDebugger_Disassembler->setIconVisibleInMenu(true);
        actionC64_Geschwindigkeit = new QAction(MainWindow);
        actionC64_Geschwindigkeit->setObjectName(QString::fromUtf8("actionC64_Geschwindigkeit"));
        actionC64_Geschwindigkeit->setEnabled(true);
        QIcon icon10;
        icon10.addFile(QString::fromUtf8(":/grafik/toolbar32/speed.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionC64_Geschwindigkeit->setIcon(icon10);
        actionC64_Geschwindigkeit->setFont(font1);
        actionC64_Geschwindigkeit->setIconVisibleInMenu(true);
        actionCross_Assembler = new QAction(MainWindow);
        actionCross_Assembler->setObjectName(QString::fromUtf8("actionCross_Assembler"));
        actionCross_Assembler->setEnabled(false);
        actionCross_Assembler->setFont(font1);
        actionCross_Assembler->setIconVisibleInMenu(true);
        actionLan64 = new QAction(MainWindow);
        actionLan64->setObjectName(QString::fromUtf8("actionLan64"));
        actionLan64->setEnabled(false);
        actionLan64->setFont(font1);
        actionLan64->setIconVisibleInMenu(true);
        actionVirtuelle_C64_Tastatur = new QAction(MainWindow);
        actionVirtuelle_C64_Tastatur->setObjectName(QString::fromUtf8("actionVirtuelle_C64_Tastatur"));
        actionVirtuelle_C64_Tastatur->setEnabled(true);
        actionVirtuelle_C64_Tastatur->setFont(font1);
        actionVirtuelle_C64_Tastatur->setIconVisibleInMenu(true);
        actionVideo_Capture = new QAction(MainWindow);
        actionVideo_Capture->setObjectName(QString::fromUtf8("actionVideo_Capture"));
        actionVideo_Capture->setEnabled(true);
        actionVideo_Capture->setFont(font1);
        actionVideo_Capture->setIconVisibleInMenu(true);
        actionEmu64_Einstellungen = new QAction(MainWindow);
        actionEmu64_Einstellungen->setObjectName(QString::fromUtf8("actionEmu64_Einstellungen"));
        actionEmu64_Einstellungen->setEnabled(true);
        actionEmu64_Einstellungen->setFont(font1);
        actionEmu64_Einstellungen->setIconVisibleInMenu(true);
        actionTV_Video_Einstellungen = new QAction(MainWindow);
        actionTV_Video_Einstellungen->setObjectName(QString::fromUtf8("actionTV_Video_Einstellungen"));
        actionTV_Video_Einstellungen->setEnabled(true);
        actionTV_Video_Einstellungen->setFont(font1);
        actionTV_Video_Einstellungen->setIconVisibleInMenu(true);
        actionVollbild = new QAction(MainWindow);
        actionVollbild->setObjectName(QString::fromUtf8("actionVollbild"));
        actionVollbild->setEnabled(true);
        actionVollbild->setFont(font1);
        actionVollbild->setShortcut(QString::fromUtf8("Alt+Return"));
        actionVollbild->setIconVisibleInMenu(true);
        actionC64_Tastaturbelegung_ndern = new QAction(MainWindow);
        actionC64_Tastaturbelegung_ndern->setObjectName(QString::fromUtf8("actionC64_Tastaturbelegung_ndern"));
        actionC64_Tastaturbelegung_ndern->setEnabled(true);
        actionC64_Tastaturbelegung_ndern->setFont(font1);
        actionC64_Tastaturbelegung_ndern->setIconVisibleInMenu(true);
        actionC64_Tastaturbelegung_zur_cksetzen = new QAction(MainWindow);
        actionC64_Tastaturbelegung_zur_cksetzen->setObjectName(QString::fromUtf8("actionC64_Tastaturbelegung_zur_cksetzen"));
        actionC64_Tastaturbelegung_zur_cksetzen->setEnabled(false);
        actionC64_Tastaturbelegung_zur_cksetzen->setFont(font1);
        actionC64_Tastaturbelegung_zur_cksetzen->setIconVisibleInMenu(true);
        actionFenster_Optionen = new QAction(MainWindow);
        actionFenster_Optionen->setObjectName(QString::fromUtf8("actionFenster_Optionen"));
        actionFenster_Optionen->setEnabled(false);
        actionFenster_Optionen->setFont(font1);
        actionFenster_Optionen->setIconVisibleInMenu(true);
        actionHandbuch = new QAction(MainWindow);
        actionHandbuch->setObjectName(QString::fromUtf8("actionHandbuch"));
        actionHandbuch->setEnabled(false);
        QIcon icon11;
        icon11.addFile(QString::fromUtf8(":/grafik/toolbar32/handbuch.png"), QSize(), QIcon::Normal, QIcon::Off);
        actionHandbuch->setIcon(icon11);
        actionHandbuch->setFont(font1);
        actionHandbuch->setIconVisibleInMenu(true);
        actionGameBase64 = new QAction(MainWindow);
        actionGameBase64->setObjectName(QString::fromUtf8("actionGameBase64"));
        actionGameBase64->setEnabled(false);
        actionGameBase64->setFont(font1);
        actionGameBase64->setIconVisibleInMenu(true);
        actionREU_einstecken = new QAction(MainWindow);
        actionREU_einstecken->setObjectName(QString::fromUtf8("actionREU_einstecken"));
        actionREU_entfernen = new QAction(MainWindow);
        actionREU_entfernen->setObjectName(QString::fromUtf8("actionREU_entfernen"));
        actionREU_laden = new QAction(MainWindow);
        actionREU_laden->setObjectName(QString::fromUtf8("actionREU_laden"));
        actionREU_speichern = new QAction(MainWindow);
        actionREU_speichern->setObjectName(QString::fromUtf8("actionREU_speichern"));
        actionREU_loeschen = new QAction(MainWindow);
        actionREU_loeschen->setObjectName(QString::fromUtf8("actionREU_loeschen"));
        actionGEO_einstecken = new QAction(MainWindow);
        actionGEO_einstecken->setObjectName(QString::fromUtf8("actionGEO_einstecken"));
        actionGEO_entfernen = new QAction(MainWindow);
        actionGEO_entfernen->setObjectName(QString::fromUtf8("actionGEO_entfernen"));
        actionGEO_laden = new QAction(MainWindow);
        actionGEO_laden->setObjectName(QString::fromUtf8("actionGEO_laden"));
        actionGEO_speichern = new QAction(MainWindow);
        actionGEO_speichern->setObjectName(QString::fromUtf8("actionGEO_speichern"));
        actionGEO_loeschen = new QAction(MainWindow);
        actionGEO_loeschen->setObjectName(QString::fromUtf8("actionGEO_loeschen"));
        actionC64_Tastenbelegung_Show = new QAction(MainWindow);
        actionC64_Tastenbelegung_Show->setObjectName(QString::fromUtf8("actionC64_Tastenbelegung_Show"));
        actionC64_Tastenbelegung_Show->setCheckable(false);
        actionC64_Tastenbelegung_Show->setVisible(true);
        centralWidget = new QWidget(MainWindow);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        centralWidget->setFont(font);
        FloppyTabel = new QTableWidget(centralWidget);
        FloppyTabel->setObjectName(QString::fromUtf8("FloppyTabel"));
        FloppyTabel->setGeometry(QRect(10, 10, 411, 103));
        FloppyTabel->setFocusPolicy(Qt::NoFocus);
        FloppyTabel->setAutoFillBackground(false);
        FloppyTabel->setSelectionMode(QAbstractItemView::NoSelection);
        FloppyTabel->setShowGrid(false);
        FloppyTabel->horizontalHeader()->setVisible(false);
        FloppyTabel->horizontalHeader()->setHighlightSections(false);
        FloppyTabel->verticalHeader()->setVisible(false);
        FloppyTabel->verticalHeader()->setHighlightSections(false);
        MainWindow->setCentralWidget(centralWidget);
        statusBar = new QStatusBar(MainWindow);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        statusBar->setMinimumSize(QSize(0, 0));
        statusBar->setFont(font1);
        statusBar->setAutoFillBackground(false);
        statusBar->setSizeGripEnabled(false);
        MainWindow->setStatusBar(statusBar);
        toolBar = new QToolBar(MainWindow);
        toolBar->setObjectName(QString::fromUtf8("toolBar"));
        toolBar->setEnabled(true);
        toolBar->setWindowTitle(QString::fromUtf8("toolBar"));
        toolBar->setMovable(true);
        toolBar->setAllowedAreas(Qt::AllToolBarAreas);
        toolBar->setIconSize(QSize(32, 32));
        toolBar->setToolButtonStyle(Qt::ToolButtonIconOnly);
        MainWindow->addToolBar(Qt::TopToolBarArea, toolBar);
        menuBar = new QMenuBar(MainWindow);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 482, 16));
        menuBar->setFont(font1);
        menuBar->setDefaultUp(false);
        menuBar->setNativeMenuBar(false);
        menu_main_help = new QMenu(menuBar);
        menu_main_help->setObjectName(QString::fromUtf8("menu_main_help"));
        menu_main_help->setFont(font1);
        menuC64_Dateien = new QMenu(menuBar);
        menuC64_Dateien->setObjectName(QString::fromUtf8("menuC64_Dateien"));
        menuReset = new QMenu(menuBar);
        menuReset->setObjectName(QString::fromUtf8("menuReset"));
        menuTools = new QMenu(menuBar);
        menuTools->setObjectName(QString::fromUtf8("menuTools"));
        menuREU_1764_16MB = new QMenu(menuTools);
        menuREU_1764_16MB->setObjectName(QString::fromUtf8("menuREU_1764_16MB"));
        menuREU_1764_16MB->setTearOffEnabled(false);
        menuGEO_NEO_RAM_2MB = new QMenu(menuTools);
        menuGEO_NEO_RAM_2MB->setObjectName(QString::fromUtf8("menuGEO_NEO_RAM_2MB"));
        menuEinstellungen = new QMenu(menuBar);
        menuEinstellungen->setObjectName(QString::fromUtf8("menuEinstellungen"));
        menuSprache = new QMenu(menuBar);
        menuSprache->setObjectName(QString::fromUtf8("menuSprache"));
        menuSprache->setFont(font1);
        menuDatei = new QMenu(menuBar);
        menuDatei->setObjectName(QString::fromUtf8("menuDatei"));
        MainWindow->setMenuBar(menuBar);

        toolBar->addAction(actionBeenden);
        toolBar->addAction(menu_main_info);
        toolBar->addAction(actionScreenshot);
        toolBar->addAction(actionFloppy_1541_II);
        toolBar->addAction(actionBandlaufwerk_1530);
        toolBar->addAction(actionSteckmodule);
        toolBar->addAction(actionDebugger_Disassembler);
        toolBar->addAction(actionSoftreset);
        toolBar->addAction(actionHardreset);
        toolBar->addAction(actionC64_Geschwindigkeit);
        toolBar->addAction(actionHandbuch);
        menuBar->addAction(menuDatei->menuAction());
        menuBar->addAction(menuC64_Dateien->menuAction());
        menuBar->addAction(menuReset->menuAction());
        menuBar->addAction(menuTools->menuAction());
        menuBar->addAction(menuEinstellungen->menuAction());
        menuBar->addAction(menu_main_help->menuAction());
        menuBar->addAction(menuSprache->menuAction());
        menu_main_help->addAction(menu_main_info);
        menu_main_help->addAction(actionHandbuch);
        menu_main_help->addSeparator();
        menu_main_help->addAction(actionC64_Tastenbelegung_Show);
        menuC64_Dateien->addAction(actionFloppy_1541_II);
        menuC64_Dateien->addAction(actionBandlaufwerk_1530);
        menuC64_Dateien->addAction(actionSteckmodule);
        menuC64_Dateien->addAction(actionAutostart);
        menuC64_Dateien->addAction(actionC64_Programme_direkt_laden);
        menuReset->addAction(actionSoftreset);
        menuReset->addAction(actionHardreset);
        menuTools->addAction(actionQuick_C64_Snapshot);
        menuTools->addAction(actionScreenshot);
        menuTools->addAction(actionSID_Dump);
        menuTools->addAction(actionSID_Player);
        menuTools->addAction(actionDebugger_Disassembler);
        menuTools->addAction(actionC64_Geschwindigkeit);
        menuTools->addAction(actionCross_Assembler);
        menuTools->addAction(actionLan64);
        menuTools->addAction(actionVirtuelle_C64_Tastatur);
        menuTools->addAction(actionGameBase64);
        menuTools->addAction(menuREU_1764_16MB->menuAction());
        menuTools->addAction(menuGEO_NEO_RAM_2MB->menuAction());
        menuTools->addAction(actionVideo_Capture);
        menuREU_1764_16MB->addAction(actionREU_einstecken);
        menuREU_1764_16MB->addAction(actionREU_entfernen);
        menuREU_1764_16MB->addSeparator();
        menuREU_1764_16MB->addAction(actionREU_laden);
        menuREU_1764_16MB->addAction(actionREU_speichern);
        menuREU_1764_16MB->addAction(actionREU_loeschen);
        menuGEO_NEO_RAM_2MB->addAction(actionGEO_einstecken);
        menuGEO_NEO_RAM_2MB->addAction(actionGEO_entfernen);
        menuGEO_NEO_RAM_2MB->addSeparator();
        menuGEO_NEO_RAM_2MB->addAction(actionGEO_laden);
        menuGEO_NEO_RAM_2MB->addAction(actionGEO_speichern);
        menuGEO_NEO_RAM_2MB->addAction(actionGEO_loeschen);
        menuEinstellungen->addAction(actionEmu64_Einstellungen);
        menuEinstellungen->addAction(actionTV_Video_Einstellungen);
        menuEinstellungen->addAction(actionVollbild);
        menuEinstellungen->addAction(actionC64_Tastaturbelegung_ndern);
        menuEinstellungen->addAction(actionC64_Tastaturbelegung_zur_cksetzen);
        menuEinstellungen->addAction(actionFenster_Optionen);
        menuDatei->addAction(actionBeenden);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        menu_main_info->setText(QApplication::translate("MainWindow", "Info", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        menu_main_info->setStatusTip(QApplication::translate("MainWindow", "Infos zur aktuellen Emu64 Version.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionFloppy_1541_II->setText(QApplication::translate("MainWindow", "Floppy 1541 II", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionFloppy_1541_II->setToolTip(QApplication::translate("MainWindow", "Floppy 1541 II", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionFloppy_1541_II->setStatusTip(QApplication::translate("MainWindow", "Hier k\303\266nnen Sie C64 Diskimages ausw\303\244hlen oder bearbeiten. [D64,G64,ZIP]", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionBandlaufwerk_1530->setText(QApplication::translate("MainWindow", "Datasette 1530", 0, QApplication::UnicodeUTF8));
        actionBandlaufwerk_1530->setIconText(QApplication::translate("MainWindow", "Datasette 1530", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionBandlaufwerk_1530->setToolTip(QApplication::translate("MainWindow", "Datasette 1530", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionBandlaufwerk_1530->setStatusTip(QApplication::translate("MainWindow", "Alle Funktionen der Datasette 1530. [TAP,WAV]", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionSteckmodule->setText(QApplication::translate("MainWindow", "Steckmodule", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionSteckmodule->setToolTip(QApplication::translate("MainWindow", "Steckmodule", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionSteckmodule->setStatusTip(QApplication::translate("MainWindow", "Hier k\303\266nnen Sie ein Steckmodul in den Emu64 einsetzen. [CRT]", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionAutostart->setText(QApplication::translate("MainWindow", "Autostart", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionAutostart->setStatusTip(QApplication::translate("MainWindow", "Die schnellste m\303\266glichkeit ein C64 Programm zu laden und zu starten. [PRG,T64,P00,FRZ,D64,G64]", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionC64_Programme_direkt_laden->setText(QApplication::translate("MainWindow", "C64 Programme direkt laden", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionC64_Programme_direkt_laden->setStatusTip(QApplication::translate("MainWindow", "Hier wird das Programm direkt in den Speicher geladen ohne C64 Laderoutiene. [PRG,T64,P00,FRZ]", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionSoftreset->setText(QApplication::translate("MainWindow", "Softreset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionSoftreset->setStatusTip(QApplication::translate("MainWindow", "F\303\274hrt ein Softreset des C64 durch. Das C64 RAM wird dabei nicht gel\303\266scht.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionHardreset->setText(QApplication::translate("MainWindow", "Hardreset", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionHardreset->setStatusTip(QApplication::translate("MainWindow", "F\303\274hrt ein Hardreset des C64 durch. Das C64 RAM wird dabei gel\303\266scht. Es gleicht dem Ein- und Auschalten des C64.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionBeenden->setText(QApplication::translate("MainWindow", "Beenden", 0, QApplication::UnicodeUTF8));
        actionBeenden->setIconText(QApplication::translate("MainWindow", "Beenden", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        actionBeenden->setToolTip(QApplication::translate("MainWindow", "Beenden", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        actionBeenden->setStatusTip(QApplication::translate("MainWindow", "Beendet den Emu64.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionQuick_C64_Snapshot->setText(QApplication::translate("MainWindow", "Quick C64 Snapshot", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionQuick_C64_Snapshot->setStatusTip(QApplication::translate("MainWindow", "Den aktuellen Zustand des C64 speichern, um zu einem sp\303\244teren Zeitpunkt wieder zu laden.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionScreenshot->setText(QApplication::translate("MainWindow", "Screenshot", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionScreenshot->setStatusTip(QApplication::translate("MainWindow", "Erzeugt ein Bildschirmfoto vom C64 Bildschirm.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionScreenshot->setShortcut(QApplication::translate("MainWindow", "F9", 0, QApplication::UnicodeUTF8));
        actionSID_Dump->setText(QApplication::translate("MainWindow", "SID Dump", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionSID_Dump->setStatusTip(QApplication::translate("MainWindow", "Zeichnen Sie alle SID (Sound Interface Device) Signale auf um sie sp\303\244ter mit dem  mitgelieferten SID Player abzuspielen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionSID_Player->setText(QApplication::translate("MainWindow", "SID Player", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionSID_Player->setStatusTip(QApplication::translate("MainWindow", "Schaltet in den SID Player Modus. Damit wird der Emu64 zu einem vollwertigen SID Player.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionDebugger_Disassembler->setText(QApplication::translate("MainWindow", "Debugger/Disassembler", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionDebugger_Disassembler->setStatusTip(QApplication::translate("MainWindow", "Sehr Komfortabler Debugger und Disassembler.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionC64_Geschwindigkeit->setText(QApplication::translate("MainWindow", "C64 Geschwindigkeit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionC64_Geschwindigkeit->setStatusTip(QApplication::translate("MainWindow", "Hier k\303\266nnen Sie die C64 Geschwindigkeit sufenlos von 5% bis 500% einstellen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionCross_Assembler->setText(QApplication::translate("MainWindow", "Cross Assembler", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionCross_Assembler->setStatusTip(QApplication::translate("MainWindow", "Integrierter 2Pass Assembler der an TurboAss angelehnt ist.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionLan64->setText(QApplication::translate("MainWindow", "Lan64 ", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionLan64->setStatusTip(QApplication::translate("MainWindow", "Hiermit kann man Programme vom PC zum C64 \303\274bertragen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionVirtuelle_C64_Tastatur->setText(QApplication::translate("MainWindow", "Virtuelle C64 Tastatur", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionVirtuelle_C64_Tastatur->setStatusTip(QApplication::translate("MainWindow", "Eine Virtuelle C64 Tastatur, diese zeigt auch alle C64 Symbole.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionVideo_Capture->setText(QApplication::translate("MainWindow", "Audio und Video Aufnahme", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionVideo_Capture->setStatusTip(QApplication::translate("MainWindow", "Mit diesem Tool k\303\266nnen sie die den Ton und das Videobild aufnehmen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionEmu64_Einstellungen->setText(QApplication::translate("MainWindow", "Emu64 Einstellungen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionEmu64_Einstellungen->setStatusTip(QApplication::translate("MainWindow", "Hier stellen sie den Emu64 nach ihren w\303\274nschen ein.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionTV_Video_Einstellungen->setText(QApplication::translate("MainWindow", "TV/Video Einstellungen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionTV_Video_Einstellungen->setStatusTip(QApplication::translate("MainWindow", "Hiermit kann man das Aussehen der Videoausgabe ver\303\244ndern. Z.B.: S\303\244ttigung, Helligkeit und Kontrast.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionVollbild->setText(QApplication::translate("MainWindow", "Vollbild", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionVollbild->setStatusTip(QApplication::translate("MainWindow", "Emu64 wird im Vollbild ausgef\303\274hrt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionC64_Tastaturbelegung_ndern->setText(QApplication::translate("MainWindow", "C64 Tastaturbelegung \303\244ndern", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionC64_Tastaturbelegung_ndern->setStatusTip(QApplication::translate("MainWindow", "Hiermit k\303\266nnen Sie die komplette Tastaturbelegung(C64) \303\244ndern.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionC64_Tastaturbelegung_zur_cksetzen->setText(QApplication::translate("MainWindow", "C64 Tastaturbelegung zur\303\274cksetzen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionC64_Tastaturbelegung_zur_cksetzen->setStatusTip(QApplication::translate("MainWindow", "Hiermit wird die Tastaturbelegung auf Standardwerte zur\303\274ck gesetzt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionFenster_Optionen->setText(QApplication::translate("MainWindow", "Fenster Optionen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionFenster_Optionen->setStatusTip(QApplication::translate("MainWindow", "Funktionen die die Emu64 Fenster beeinflussen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionHandbuch->setText(QApplication::translate("MainWindow", "Handbuch", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionHandbuch->setStatusTip(QApplication::translate("MainWindow", "\303\226ffnet das Emu64 Handbuch.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionGameBase64->setText(QApplication::translate("MainWindow", "GameBase64", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionGameBase64->setStatusTip(QApplication::translate("MainWindow", "\303\234ber dieses Tool ist es m\303\266glich auf alle Spiele die es in der GameBase64 gibt zuzugreifen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionREU_einstecken->setText(QApplication::translate("MainWindow", "REU einstecken", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionREU_einstecken->setStatusTip(QApplication::translate("MainWindow", "Die REU wird in den C64 gesteckt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionREU_entfernen->setText(QApplication::translate("MainWindow", "REU entfernen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionREU_entfernen->setStatusTip(QApplication::translate("MainWindow", "Die REU wird aus dem C64 rausgezogen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionREU_laden->setText(QApplication::translate("MainWindow", "Inhalt aus einer Datei laden", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionREU_laden->setStatusTip(QApplication::translate("MainWindow", "Der Inhalt der REU wird aus einer externen Datei geladen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionREU_speichern->setText(QApplication::translate("MainWindow", "Inhalt in eine Datei speichern", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionREU_speichern->setStatusTip(QApplication::translate("MainWindow", "Der Inhalt der REU wird in eine externen Datei gespeichert.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionREU_loeschen->setText(QApplication::translate("MainWindow", "Inhalt l\303\266schen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionREU_loeschen->setStatusTip(QApplication::translate("MainWindow", "Der Inhalt der REU wird gel\303\266scht.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionGEO_einstecken->setText(QApplication::translate("MainWindow", "GEO-RAM einstecken", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionGEO_einstecken->setStatusTip(QApplication::translate("MainWindow", "Die GEO-RAM wird in den C64 gesteckt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionGEO_entfernen->setText(QApplication::translate("MainWindow", "GEO-RAM entfernen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionGEO_entfernen->setStatusTip(QApplication::translate("MainWindow", "Die GEO-RAM wird aus dem C64 rausgezogen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionGEO_laden->setText(QApplication::translate("MainWindow", "Inhalt aus einer Datei laden", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionGEO_laden->setStatusTip(QApplication::translate("MainWindow", "Der Inhalt der GEO-RAM wird aus einer externen Datei geladen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionGEO_speichern->setText(QApplication::translate("MainWindow", "Inhalt in eine Datei speichern", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionGEO_speichern->setStatusTip(QApplication::translate("MainWindow", "Der Inhalt der GEO-RAM wird in eine externen Datei gespeichert.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionGEO_loeschen->setText(QApplication::translate("MainWindow", "Inhalt l\303\266schen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        actionGEO_loeschen->setStatusTip(QApplication::translate("MainWindow", "Der Inhalt der GEO-RAM wird gel\303\266scht.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        actionC64_Tastenbelegung_Show->setText(QApplication::translate("MainWindow", "C64 Tastenbelegung anzeigen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        menuBar->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_WHATSTHIS
        menuBar->setWhatsThis(QString());
#endif // QT_NO_WHATSTHIS
        menu_main_help->setTitle(QApplication::translate("MainWindow", "Hilfe", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        menuC64_Dateien->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        menuC64_Dateien->setTitle(QApplication::translate("MainWindow", "C64 Dateien", 0, QApplication::UnicodeUTF8));
        menuReset->setTitle(QApplication::translate("MainWindow", "Reset", 0, QApplication::UnicodeUTF8));
        menuTools->setTitle(QApplication::translate("MainWindow", "Tools", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        menuREU_1764_16MB->setStatusTip(QApplication::translate("MainWindow", "Emuliert eine komplette REU mit 16 MB", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        menuREU_1764_16MB->setTitle(QApplication::translate("MainWindow", "REU 1764 (16MB)", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_STATUSTIP
        menuGEO_NEO_RAM_2MB->setStatusTip(QApplication::translate("MainWindow", "Emuliert eine komplette GEO- bzw NEO-RAM mit 2 MB", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        menuGEO_NEO_RAM_2MB->setTitle(QApplication::translate("MainWindow", "GEO- / NEO-RAM (2MB)", 0, QApplication::UnicodeUTF8));
        menuEinstellungen->setTitle(QApplication::translate("MainWindow", "Einstellungen", 0, QApplication::UnicodeUTF8));
        menuSprache->setTitle(QApplication::translate("MainWindow", "Sprache", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        menuDatei->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        menuDatei->setTitle(QApplication::translate("MainWindow", "Datei", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MainWindow);
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAIN_WINDOW_H
