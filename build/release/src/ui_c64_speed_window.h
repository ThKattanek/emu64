/********************************************************************************
** Form generated from reading UI file 'c64_speed_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C64_SPEED_WINDOW_H
#define UI_C64_SPEED_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_C64SpeedWindow
{
public:
    QSpinBox *C64Speed;
    QPushButton *Set5;
    QPushButton *Set25;
    QPushButton *Set50;
    QPushButton *Set100;
    QPushButton *Set200;
    QPushButton *Set300;
    QPushButton *Set400;
    QPushButton *Set500;
    QPushButton *Pause;
    QScrollBar *ScrollC64Speed;
    QCheckBox *WarpMode;

    void setupUi(QDialog *C64SpeedWindow)
    {
        if (C64SpeedWindow->objectName().isEmpty())
            C64SpeedWindow->setObjectName(QString::fromUtf8("C64SpeedWindow"));
        C64SpeedWindow->resize(230, 105);
        C64SpeedWindow->setMinimumSize(QSize(0, 0));
        C64SpeedWindow->setMaximumSize(QSize(1000, 1000));
        C64Speed = new QSpinBox(C64SpeedWindow);
        C64Speed->setObjectName(QString::fromUtf8("C64Speed"));
        C64Speed->setGeometry(QRect(4, 4, 93, 24));
        C64Speed->setWrapping(false);
        C64Speed->setButtonSymbols(QAbstractSpinBox::PlusMinus);
        C64Speed->setAccelerated(false);
        C64Speed->setMinimum(5);
        C64Speed->setMaximum(5000);
        Set5 = new QPushButton(C64SpeedWindow);
        Set5->setObjectName(QString::fromUtf8("Set5"));
        Set5->setGeometry(QRect(4, 52, 53, 25));
        Set25 = new QPushButton(C64SpeedWindow);
        Set25->setObjectName(QString::fromUtf8("Set25"));
        Set25->setGeometry(QRect(60, 52, 53, 25));
        Set50 = new QPushButton(C64SpeedWindow);
        Set50->setObjectName(QString::fromUtf8("Set50"));
        Set50->setGeometry(QRect(116, 52, 53, 25));
        Set100 = new QPushButton(C64SpeedWindow);
        Set100->setObjectName(QString::fromUtf8("Set100"));
        Set100->setGeometry(QRect(172, 52, 53, 25));
        Set200 = new QPushButton(C64SpeedWindow);
        Set200->setObjectName(QString::fromUtf8("Set200"));
        Set200->setGeometry(QRect(4, 76, 53, 25));
        Set300 = new QPushButton(C64SpeedWindow);
        Set300->setObjectName(QString::fromUtf8("Set300"));
        Set300->setGeometry(QRect(60, 76, 53, 25));
        Set400 = new QPushButton(C64SpeedWindow);
        Set400->setObjectName(QString::fromUtf8("Set400"));
        Set400->setGeometry(QRect(116, 76, 53, 25));
        Set500 = new QPushButton(C64SpeedWindow);
        Set500->setObjectName(QString::fromUtf8("Set500"));
        Set500->setGeometry(QRect(172, 76, 53, 25));
        Pause = new QPushButton(C64SpeedWindow);
        Pause->setObjectName(QString::fromUtf8("Pause"));
        Pause->setGeometry(QRect(100, 4, 73, 25));
        ScrollC64Speed = new QScrollBar(C64SpeedWindow);
        ScrollC64Speed->setObjectName(QString::fromUtf8("ScrollC64Speed"));
        ScrollC64Speed->setGeometry(QRect(4, 32, 221, 14));
        ScrollC64Speed->setMinimum(5);
        ScrollC64Speed->setMaximum(500);
        ScrollC64Speed->setOrientation(Qt::Horizontal);
        WarpMode = new QCheckBox(C64SpeedWindow);
        WarpMode->setObjectName(QString::fromUtf8("WarpMode"));
        WarpMode->setGeometry(QRect(180, 6, 51, 20));

        retranslateUi(C64SpeedWindow);

        QMetaObject::connectSlotsByName(C64SpeedWindow);
    } // setupUi

    void retranslateUi(QDialog *C64SpeedWindow)
    {
        C64SpeedWindow->setWindowTitle(QApplication::translate("C64SpeedWindow", "C64 Geschwindigkeit", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        C64Speed->setToolTip(QApplication::translate("C64SpeedWindow", "Hier kannst du die C64 Geschwindigkeit manuell eingeben.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        C64Speed->setSuffix(QApplication::translate("C64SpeedWindow", "%", 0, QApplication::UnicodeUTF8));
        C64Speed->setPrefix(QString());
#ifndef QT_NO_TOOLTIP
        Set5->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 5% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set5->setText(QApplication::translate("C64SpeedWindow", "5%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Set25->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 25% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set25->setText(QApplication::translate("C64SpeedWindow", "25%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Set50->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 50% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set50->setText(QApplication::translate("C64SpeedWindow", "50%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Set100->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 100% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set100->setText(QApplication::translate("C64SpeedWindow", "100%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Set200->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 200% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set200->setText(QApplication::translate("C64SpeedWindow", "200%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Set300->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 300% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set300->setText(QApplication::translate("C64SpeedWindow", "300%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Set400->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 400% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set400->setText(QApplication::translate("C64SpeedWindow", "400%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Set500->setToolTip(QApplication::translate("C64SpeedWindow", "Die C64 Geschwindigkeit wird auf 500% gestellt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Set500->setText(QApplication::translate("C64SpeedWindow", "500%", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        Pause->setToolTip(QString());
#endif // QT_NO_TOOLTIP
        Pause->setText(QApplication::translate("C64SpeedWindow", "Pause", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        ScrollC64Speed->setToolTip(QApplication::translate("C64SpeedWindow", "Stufenlos einstellbare Geschwindigkeit.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        WarpMode->setText(QApplication::translate("C64SpeedWindow", "Warp", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class C64SpeedWindow: public Ui_C64SpeedWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C64_SPEED_WINDOW_H
