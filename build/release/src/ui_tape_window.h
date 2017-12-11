/********************************************************************************
** Form generated from reading UI file 'tape_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TAPE_WINDOW_H
#define UI_TAPE_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include "widget_file_browse.h"
#include "widget_tape_counter.h"

QT_BEGIN_NAMESPACE

class Ui_TapeWindow
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *Rec;
    QPushButton *Play;
    QPushButton *Rew;
    QPushButton *FFw;
    QPushButton *Stop;
    QPushButton *Volume;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    WidgetTapeCounter *Counter;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label;
    QSpacerItem *horizontalSpacer_5;
    QPushButton *RecordLED;
    QSpacerItem *horizontalSpacer_2;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_2;
    QSpacerItem *horizontalSpacer_4;
    QPushButton *MotorLED;
    QSpacerItem *horizontalSpacer_3;
    WidgetFileBrowse *FileBrowser;
    QLabel *TapeInfoOut;

    void setupUi(QDialog *TapeWindow)
    {
        if (TapeWindow->objectName().isEmpty())
            TapeWindow->setObjectName(QString::fromUtf8("TapeWindow"));
        TapeWindow->resize(485, 380);
        verticalLayout = new QVBoxLayout(TapeWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        Rec = new QPushButton(TapeWindow);
        Rec->setObjectName(QString::fromUtf8("Rec"));
        QFont font;
        font.setPointSize(12);
        font.setBold(true);
        font.setWeight(75);
        Rec->setFont(font);
        Rec->setCheckable(true);

        horizontalLayout->addWidget(Rec);

        Play = new QPushButton(TapeWindow);
        Play->setObjectName(QString::fromUtf8("Play"));
        Play->setFont(font);
        Play->setCheckable(true);

        horizontalLayout->addWidget(Play);

        Rew = new QPushButton(TapeWindow);
        Rew->setObjectName(QString::fromUtf8("Rew"));
        Rew->setFont(font);
        Rew->setCheckable(true);

        horizontalLayout->addWidget(Rew);

        FFw = new QPushButton(TapeWindow);
        FFw->setObjectName(QString::fromUtf8("FFw"));
        FFw->setFont(font);
        FFw->setCheckable(true);

        horizontalLayout->addWidget(FFw);

        Stop = new QPushButton(TapeWindow);
        Stop->setObjectName(QString::fromUtf8("Stop"));
        Stop->setFont(font);
        Stop->setCheckable(true);

        horizontalLayout->addWidget(Stop);

        Volume = new QPushButton(TapeWindow);
        Volume->setObjectName(QString::fromUtf8("Volume"));
        Volume->setIconSize(QSize(16, 16));
        Volume->setFlat(true);

        horizontalLayout->addWidget(Volume);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        Counter = new WidgetTapeCounter(TapeWindow);
        Counter->setObjectName(QString::fromUtf8("Counter"));

        horizontalLayout_2->addWidget(Counter);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(0);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label = new QLabel(TapeWindow);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout_4->addWidget(label);

        horizontalSpacer_5 = new QSpacerItem(5, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_5);

        RecordLED = new QPushButton(TapeWindow);
        RecordLED->setObjectName(QString::fromUtf8("RecordLED"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(RecordLED->sizePolicy().hasHeightForWidth());
        RecordLED->setSizePolicy(sizePolicy);
        RecordLED->setMinimumSize(QSize(12, 12));
        RecordLED->setMaximumSize(QSize(12, 12));

        horizontalLayout_4->addWidget(RecordLED);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_2 = new QLabel(TapeWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_5->addWidget(label_2);

        horizontalSpacer_4 = new QSpacerItem(7, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);

        MotorLED = new QPushButton(TapeWindow);
        MotorLED->setObjectName(QString::fromUtf8("MotorLED"));
        sizePolicy.setHeightForWidth(MotorLED->sizePolicy().hasHeightForWidth());
        MotorLED->setSizePolicy(sizePolicy);
        MotorLED->setMinimumSize(QSize(12, 12));
        MotorLED->setMaximumSize(QSize(12, 12));

        horizontalLayout_5->addWidget(MotorLED);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_3);


        verticalLayout_2->addLayout(horizontalLayout_5);


        horizontalLayout_2->addLayout(verticalLayout_2);


        verticalLayout->addLayout(horizontalLayout_2);

        FileBrowser = new WidgetFileBrowse(TapeWindow);
        FileBrowser->setObjectName(QString::fromUtf8("FileBrowser"));

        verticalLayout->addWidget(FileBrowser);

        TapeInfoOut = new QLabel(TapeWindow);
        TapeInfoOut->setObjectName(QString::fromUtf8("TapeInfoOut"));

        verticalLayout->addWidget(TapeInfoOut);


        retranslateUi(TapeWindow);

        QMetaObject::connectSlotsByName(TapeWindow);
    } // setupUi

    void retranslateUi(QDialog *TapeWindow)
    {
        TapeWindow->setWindowTitle(QApplication::translate("TapeWindow", "Datasette 1530", 0, QApplication::UnicodeUTF8));
        Rec->setText(QApplication::translate("TapeWindow", "RECORD", 0, QApplication::UnicodeUTF8));
        Play->setText(QApplication::translate("TapeWindow", "PLAY", 0, QApplication::UnicodeUTF8));
        Rew->setText(QApplication::translate("TapeWindow", "REWIND", 0, QApplication::UnicodeUTF8));
        FFw->setText(QApplication::translate("TapeWindow", "F.FWD", 0, QApplication::UnicodeUTF8));
        Stop->setText(QApplication::translate("TapeWindow", "STOP", 0, QApplication::UnicodeUTF8));
        Volume->setText(QString());
        label->setText(QApplication::translate("TapeWindow", "Record:", 0, QApplication::UnicodeUTF8));
        RecordLED->setText(QString());
        label_2->setText(QApplication::translate("TapeWindow", "Motor :", 0, QApplication::UnicodeUTF8));
        MotorLED->setText(QString());
        TapeInfoOut->setText(QString());
    } // retranslateUi

};

namespace Ui {
    class TapeWindow: public Ui_TapeWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TAPE_WINDOW_H
