/********************************************************************************
** Form generated from reading UI file 'widget_memory_zeile.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_MEMORY_ZEILE_H
#define UI_WIDGET_MEMORY_ZEILE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QWidget>
#include "widget_bit_leiste.h"

QT_BEGIN_NAMESPACE

class Ui_WidgetMemoryZeile
{
public:
    QLabel *AdresseOut;
    QLineEdit *EditValue_0;
    QLineEdit *EditValue_1;
    QLineEdit *EditValue_2;
    QLineEdit *EditValue_3;
    QLineEdit *EditValue_4;
    QLineEdit *EditValue_5;
    QLineEdit *EditValue_6;
    QLineEdit *EditValue_7;
    QLineEdit *EditValue_8;
    QLineEdit *EditValue_9;
    QLineEdit *EditValue_10;
    QLineEdit *EditValue_11;
    QLineEdit *EditValue_12;
    QLineEdit *EditValue_13;
    QLineEdit *EditValue_14;
    QLineEdit *EditValue_15;
    WidgetBitLeiste *BitLeiste_0;
    WidgetBitLeiste *BitLeiste_1;
    WidgetBitLeiste *BitLeiste_2;
    WidgetBitLeiste *BitLeiste_3;
    WidgetBitLeiste *BitLeiste_4;
    WidgetBitLeiste *BitLeiste_5;
    WidgetBitLeiste *BitLeiste_6;
    WidgetBitLeiste *BitLeiste_7;
    WidgetBitLeiste *BitLeiste_8;
    WidgetBitLeiste *BitLeiste_9;
    WidgetBitLeiste *BitLeiste_10;
    WidgetBitLeiste *BitLeiste_11;
    WidgetBitLeiste *BitLeiste_12;
    WidgetBitLeiste *BitLeiste_13;
    WidgetBitLeiste *BitLeiste_14;
    WidgetBitLeiste *BitLeiste_15;
    QLabel *ReadOut;
    QLabel *WriteOut;

    void setupUi(QWidget *WidgetMemoryZeile)
    {
        if (WidgetMemoryZeile->objectName().isEmpty())
            WidgetMemoryZeile->setObjectName(QString::fromUtf8("WidgetMemoryZeile"));
        WidgetMemoryZeile->resize(516, 16);
        WidgetMemoryZeile->setAutoFillBackground(false);
        AdresseOut = new QLabel(WidgetMemoryZeile);
        AdresseOut->setObjectName(QString::fromUtf8("AdresseOut"));
        AdresseOut->setGeometry(QRect(0, 0, 45, 16));
        AdresseOut->setStyleSheet(QString::fromUtf8("border-right: 1px solid rgb(0,50,0);\n"
"border-bottom: 1px solid rgb(0,50,0);\n"
"background-color: rgb(100,100, 200);\n"
"color: rgb(170, 170, 255);\n"
""));
        AdresseOut->setText(QString::fromUtf8("ADR"));
        AdresseOut->setAlignment(Qt::AlignCenter);
        EditValue_0 = new QLineEdit(WidgetMemoryZeile);
        EditValue_0->setObjectName(QString::fromUtf8("EditValue_0"));
        EditValue_0->setGeometry(QRect(45, 0, 20, 15));
        EditValue_0->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_0->setText(QString::fromUtf8("00"));
        EditValue_0->setMaxLength(2);
        EditValue_1 = new QLineEdit(WidgetMemoryZeile);
        EditValue_1->setObjectName(QString::fromUtf8("EditValue_1"));
        EditValue_1->setGeometry(QRect(65, 0, 20, 15));
        EditValue_1->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_1->setText(QString::fromUtf8("01"));
        EditValue_1->setMaxLength(2);
        EditValue_2 = new QLineEdit(WidgetMemoryZeile);
        EditValue_2->setObjectName(QString::fromUtf8("EditValue_2"));
        EditValue_2->setGeometry(QRect(85, 0, 20, 15));
        EditValue_2->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_2->setText(QString::fromUtf8("02"));
        EditValue_2->setMaxLength(2);
        EditValue_3 = new QLineEdit(WidgetMemoryZeile);
        EditValue_3->setObjectName(QString::fromUtf8("EditValue_3"));
        EditValue_3->setGeometry(QRect(105, 0, 20, 15));
        EditValue_3->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_3->setText(QString::fromUtf8("03"));
        EditValue_3->setMaxLength(2);
        EditValue_4 = new QLineEdit(WidgetMemoryZeile);
        EditValue_4->setObjectName(QString::fromUtf8("EditValue_4"));
        EditValue_4->setGeometry(QRect(125, 0, 20, 15));
        EditValue_4->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_4->setText(QString::fromUtf8("04"));
        EditValue_4->setMaxLength(2);
        EditValue_5 = new QLineEdit(WidgetMemoryZeile);
        EditValue_5->setObjectName(QString::fromUtf8("EditValue_5"));
        EditValue_5->setGeometry(QRect(145, 0, 20, 15));
        EditValue_5->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_5->setText(QString::fromUtf8("05"));
        EditValue_5->setMaxLength(2);
        EditValue_6 = new QLineEdit(WidgetMemoryZeile);
        EditValue_6->setObjectName(QString::fromUtf8("EditValue_6"));
        EditValue_6->setGeometry(QRect(165, 0, 20, 15));
        EditValue_6->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_6->setText(QString::fromUtf8("06"));
        EditValue_6->setMaxLength(2);
        EditValue_7 = new QLineEdit(WidgetMemoryZeile);
        EditValue_7->setObjectName(QString::fromUtf8("EditValue_7"));
        EditValue_7->setGeometry(QRect(185, 0, 20, 15));
        EditValue_7->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_7->setText(QString::fromUtf8("07"));
        EditValue_7->setMaxLength(2);
        EditValue_8 = new QLineEdit(WidgetMemoryZeile);
        EditValue_8->setObjectName(QString::fromUtf8("EditValue_8"));
        EditValue_8->setGeometry(QRect(205, 0, 20, 15));
        EditValue_8->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_8->setText(QString::fromUtf8("08"));
        EditValue_8->setMaxLength(2);
        EditValue_9 = new QLineEdit(WidgetMemoryZeile);
        EditValue_9->setObjectName(QString::fromUtf8("EditValue_9"));
        EditValue_9->setGeometry(QRect(225, 0, 20, 15));
        EditValue_9->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_9->setText(QString::fromUtf8("09"));
        EditValue_9->setMaxLength(2);
        EditValue_10 = new QLineEdit(WidgetMemoryZeile);
        EditValue_10->setObjectName(QString::fromUtf8("EditValue_10"));
        EditValue_10->setGeometry(QRect(245, 0, 20, 15));
        EditValue_10->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_10->setText(QString::fromUtf8("0A"));
        EditValue_10->setMaxLength(2);
        EditValue_11 = new QLineEdit(WidgetMemoryZeile);
        EditValue_11->setObjectName(QString::fromUtf8("EditValue_11"));
        EditValue_11->setGeometry(QRect(265, 0, 20, 15));
        EditValue_11->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_11->setText(QString::fromUtf8("0B"));
        EditValue_11->setMaxLength(2);
        EditValue_12 = new QLineEdit(WidgetMemoryZeile);
        EditValue_12->setObjectName(QString::fromUtf8("EditValue_12"));
        EditValue_12->setGeometry(QRect(285, 0, 20, 15));
        EditValue_12->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_12->setText(QString::fromUtf8("0C"));
        EditValue_12->setMaxLength(2);
        EditValue_13 = new QLineEdit(WidgetMemoryZeile);
        EditValue_13->setObjectName(QString::fromUtf8("EditValue_13"));
        EditValue_13->setGeometry(QRect(305, 0, 20, 15));
        EditValue_13->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_13->setText(QString::fromUtf8("0D"));
        EditValue_13->setMaxLength(2);
        EditValue_14 = new QLineEdit(WidgetMemoryZeile);
        EditValue_14->setObjectName(QString::fromUtf8("EditValue_14"));
        EditValue_14->setGeometry(QRect(325, 0, 20, 15));
        EditValue_14->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(200, 200, 255);\n"
""));
        EditValue_14->setText(QString::fromUtf8("0E"));
        EditValue_14->setMaxLength(2);
        EditValue_15 = new QLineEdit(WidgetMemoryZeile);
        EditValue_15->setObjectName(QString::fromUtf8("EditValue_15"));
        EditValue_15->setGeometry(QRect(345, 0, 20, 15));
        EditValue_15->setStyleSheet(QString::fromUtf8("border: 0px solid gray;\n"
"background-color: rgb(180,180, 255);\n"
""));
        EditValue_15->setText(QString::fromUtf8("0F"));
        EditValue_15->setMaxLength(2);
        BitLeiste_0 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_0->setObjectName(QString::fromUtf8("BitLeiste_0"));
        BitLeiste_0->setGeometry(QRect(45, 15, 20, 1));
        BitLeiste_1 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_1->setObjectName(QString::fromUtf8("BitLeiste_1"));
        BitLeiste_1->setGeometry(QRect(65, 15, 20, 1));
        BitLeiste_2 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_2->setObjectName(QString::fromUtf8("BitLeiste_2"));
        BitLeiste_2->setGeometry(QRect(85, 15, 20, 1));
        BitLeiste_3 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_3->setObjectName(QString::fromUtf8("BitLeiste_3"));
        BitLeiste_3->setGeometry(QRect(105, 15, 20, 1));
        BitLeiste_4 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_4->setObjectName(QString::fromUtf8("BitLeiste_4"));
        BitLeiste_4->setGeometry(QRect(125, 15, 20, 1));
        BitLeiste_5 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_5->setObjectName(QString::fromUtf8("BitLeiste_5"));
        BitLeiste_5->setGeometry(QRect(145, 15, 20, 1));
        BitLeiste_6 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_6->setObjectName(QString::fromUtf8("BitLeiste_6"));
        BitLeiste_6->setGeometry(QRect(165, 15, 20, 1));
        BitLeiste_7 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_7->setObjectName(QString::fromUtf8("BitLeiste_7"));
        BitLeiste_7->setGeometry(QRect(185, 15, 20, 1));
        BitLeiste_8 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_8->setObjectName(QString::fromUtf8("BitLeiste_8"));
        BitLeiste_8->setGeometry(QRect(205, 15, 20, 1));
        BitLeiste_9 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_9->setObjectName(QString::fromUtf8("BitLeiste_9"));
        BitLeiste_9->setGeometry(QRect(225, 15, 20, 1));
        BitLeiste_10 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_10->setObjectName(QString::fromUtf8("BitLeiste_10"));
        BitLeiste_10->setGeometry(QRect(245, 15, 20, 1));
        BitLeiste_11 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_11->setObjectName(QString::fromUtf8("BitLeiste_11"));
        BitLeiste_11->setGeometry(QRect(265, 15, 20, 1));
        BitLeiste_12 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_12->setObjectName(QString::fromUtf8("BitLeiste_12"));
        BitLeiste_12->setGeometry(QRect(285, 15, 20, 1));
        BitLeiste_13 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_13->setObjectName(QString::fromUtf8("BitLeiste_13"));
        BitLeiste_13->setGeometry(QRect(305, 15, 20, 1));
        BitLeiste_14 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_14->setObjectName(QString::fromUtf8("BitLeiste_14"));
        BitLeiste_14->setGeometry(QRect(325, 15, 20, 1));
        BitLeiste_15 = new WidgetBitLeiste(WidgetMemoryZeile);
        BitLeiste_15->setObjectName(QString::fromUtf8("BitLeiste_15"));
        BitLeiste_15->setGeometry(QRect(345, 15, 20, 1));
        ReadOut = new QLabel(WidgetMemoryZeile);
        ReadOut->setObjectName(QString::fromUtf8("ReadOut"));
        ReadOut->setGeometry(QRect(365, 0, 75, 16));
        ReadOut->setStyleSheet(QString::fromUtf8("border-left: 1px solid rgb(0,50,0);\n"
"border-bottom: 1px solid rgb(0,50,0);\n"
"background-color: rgb(100,100, 200);\n"
"color: rgb(170, 170, 255);"));
        ReadOut->setText(QString::fromUtf8("Lesen"));
        ReadOut->setAlignment(Qt::AlignCenter);
        WriteOut = new QLabel(WidgetMemoryZeile);
        WriteOut->setObjectName(QString::fromUtf8("WriteOut"));
        WriteOut->setGeometry(QRect(440, 0, 75, 16));
        WriteOut->setStyleSheet(QString::fromUtf8("border-left: 1px solid rgb(0,50,0);\n"
"border-bottom: 1px solid rgb(0,50,0);\n"
"background-color: rgb(100,100, 200);\n"
"color: rgb(170, 170, 255);"));
        WriteOut->setText(QString::fromUtf8("Schreiben"));
        WriteOut->setAlignment(Qt::AlignCenter);

        retranslateUi(WidgetMemoryZeile);

        QMetaObject::connectSlotsByName(WidgetMemoryZeile);
    } // setupUi

    void retranslateUi(QWidget *WidgetMemoryZeile)
    {
        WidgetMemoryZeile->setWindowTitle(QApplication::translate("WidgetMemoryZeile", "Form", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WidgetMemoryZeile: public Ui_WidgetMemoryZeile {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_MEMORY_ZEILE_H
