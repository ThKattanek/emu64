/********************************************************************************
** Form generated from reading UI file 'widget_sr_edit.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_SR_EDIT_H
#define UI_WIDGET_SR_EDIT_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WidgetSREdit
{
public:
    QPushButton *Bit7;
    QPushButton *Bit6;
    QPushButton *Bit5;
    QPushButton *Bit4;
    QPushButton *Bit3;
    QPushButton *Bit2;
    QPushButton *Bit1;
    QPushButton *Bit0;
    QLabel *Label_Validate;
    QLabel *Label_Undefined;
    QLabel *Label_Break;
    QLabel *Label_Interrupt;
    QLabel *Label_Decimal;
    QLabel *Label_Zero;
    QLabel *Label_Carray;
    QLabel *Label_Negativ;

    void setupUi(QWidget *WidgetSREdit)
    {
        if (WidgetSREdit->objectName().isEmpty())
            WidgetSREdit->setObjectName(QString::fromUtf8("WidgetSREdit"));
        WidgetSREdit->resize(110, 26);
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(10);
        WidgetSREdit->setFont(font);
        WidgetSREdit->setWindowTitle(QString::fromUtf8("Form"));
        Bit7 = new QPushButton(WidgetSREdit);
        Bit7->setObjectName(QString::fromUtf8("Bit7"));
        Bit7->setGeometry(QRect(0, 14, 12, 12));
        Bit7->setFocusPolicy(Qt::NoFocus);
        Bit7->setText(QString::fromUtf8(""));
        Bit6 = new QPushButton(WidgetSREdit);
        Bit6->setObjectName(QString::fromUtf8("Bit6"));
        Bit6->setGeometry(QRect(14, 14, 12, 12));
        Bit6->setFocusPolicy(Qt::NoFocus);
        Bit6->setText(QString::fromUtf8(""));
        Bit5 = new QPushButton(WidgetSREdit);
        Bit5->setObjectName(QString::fromUtf8("Bit5"));
        Bit5->setGeometry(QRect(28, 14, 12, 12));
        Bit5->setFocusPolicy(Qt::NoFocus);
        Bit5->setText(QString::fromUtf8(""));
        Bit4 = new QPushButton(WidgetSREdit);
        Bit4->setObjectName(QString::fromUtf8("Bit4"));
        Bit4->setGeometry(QRect(42, 14, 12, 12));
        Bit4->setFocusPolicy(Qt::NoFocus);
        Bit4->setText(QString::fromUtf8(""));
        Bit3 = new QPushButton(WidgetSREdit);
        Bit3->setObjectName(QString::fromUtf8("Bit3"));
        Bit3->setGeometry(QRect(56, 14, 12, 12));
        Bit3->setFocusPolicy(Qt::NoFocus);
        Bit3->setText(QString::fromUtf8(""));
        Bit2 = new QPushButton(WidgetSREdit);
        Bit2->setObjectName(QString::fromUtf8("Bit2"));
        Bit2->setGeometry(QRect(70, 14, 12, 12));
        Bit2->setFocusPolicy(Qt::NoFocus);
        Bit2->setText(QString::fromUtf8(""));
        Bit1 = new QPushButton(WidgetSREdit);
        Bit1->setObjectName(QString::fromUtf8("Bit1"));
        Bit1->setGeometry(QRect(84, 14, 12, 12));
        Bit1->setFocusPolicy(Qt::NoFocus);
        Bit1->setText(QString::fromUtf8(""));
        Bit0 = new QPushButton(WidgetSREdit);
        Bit0->setObjectName(QString::fromUtf8("Bit0"));
        Bit0->setGeometry(QRect(98, 14, 12, 12));
        Bit0->setFocusPolicy(Qt::NoFocus);
        Bit0->setText(QString::fromUtf8(""));
        Label_Validate = new QLabel(WidgetSREdit);
        Label_Validate->setObjectName(QString::fromUtf8("Label_Validate"));
        Label_Validate->setGeometry(QRect(14, 0, 12, 12));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setBold(true);
        font1.setWeight(75);
        Label_Validate->setFont(font1);
        Label_Validate->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);\n"
""));
        Label_Validate->setText(QString::fromUtf8("V"));
        Label_Validate->setAlignment(Qt::AlignCenter);
        Label_Undefined = new QLabel(WidgetSREdit);
        Label_Undefined->setObjectName(QString::fromUtf8("Label_Undefined"));
        Label_Undefined->setGeometry(QRect(28, 0, 12, 12));
        Label_Undefined->setFont(font1);
        Label_Undefined->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        Label_Undefined->setText(QString::fromUtf8("/"));
        Label_Undefined->setAlignment(Qt::AlignCenter);
        Label_Break = new QLabel(WidgetSREdit);
        Label_Break->setObjectName(QString::fromUtf8("Label_Break"));
        Label_Break->setGeometry(QRect(42, 0, 12, 12));
        Label_Break->setFont(font1);
        Label_Break->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        Label_Break->setText(QString::fromUtf8("B"));
        Label_Break->setAlignment(Qt::AlignCenter);
        Label_Interrupt = new QLabel(WidgetSREdit);
        Label_Interrupt->setObjectName(QString::fromUtf8("Label_Interrupt"));
        Label_Interrupt->setGeometry(QRect(70, 0, 12, 12));
        Label_Interrupt->setFont(font1);
        Label_Interrupt->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        Label_Interrupt->setText(QString::fromUtf8("I"));
        Label_Interrupt->setAlignment(Qt::AlignCenter);
        Label_Decimal = new QLabel(WidgetSREdit);
        Label_Decimal->setObjectName(QString::fromUtf8("Label_Decimal"));
        Label_Decimal->setGeometry(QRect(56, 0, 12, 12));
        Label_Decimal->setFont(font1);
        Label_Decimal->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        Label_Decimal->setText(QString::fromUtf8("D"));
        Label_Decimal->setAlignment(Qt::AlignCenter);
        Label_Zero = new QLabel(WidgetSREdit);
        Label_Zero->setObjectName(QString::fromUtf8("Label_Zero"));
        Label_Zero->setGeometry(QRect(84, 0, 12, 12));
        Label_Zero->setFont(font1);
        Label_Zero->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        Label_Zero->setText(QString::fromUtf8("Z"));
        Label_Zero->setAlignment(Qt::AlignCenter);
        Label_Carray = new QLabel(WidgetSREdit);
        Label_Carray->setObjectName(QString::fromUtf8("Label_Carray"));
        Label_Carray->setGeometry(QRect(98, 0, 12, 12));
        Label_Carray->setFont(font1);
        Label_Carray->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        Label_Carray->setText(QString::fromUtf8("C"));
        Label_Carray->setAlignment(Qt::AlignCenter);
        Label_Negativ = new QLabel(WidgetSREdit);
        Label_Negativ->setObjectName(QString::fromUtf8("Label_Negativ"));
        Label_Negativ->setGeometry(QRect(0, 0, 12, 12));
        Label_Negativ->setFont(font1);
        Label_Negativ->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        Label_Negativ->setText(QString::fromUtf8("N"));
        Label_Negativ->setAlignment(Qt::AlignCenter);

        retranslateUi(WidgetSREdit);

        QMetaObject::connectSlotsByName(WidgetSREdit);
    } // setupUi

    void retranslateUi(QWidget *WidgetSREdit)
    {
#ifndef QT_NO_TOOLTIP
        Bit7->setToolTip(QApplication::translate("WidgetSREdit", "Negativ Flag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Bit6->setToolTip(QApplication::translate("WidgetSREdit", "\303\234berlauf Flag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Bit5->setToolTip(QApplication::translate("WidgetSREdit", "Nicht benutzt und ist immer gesetzt.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Bit4->setToolTip(QApplication::translate("WidgetSREdit", "Break Flag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Bit3->setToolTip(QApplication::translate("WidgetSREdit", "Dezimal Flag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Bit2->setToolTip(QApplication::translate("WidgetSREdit", "Interrupt Flag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Bit1->setToolTip(QApplication::translate("WidgetSREdit", "Null Flag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_TOOLTIP
        Bit0->setToolTip(QApplication::translate("WidgetSREdit", "Carry Flag.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        Q_UNUSED(WidgetSREdit);
    } // retranslateUi

};

namespace Ui {
    class WidgetSREdit: public Ui_WidgetSREdit {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_SR_EDIT_H
