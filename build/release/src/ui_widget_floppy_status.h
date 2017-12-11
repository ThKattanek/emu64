/********************************************************************************
** Form generated from reading UI file 'widget_floppy_status.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_FLOPPY_STATUS_H
#define UI_WIDGET_FLOPPY_STATUS_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WidgetFloppyStatus
{
public:
    QPushButton *InstertImage;
    QPushButton *PowerLED;
    QLabel *GeraeteIDOut;
    QPushButton *MotorLED;
    QPushButton *RWLED;
    QLabel *TrackOut;
    QLabel *SekcorOut;
    QLabel *Label1;
    QLabel *DisknameOut;
    QPushButton *EjectImage;
    QPushButton *Volume;

    void setupUi(QWidget *WidgetFloppyStatus)
    {
        if (WidgetFloppyStatus->objectName().isEmpty())
            WidgetFloppyStatus->setObjectName(QString::fromUtf8("WidgetFloppyStatus"));
        WidgetFloppyStatus->resize(395, 23);
        WidgetFloppyStatus->setWindowTitle(QString::fromUtf8(""));
        InstertImage = new QPushButton(WidgetFloppyStatus);
        InstertImage->setObjectName(QString::fromUtf8("InstertImage"));
        InstertImage->setEnabled(true);
        InstertImage->setGeometry(QRect(50, 1, 22, 22));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/grafik/disk.png"), QSize(), QIcon::Normal, QIcon::Off);
        InstertImage->setIcon(icon);
        InstertImage->setIconSize(QSize(18, 18));
        InstertImage->setFlat(true);
        PowerLED = new QPushButton(WidgetFloppyStatus);
        PowerLED->setObjectName(QString::fromUtf8("PowerLED"));
        PowerLED->setGeometry(QRect(0, 1, 24, 24));
        PowerLED->setAutoFillBackground(false);
        PowerLED->setText(QString::fromUtf8(""));
        PowerLED->setIconSize(QSize(14, 14));
        PowerLED->setFlat(true);
        GeraeteIDOut = new QLabel(WidgetFloppyStatus);
        GeraeteIDOut->setObjectName(QString::fromUtf8("GeraeteIDOut"));
        GeraeteIDOut->setGeometry(QRect(25, 4, 21, 16));
        QFont font;
        font.setPointSize(12);
        GeraeteIDOut->setFont(font);
        GeraeteIDOut->setText(QString::fromUtf8(""));
        MotorLED = new QPushButton(WidgetFloppyStatus);
        MotorLED->setObjectName(QString::fromUtf8("MotorLED"));
        MotorLED->setEnabled(true);
        MotorLED->setGeometry(QRect(100, 1, 9, 9));
        MotorLED->setText(QString::fromUtf8(""));
        MotorLED->setIconSize(QSize(9, 9));
        MotorLED->setFlat(true);
        RWLED = new QPushButton(WidgetFloppyStatus);
        RWLED->setObjectName(QString::fromUtf8("RWLED"));
        RWLED->setEnabled(true);
        RWLED->setGeometry(QRect(100, 11, 9, 9));
        RWLED->setText(QString::fromUtf8(""));
        RWLED->setIconSize(QSize(9, 9));
        RWLED->setFlat(true);
        TrackOut = new QLabel(WidgetFloppyStatus);
        TrackOut->setObjectName(QString::fromUtf8("TrackOut"));
        TrackOut->setEnabled(true);
        TrackOut->setGeometry(QRect(115, 3, 21, 16));
        TrackOut->setFont(font);
        TrackOut->setText(QString::fromUtf8("01"));
        SekcorOut = new QLabel(WidgetFloppyStatus);
        SekcorOut->setObjectName(QString::fromUtf8("SekcorOut"));
        SekcorOut->setEnabled(true);
        SekcorOut->setGeometry(QRect(143, 3, 21, 16));
        SekcorOut->setFont(font);
        SekcorOut->setText(QString::fromUtf8("00"));
        Label1 = new QLabel(WidgetFloppyStatus);
        Label1->setObjectName(QString::fromUtf8("Label1"));
        Label1->setEnabled(true);
        Label1->setGeometry(QRect(135, 3, 16, 16));
        Label1->setFont(font);
        Label1->setText(QString::fromUtf8("-"));
        DisknameOut = new QLabel(WidgetFloppyStatus);
        DisknameOut->setObjectName(QString::fromUtf8("DisknameOut"));
        DisknameOut->setGeometry(QRect(170, 2, 196, 20));
#ifndef QT_NO_TOOLTIP
        DisknameOut->setToolTip(QString::fromUtf8(""));
#endif // QT_NO_TOOLTIP
        DisknameOut->setStyleSheet(QString::fromUtf8("background-color: rgb(60,60, 180);\n"
"color: rgb(150, 150, 255);\n"
"border: 1px solid;\n"
"border-top-color: rgb(200, 200, 200);\n"
"border-left-color: rgb(200, 200, 200);\n"
"border-bottom-color: rgb(50, 50,50);\n"
"border-right-color: rgb(50, 50, 50);"));
        DisknameOut->setText(QString::fromUtf8(""));
        DisknameOut->setTextFormat(Qt::PlainText);
        DisknameOut->setScaledContents(false);
        DisknameOut->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);
        EjectImage = new QPushButton(WidgetFloppyStatus);
        EjectImage->setObjectName(QString::fromUtf8("EjectImage"));
        EjectImage->setEnabled(true);
        EjectImage->setGeometry(QRect(70, 1, 22, 22));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/grafik/eject1.png"), QSize(), QIcon::Normal, QIcon::Off);
        EjectImage->setIcon(icon1);
        EjectImage->setIconSize(QSize(19, 19));
        EjectImage->setFlat(true);
        Volume = new QPushButton(WidgetFloppyStatus);
        Volume->setObjectName(QString::fromUtf8("Volume"));
        Volume->setGeometry(QRect(370, 1, 24, 24));
        Volume->setAutoFillBackground(false);
        Volume->setText(QString::fromUtf8(""));
        Volume->setIconSize(QSize(16, 16));
        Volume->setFlat(true);

        retranslateUi(WidgetFloppyStatus);

        QMetaObject::connectSlotsByName(WidgetFloppyStatus);
    } // setupUi

    void retranslateUi(QWidget *WidgetFloppyStatus)
    {
#ifndef QT_NO_STATUSTIP
        InstertImage->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Ein Diskimage ausw\303\244hlen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        InstertImage->setText(QString());
#ifndef QT_NO_STATUSTIP
        PowerLED->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Floppylaufwerk Ein-/Ausschalten", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        GeraeteIDOut->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Floppy Ger\303\244teadresse", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        MotorLED->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Anzeige f\303\274r den Status des Diskmotors.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        RWLED->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Anzeige f\303\274r den Schreib- /Lesezugriff.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        TrackOut->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Aktuelle Spur.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        SekcorOut->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Aktueller Sektor.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        DisknameOut->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Diskname der eingelegten Diskette.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
#ifndef QT_NO_STATUSTIP
        EjectImage->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Diskimage auswerfen.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        EjectImage->setText(QString());
#ifndef QT_NO_STATUSTIP
        Volume->setStatusTip(QApplication::translate("WidgetFloppyStatus", "Lautst\303\244rke der Floppyger\303\244usche.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_STATUSTIP
        Q_UNUSED(WidgetFloppyStatus);
    } // retranslateUi

};

namespace Ui {
    class WidgetFloppyStatus: public Ui_WidgetFloppyStatus {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_FLOPPY_STATUS_H
