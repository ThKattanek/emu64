/********************************************************************************
** Form generated from reading UI file 'widget_tape_counter.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_TAPE_COUNTER_H
#define UI_WIDGET_TAPE_COUNTER_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WidgetTapeCounter
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *ziffer2;
    QLabel *ziffer1;
    QLabel *ziffer0;

    void setupUi(QWidget *WidgetTapeCounter)
    {
        if (WidgetTapeCounter->objectName().isEmpty())
            WidgetTapeCounter->setObjectName(QString::fromUtf8("WidgetTapeCounter"));
        WidgetTapeCounter->resize(100, 50);
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(WidgetTapeCounter->sizePolicy().hasHeightForWidth());
        WidgetTapeCounter->setSizePolicy(sizePolicy);
        horizontalLayout = new QHBoxLayout(WidgetTapeCounter);
        horizontalLayout->setSpacing(2);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        ziffer2 = new QLabel(WidgetTapeCounter);
        ziffer2->setObjectName(QString::fromUtf8("ziffer2"));

        horizontalLayout->addWidget(ziffer2);

        ziffer1 = new QLabel(WidgetTapeCounter);
        ziffer1->setObjectName(QString::fromUtf8("ziffer1"));

        horizontalLayout->addWidget(ziffer1);

        ziffer0 = new QLabel(WidgetTapeCounter);
        ziffer0->setObjectName(QString::fromUtf8("ziffer0"));

        horizontalLayout->addWidget(ziffer0);


        retranslateUi(WidgetTapeCounter);

        QMetaObject::connectSlotsByName(WidgetTapeCounter);
    } // setupUi

    void retranslateUi(QWidget *WidgetTapeCounter)
    {
        WidgetTapeCounter->setWindowTitle(QApplication::translate("WidgetTapeCounter", "Form", 0, QApplication::UnicodeUTF8));
        ziffer2->setText(QApplication::translate("WidgetTapeCounter", "0", 0, QApplication::UnicodeUTF8));
        ziffer1->setText(QApplication::translate("WidgetTapeCounter", "0", 0, QApplication::UnicodeUTF8));
        ziffer0->setText(QApplication::translate("WidgetTapeCounter", "0", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class WidgetTapeCounter: public Ui_WidgetTapeCounter {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_TAPE_COUNTER_H
