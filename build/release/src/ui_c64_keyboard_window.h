/********************************************************************************
** Form generated from reading UI file 'c64_keyboard_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_C64_KEYBOARD_WINDOW_H
#define UI_C64_KEYBOARD_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>

QT_BEGIN_NAMESPACE

class Ui_C64KeyboardWindow
{
public:

    void setupUi(QDialog *C64KeyboardWindow)
    {
        if (C64KeyboardWindow->objectName().isEmpty())
            C64KeyboardWindow->setObjectName(QString::fromUtf8("C64KeyboardWindow"));
        C64KeyboardWindow->resize(660, 195);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(2);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(C64KeyboardWindow->sizePolicy().hasHeightForWidth());
        C64KeyboardWindow->setSizePolicy(sizePolicy);
        C64KeyboardWindow->setMinimumSize(QSize(572, 169));
        C64KeyboardWindow->setMaximumSize(QSize(5000, 5000));
        C64KeyboardWindow->setStyleSheet(QString::fromUtf8("border-image: url(:/graphics/c64_tastatur.png);"));

        retranslateUi(C64KeyboardWindow);

        QMetaObject::connectSlotsByName(C64KeyboardWindow);
    } // setupUi

    void retranslateUi(QDialog *C64KeyboardWindow)
    {
        C64KeyboardWindow->setWindowTitle(QApplication::translate("C64KeyboardWindow", "Virtuelle C64 Tastatur", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class C64KeyboardWindow: public Ui_C64KeyboardWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_C64_KEYBOARD_WINDOW_H
