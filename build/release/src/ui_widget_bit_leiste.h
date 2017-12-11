/********************************************************************************
** Form generated from reading UI file 'widget_bit_leiste.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_BIT_LEISTE_H
#define UI_WIDGET_BIT_LEISTE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WidgetBitLeiste
{
public:

    void setupUi(QWidget *WidgetBitLeiste)
    {
        if (WidgetBitLeiste->objectName().isEmpty())
            WidgetBitLeiste->setObjectName(QString::fromUtf8("WidgetBitLeiste"));
        WidgetBitLeiste->resize(20, 16);

        retranslateUi(WidgetBitLeiste);

        QMetaObject::connectSlotsByName(WidgetBitLeiste);
    } // setupUi

    void retranslateUi(QWidget *WidgetBitLeiste)
    {
        WidgetBitLeiste->setWindowTitle(QApplication::translate("WidgetBitLeiste", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WidgetBitLeiste: public Ui_WidgetBitLeiste {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_BIT_LEISTE_H
