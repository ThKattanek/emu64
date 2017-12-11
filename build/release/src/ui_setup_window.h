/********************************************************************************
** Form generated from reading UI file 'setup_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SETUP_WINDOW_H
#define UI_SETUP_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDial>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpinBox>
#include <QtGui/QTabWidget>
#include <QtGui/QTableWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_SetupWindow
{
public:
    QTabWidget *SetupTab;
    QWidget *EmualtionTab;
    QGroupBox *FullscreenGroup_2;
    QComboBox *FirstSidTyp;
    QComboBox *SecondSidTyp;
    QComboBox *SecondSidAddress;
    QLabel *sid1_label;
    QCheckBox *SecondSidEnable;
    QLabel *sid_io_label;
    QCheckBox *SidCycleExactEnable;
    QCheckBox *SidFilterEnable;
    QCheckBox *Sid6ChannelMode;
    QGroupBox *groupBox_3;
    QComboBox *SelectRomSet;
    QPushButton *NewRomSet;
    QPushButton *DeleteRomSet;
    QWidget *SoundTab;
    QDial *SIDVolume;
    QLabel *label_4;
    QLabel *SIDVolumeOut;
    QWidget *GrafikTab;
    QLabel *label_Farbmodus;
    QComboBox *C64Farbmodus;
    QGroupBox *WindowGroup;
    QCheckBox *WPal;
    QCheckBox *WDouble;
    QCheckBox *WVsync;
    QCheckBox *WFilter;
    QRadioButton *W16Bit;
    QRadioButton *W32Bit;
    QCheckBox *WAspectRatio;
    QCheckBox *AutoMouseHide;
    QSpinBox *AutoMouseHideTime;
    QLabel *label_5;
    QGroupBox *FullscreenGroup;
    QCheckBox *FAspectRatio;
    QGroupBox *screenshot_group;
    QLabel *label;
    QComboBox *ScreenshotFormat;
    QPushButton *ResetSShotCounter;
    QWidget *SteuerungTab;
    QGroupBox *groupBox;
    QTableWidget *VJoySlots;
    QGroupBox *groupBox_2;
    QLabel *label_2;
    QComboBox *MausPort;
    QLabel *label_3;
    QWidget *Lan64Tab;
    QWidget *GB64Tab;

    void setupUi(QDialog *SetupWindow)
    {
        if (SetupWindow->objectName().isEmpty())
            SetupWindow->setObjectName(QString::fromUtf8("SetupWindow"));
        SetupWindow->setEnabled(true);
        SetupWindow->resize(411, 361);
        SetupTab = new QTabWidget(SetupWindow);
        SetupTab->setObjectName(QString::fromUtf8("SetupTab"));
        SetupTab->setEnabled(true);
        SetupTab->setGeometry(QRect(5, 5, 401, 351));
        QFont font;
        font.setPointSize(8);
        SetupTab->setFont(font);
        SetupTab->setDocumentMode(false);
        EmualtionTab = new QWidget();
        EmualtionTab->setObjectName(QString::fromUtf8("EmualtionTab"));
        EmualtionTab->setEnabled(true);
        FullscreenGroup_2 = new QGroupBox(EmualtionTab);
        FullscreenGroup_2->setObjectName(QString::fromUtf8("FullscreenGroup_2"));
        FullscreenGroup_2->setEnabled(true);
        FullscreenGroup_2->setGeometry(QRect(10, 10, 381, 121));
        QFont font1;
        font1.setPointSize(9);
        FullscreenGroup_2->setFont(font1);
        FirstSidTyp = new QComboBox(FullscreenGroup_2);
        FirstSidTyp->setObjectName(QString::fromUtf8("FirstSidTyp"));
        FirstSidTyp->setGeometry(QRect(10, 51, 81, 21));
        SecondSidTyp = new QComboBox(FullscreenGroup_2);
        SecondSidTyp->setObjectName(QString::fromUtf8("SecondSidTyp"));
        SecondSidTyp->setGeometry(QRect(130, 50, 81, 21));
        SecondSidAddress = new QComboBox(FullscreenGroup_2);
        SecondSidAddress->setObjectName(QString::fromUtf8("SecondSidAddress"));
        SecondSidAddress->setGeometry(QRect(240, 50, 69, 21));
        sid1_label = new QLabel(FullscreenGroup_2);
        sid1_label->setObjectName(QString::fromUtf8("sid1_label"));
        sid1_label->setGeometry(QRect(10, 25, 81, 21));
        SecondSidEnable = new QCheckBox(FullscreenGroup_2);
        SecondSidEnable->setObjectName(QString::fromUtf8("SecondSidEnable"));
        SecondSidEnable->setGeometry(QRect(130, 22, 81, 20));
        sid_io_label = new QLabel(FullscreenGroup_2);
        sid_io_label->setObjectName(QString::fromUtf8("sid_io_label"));
        sid_io_label->setGeometry(QRect(240, 25, 61, 16));
        SidCycleExactEnable = new QCheckBox(FullscreenGroup_2);
        SidCycleExactEnable->setObjectName(QString::fromUtf8("SidCycleExactEnable"));
        SidCycleExactEnable->setGeometry(QRect(10, 77, 151, 20));
        SidFilterEnable = new QCheckBox(FullscreenGroup_2);
        SidFilterEnable->setObjectName(QString::fromUtf8("SidFilterEnable"));
        SidFilterEnable->setGeometry(QRect(10, 97, 151, 20));
        Sid6ChannelMode = new QCheckBox(FullscreenGroup_2);
        Sid6ChannelMode->setObjectName(QString::fromUtf8("Sid6ChannelMode"));
        Sid6ChannelMode->setGeometry(QRect(170, 80, 101, 16));
        groupBox_3 = new QGroupBox(EmualtionTab);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(10, 140, 381, 71));
        groupBox_3->setFont(font1);
        SelectRomSet = new QComboBox(groupBox_3);
        SelectRomSet->setObjectName(QString::fromUtf8("SelectRomSet"));
        SelectRomSet->setGeometry(QRect(10, 30, 221, 21));
        NewRomSet = new QPushButton(groupBox_3);
        NewRomSet->setObjectName(QString::fromUtf8("NewRomSet"));
        NewRomSet->setGeometry(QRect(240, 30, 51, 21));
        DeleteRomSet = new QPushButton(groupBox_3);
        DeleteRomSet->setObjectName(QString::fromUtf8("DeleteRomSet"));
        DeleteRomSet->setGeometry(QRect(300, 30, 61, 21));
        SetupTab->addTab(EmualtionTab, QString());
        SoundTab = new QWidget();
        SoundTab->setObjectName(QString::fromUtf8("SoundTab"));
        SoundTab->setEnabled(true);
        SIDVolume = new QDial(SoundTab);
        SIDVolume->setObjectName(QString::fromUtf8("SIDVolume"));
        SIDVolume->setGeometry(QRect(10, 29, 51, 51));
        SIDVolume->setMaximum(100);
        SIDVolume->setValue(100);
        SIDVolume->setInvertedAppearance(false);
        SIDVolume->setInvertedControls(false);
        SIDVolume->setWrapping(false);
        SIDVolume->setNotchTarget(3.6);
        SIDVolume->setNotchesVisible(true);
        label_4 = new QLabel(SoundTab);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(0, 9, 71, 20));
        label_4->setAlignment(Qt::AlignCenter);
        SIDVolumeOut = new QLabel(SoundTab);
        SIDVolumeOut->setObjectName(QString::fromUtf8("SIDVolumeOut"));
        SIDVolumeOut->setGeometry(QRect(10, 80, 51, 21));
        QFont font2;
        font2.setPointSize(10);
        font2.setBold(true);
        font2.setWeight(75);
        SIDVolumeOut->setFont(font2);
        SIDVolumeOut->setAlignment(Qt::AlignCenter);
        SetupTab->addTab(SoundTab, QString());
        GrafikTab = new QWidget();
        GrafikTab->setObjectName(QString::fromUtf8("GrafikTab"));
        GrafikTab->setEnabled(true);
        label_Farbmodus = new QLabel(GrafikTab);
        label_Farbmodus->setObjectName(QString::fromUtf8("label_Farbmodus"));
        label_Farbmodus->setGeometry(QRect(10, 6, 101, 20));
        label_Farbmodus->setFont(font1);
        C64Farbmodus = new QComboBox(GrafikTab);
        C64Farbmodus->setObjectName(QString::fromUtf8("C64Farbmodus"));
        C64Farbmodus->setGeometry(QRect(110, 10, 161, 21));
        C64Farbmodus->setFont(font1);
        WindowGroup = new QGroupBox(GrafikTab);
        WindowGroup->setObjectName(QString::fromUtf8("WindowGroup"));
        WindowGroup->setGeometry(QRect(10, 30, 381, 131));
        WindowGroup->setFont(font1);
        WPal = new QCheckBox(WindowGroup);
        WPal->setObjectName(QString::fromUtf8("WPal"));
        WPal->setGeometry(QRect(10, 20, 177, 21));
        WDouble = new QCheckBox(WindowGroup);
        WDouble->setObjectName(QString::fromUtf8("WDouble"));
        WDouble->setGeometry(QRect(10, 40, 177, 21));
        WVsync = new QCheckBox(WindowGroup);
        WVsync->setObjectName(QString::fromUtf8("WVsync"));
        WVsync->setEnabled(false);
        WVsync->setGeometry(QRect(10, 60, 106, 21));
        WFilter = new QCheckBox(WindowGroup);
        WFilter->setObjectName(QString::fromUtf8("WFilter"));
        WFilter->setEnabled(true);
        WFilter->setGeometry(QRect(310, 60, 61, 21));
        W16Bit = new QRadioButton(WindowGroup);
        W16Bit->setObjectName(QString::fromUtf8("W16Bit"));
        W16Bit->setEnabled(false);
        W16Bit->setGeometry(QRect(310, 15, 61, 21));
        W16Bit->setChecked(false);
        W32Bit = new QRadioButton(WindowGroup);
        W32Bit->setObjectName(QString::fromUtf8("W32Bit"));
        W32Bit->setEnabled(true);
        W32Bit->setGeometry(QRect(310, 35, 61, 21));
        W32Bit->setChecked(true);
        WAspectRatio = new QCheckBox(WindowGroup);
        WAspectRatio->setObjectName(QString::fromUtf8("WAspectRatio"));
        WAspectRatio->setGeometry(QRect(10, 80, 177, 21));
        AutoMouseHide = new QCheckBox(WindowGroup);
        AutoMouseHide->setObjectName(QString::fromUtf8("AutoMouseHide"));
        AutoMouseHide->setGeometry(QRect(10, 100, 141, 21));
        AutoMouseHideTime = new QSpinBox(WindowGroup);
        AutoMouseHideTime->setObjectName(QString::fromUtf8("AutoMouseHideTime"));
        AutoMouseHideTime->setEnabled(true);
        AutoMouseHideTime->setGeometry(QRect(160, 100, 42, 22));
        AutoMouseHideTime->setMinimum(1);
        AutoMouseHideTime->setMaximum(90);
        label_5 = new QLabel(WindowGroup);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(210, 100, 31, 20));
        FullscreenGroup = new QGroupBox(GrafikTab);
        FullscreenGroup->setObjectName(QString::fromUtf8("FullscreenGroup"));
        FullscreenGroup->setEnabled(true);
        FullscreenGroup->setGeometry(QRect(10, 165, 381, 101));
        FullscreenGroup->setFont(font1);
        FAspectRatio = new QCheckBox(FullscreenGroup);
        FAspectRatio->setObjectName(QString::fromUtf8("FAspectRatio"));
        FAspectRatio->setGeometry(QRect(10, 20, 177, 21));
        screenshot_group = new QGroupBox(GrafikTab);
        screenshot_group->setObjectName(QString::fromUtf8("screenshot_group"));
        screenshot_group->setEnabled(true);
        screenshot_group->setGeometry(QRect(10, 270, 381, 51));
        screenshot_group->setFont(font1);
        label = new QLabel(screenshot_group);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 25, 41, 16));
        label->setFont(font1);
        ScreenshotFormat = new QComboBox(screenshot_group);
        ScreenshotFormat->setObjectName(QString::fromUtf8("ScreenshotFormat"));
        ScreenshotFormat->setEnabled(true);
        ScreenshotFormat->setGeometry(QRect(55, 25, 61, 21));
        ScreenshotFormat->setFont(font1);
        ResetSShotCounter = new QPushButton(screenshot_group);
        ResetSShotCounter->setObjectName(QString::fromUtf8("ResetSShotCounter"));
        ResetSShotCounter->setGeometry(QRect(125, 25, 161, 21));
        ResetSShotCounter->setFont(font1);
        SetupTab->addTab(GrafikTab, QString());
        SteuerungTab = new QWidget();
        SteuerungTab->setObjectName(QString::fromUtf8("SteuerungTab"));
        SteuerungTab->setEnabled(true);
        SteuerungTab->setFont(font1);
        groupBox = new QGroupBox(SteuerungTab);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(8, 8, 385, 241));
        VJoySlots = new QTableWidget(groupBox);
        VJoySlots->setObjectName(QString::fromUtf8("VJoySlots"));
        VJoySlots->setGeometry(QRect(4, 16, 377, 221));
        VJoySlots->setFont(font);
        VJoySlots->setSelectionMode(QAbstractItemView::NoSelection);
        VJoySlots->setSelectionBehavior(QAbstractItemView::SelectItems);
        VJoySlots->setShowGrid(true);
        VJoySlots->setGridStyle(Qt::SolidLine);
        VJoySlots->horizontalHeader()->setCascadingSectionResizes(false);
        VJoySlots->horizontalHeader()->setHighlightSections(false);
        VJoySlots->verticalHeader()->setHighlightSections(false);
        groupBox_2 = new QGroupBox(SteuerungTab);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setGeometry(QRect(10, 250, 381, 71));
        label_2 = new QLabel(groupBox_2);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 20, 21, 21));
        label_2->setFont(font1);
        MausPort = new QComboBox(groupBox_2);
        MausPort->setObjectName(QString::fromUtf8("MausPort"));
        MausPort->setEnabled(true);
        MausPort->setGeometry(QRect(40, 20, 41, 21));
        MausPort->setFont(font1);
        label_3 = new QLabel(groupBox_2);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(10, 40, 361, 31));
        QFont font3;
        font3.setBold(true);
        font3.setWeight(75);
        label_3->setFont(font3);
        label_3->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        SetupTab->addTab(SteuerungTab, QString());
        Lan64Tab = new QWidget();
        Lan64Tab->setObjectName(QString::fromUtf8("Lan64Tab"));
        Lan64Tab->setEnabled(false);
        SetupTab->addTab(Lan64Tab, QString());
        GB64Tab = new QWidget();
        GB64Tab->setObjectName(QString::fromUtf8("GB64Tab"));
        GB64Tab->setEnabled(false);
        SetupTab->addTab(GB64Tab, QString());

        retranslateUi(SetupWindow);

        SetupTab->setCurrentIndex(2);


        QMetaObject::connectSlotsByName(SetupWindow);
    } // setupUi

    void retranslateUi(QDialog *SetupWindow)
    {
        SetupWindow->setWindowTitle(QApplication::translate("SetupWindow", "Emu64 Einstellungen", 0, QApplication::UnicodeUTF8));
        FullscreenGroup_2->setTitle(QApplication::translate("SetupWindow", "SID", 0, QApplication::UnicodeUTF8));
        sid1_label->setText(QApplication::translate("SetupWindow", "1. SID", 0, QApplication::UnicodeUTF8));
        SecondSidEnable->setText(QApplication::translate("SetupWindow", "2. SID", 0, QApplication::UnicodeUTF8));
        sid_io_label->setText(QApplication::translate("SetupWindow", "IO Bereich", 0, QApplication::UnicodeUTF8));
        SidCycleExactEnable->setText(QApplication::translate("SetupWindow", "Zyklengenaue Emulation", 0, QApplication::UnicodeUTF8));
        SidFilterEnable->setText(QApplication::translate("SetupWindow", "Filter Emulation", 0, QApplication::UnicodeUTF8));
        Sid6ChannelMode->setText(QApplication::translate("SetupWindow", "6 Kanal Modus", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("SetupWindow", "ROM Set's", 0, QApplication::UnicodeUTF8));
        NewRomSet->setText(QApplication::translate("SetupWindow", "Neu", 0, QApplication::UnicodeUTF8));
        DeleteRomSet->setText(QApplication::translate("SetupWindow", "L\303\266schen", 0, QApplication::UnicodeUTF8));
        SetupTab->setTabText(SetupTab->indexOf(EmualtionTab), QApplication::translate("SetupWindow", "Emulation", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("SetupWindow", "SID Volume", 0, QApplication::UnicodeUTF8));
        SIDVolumeOut->setText(QApplication::translate("SetupWindow", "100%", 0, QApplication::UnicodeUTF8));
        SetupTab->setTabText(SetupTab->indexOf(SoundTab), QApplication::translate("SetupWindow", "Sound", 0, QApplication::UnicodeUTF8));
        label_Farbmodus->setText(QApplication::translate("SetupWindow", "C64 Farbmodus:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        C64Farbmodus->setToolTip(QApplication::translate("SetupWindow", "Setzt das C64 Farbschema wenn kein PAL ausgew\303\244hlt wurde.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        WindowGroup->setTitle(QApplication::translate("SetupWindow", "Fenstermodus", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        WPal->setToolTip(QApplication::translate("SetupWindow", "Schaltet die PAL Emulation der Grafikausgabe ein. (Sollte vorrangig benutzt werden)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        WPal->setText(QApplication::translate("SetupWindow", "PAL Videoemulation", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        WDouble->setToolTip(QApplication::translate("SetupWindow", "Verdoppelt die Textur in der Breite und in der H\303\266he. (Sch\303\244rferes Bild beim Skalieren)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        WDouble->setText(QApplication::translate("SetupWindow", "Texturgr\303\266\303\237e verdoppeln", 0, QApplication::UnicodeUTF8));
        WVsync->setText(QApplication::translate("SetupWindow", "Vertikal Sync", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        WFilter->setToolTip(QApplication::translate("SetupWindow", "Schaltet einen Bilineare Filterung mit zu. (Das Bild wird etwas verwaschener)", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        WFilter->setText(QApplication::translate("SetupWindow", "Filter", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        W16Bit->setToolTip(QApplication::translate("SetupWindow", "Schaltet den internen Farbmodus auf 16Bit pro Pixel um.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        W16Bit->setText(QApplication::translate("SetupWindow", "16 Bit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        W32Bit->setToolTip(QApplication::translate("SetupWindow", "Schaltet den internen Farbmodus auf 32Bit pro Pixel um.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        W32Bit->setText(QApplication::translate("SetupWindow", "32 Bit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        WAspectRatio->setToolTip(QApplication::translate("SetupWindow", "Erzwingt ein Bildschirm Seitenverh\303\244ltnis von 4:3", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        WAspectRatio->setText(QApplication::translate("SetupWindow", "4:3 Seitenverh\303\244ltnis", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        AutoMouseHide->setToolTip(QApplication::translate("SetupWindow", "Blendet den Mauszeiger nach einer einstellbaren Zeit automatisch aus.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        AutoMouseHide->setText(QApplication::translate("SetupWindow", "Mauszeiger ausblenden", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        AutoMouseHideTime->setToolTip(QApplication::translate("SetupWindow", "Zeit in Sekunden.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_5->setText(QApplication::translate("SetupWindow", "Sek.", 0, QApplication::UnicodeUTF8));
        FullscreenGroup->setTitle(QApplication::translate("SetupWindow", "Vollbildmodus", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        FAspectRatio->setToolTip(QApplication::translate("SetupWindow", "Erzwingt ein Bildschirm Seitenverh\303\244ltnis von 4:3", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        FAspectRatio->setText(QApplication::translate("SetupWindow", "4:3 Seitenverh\303\244ltnis", 0, QApplication::UnicodeUTF8));
        screenshot_group->setTitle(QApplication::translate("SetupWindow", "Bildschirmfotos vom C64", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("SetupWindow", "Format", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ScreenshotFormat->setToolTip(QApplication::translate("SetupWindow", "Hier kann man das Screenshot Format einstellen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        ResetSShotCounter->setToolTip(QApplication::translate("SetupWindow", "Setzt den Z\303\244hler f\303\274r die angeh\303\244ngte Zahl an der Screenshot Datei zur\303\274ck.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        ResetSShotCounter->setText(QApplication::translate("SetupWindow", "Counter Zur\303\274cksetzen", 0, QApplication::UnicodeUTF8));
        SetupTab->setTabText(SetupTab->indexOf(GrafikTab), QApplication::translate("SetupWindow", "Grafik", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("SetupWindow", "Virtuelle - Joystick's", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("SetupWindow", "Maus 1351", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("SetupWindow", "Port", 0, QApplication::UnicodeUTF8));
        MausPort->clear();
        MausPort->insertItems(0, QStringList()
         << QApplication::translate("SetupWindow", "1", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("SetupWindow", "2", 0, QApplication::UnicodeUTF8)
        );
#ifndef QT_NO_TOOLTIP
        MausPort->setToolTip(QApplication::translate("SetupWindow", "Hier kann man das Screenshot Format einstellen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        label_3->setText(QApplication::translate("SetupWindow", "Aktivieren der Maus im C64 Bildschirm mit Strg+Li. Maustaste", 0, QApplication::UnicodeUTF8));
        SetupTab->setTabText(SetupTab->indexOf(SteuerungTab), QApplication::translate("SetupWindow", "Steuerung", 0, QApplication::UnicodeUTF8));
        SetupTab->setTabText(SetupTab->indexOf(Lan64Tab), QApplication::translate("SetupWindow", "Lan64", 0, QApplication::UnicodeUTF8));
        SetupTab->setTabText(SetupTab->indexOf(GB64Tab), QApplication::translate("SetupWindow", "GameBase64", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class SetupWindow: public Ui_SetupWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SETUP_WINDOW_H
