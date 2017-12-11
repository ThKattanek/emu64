/********************************************************************************
** Form generated from reading UI file 'debugger_iec_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGGER_IEC_WINDOW_H
#define UI_DEBUGGER_IEC_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_DebuggerIECWindow
{
public:
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_6;
    QPushButton *DataIn;
    QSpacerItem *horizontalSpacer_4;
    QLabel *label_2;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_7;
    QPushButton *ClkIn;
    QSpacerItem *horizontalSpacer_5;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_3;
    QPushButton *DataOut;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_5;
    QPushButton *AtnOut;
    QSpacerItem *horizontalSpacer_3;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_4;
    QPushButton *ClkOut;
    QSpacerItem *horizontalSpacer_2;
    QVBoxLayout *verticalLayout;
    QPushButton *StartExport;
    QPushButton *StopExport;
    QSpacerItem *verticalSpacer;
    QSpacerItem *horizontalSpacer_6;

    void setupUi(QDialog *DebuggerIECWindow)
    {
        if (DebuggerIECWindow->objectName().isEmpty())
            DebuggerIECWindow->setObjectName(QString::fromUtf8("DebuggerIECWindow"));
        DebuggerIECWindow->resize(286, 87);
        horizontalLayout = new QHBoxLayout(DebuggerIECWindow);
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(2, 2, 2, 2);
        gridLayout = new QGridLayout();
        gridLayout->setSpacing(0);
        gridLayout->setObjectName(QString::fromUtf8("gridLayout"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_6 = new QLabel(DebuggerIECWindow);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setMinimumSize(QSize(40, 0));
        label_6->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_5->addWidget(label_6);

        DataIn = new QPushButton(DebuggerIECWindow);
        DataIn->setObjectName(QString::fromUtf8("DataIn"));
        DataIn->setMinimumSize(QSize(12, 12));
        DataIn->setMaximumSize(QSize(12, 12));

        horizontalLayout_5->addWidget(DataIn);

        horizontalSpacer_4 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_4);


        gridLayout->addLayout(horizontalLayout_5, 1, 1, 1, 1);

        label_2 = new QLabel(DebuggerIECWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QFont font;
        font.setBold(true);
        font.setWeight(75);
        label_2->setFont(font);
        label_2->setStyleSheet(QString::fromUtf8("background-color: rgb(188, 199, 233);"));
        label_2->setFrameShape(QFrame::Box);
        label_2->setFrameShadow(QFrame::Sunken);
        label_2->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label_2, 0, 1, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setObjectName(QString::fromUtf8("horizontalLayout_6"));
        label_7 = new QLabel(DebuggerIECWindow);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setMinimumSize(QSize(40, 0));
        label_7->setMaximumSize(QSize(40, 16777215));

        horizontalLayout_6->addWidget(label_7);

        ClkIn = new QPushButton(DebuggerIECWindow);
        ClkIn->setObjectName(QString::fromUtf8("ClkIn"));
        ClkIn->setMinimumSize(QSize(12, 12));
        ClkIn->setMaximumSize(QSize(12, 12));

        horizontalLayout_6->addWidget(ClkIn);

        horizontalSpacer_5 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_6->addItem(horizontalSpacer_5);


        gridLayout->addLayout(horizontalLayout_6, 2, 1, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_3 = new QLabel(DebuggerIECWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(39, 0));
        label_3->setMaximumSize(QSize(39, 16777215));

        horizontalLayout_2->addWidget(label_3);

        DataOut = new QPushButton(DebuggerIECWindow);
        DataOut->setObjectName(QString::fromUtf8("DataOut"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(DataOut->sizePolicy().hasHeightForWidth());
        DataOut->setSizePolicy(sizePolicy);
        DataOut->setMinimumSize(QSize(12, 12));
        DataOut->setMaximumSize(QSize(12, 12));

        horizontalLayout_2->addWidget(DataOut);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setObjectName(QString::fromUtf8("horizontalLayout_7"));

        gridLayout->addLayout(horizontalLayout_7, 3, 1, 1, 1);

        label = new QLabel(DebuggerIECWindow);
        label->setObjectName(QString::fromUtf8("label"));
        label->setFont(font);
        label->setStyleSheet(QString::fromUtf8("background-color: rgb(188, 199, 233);"));
        label->setFrameShape(QFrame::Box);
        label->setFrameShadow(QFrame::Sunken);
        label->setAlignment(Qt::AlignCenter);

        gridLayout->addWidget(label, 0, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_5 = new QLabel(DebuggerIECWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(45, 0));
        label_5->setMaximumSize(QSize(45, 16777215));

        horizontalLayout_4->addWidget(label_5);

        AtnOut = new QPushButton(DebuggerIECWindow);
        AtnOut->setObjectName(QString::fromUtf8("AtnOut"));
        AtnOut->setMinimumSize(QSize(12, 12));
        AtnOut->setMaximumSize(QSize(12, 12));

        horizontalLayout_4->addWidget(AtnOut);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_3);


        gridLayout->addLayout(horizontalLayout_4, 3, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_4 = new QLabel(DebuggerIECWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(45, 0));
        label_4->setMaximumSize(QSize(45, 16777215));

        horizontalLayout_3->addWidget(label_4);

        ClkOut = new QPushButton(DebuggerIECWindow);
        ClkOut->setObjectName(QString::fromUtf8("ClkOut"));
        ClkOut->setMinimumSize(QSize(12, 12));
        ClkOut->setMaximumSize(QSize(12, 12));

        horizontalLayout_3->addWidget(ClkOut);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        gridLayout->addLayout(horizontalLayout_3, 2, 0, 1, 1);


        horizontalLayout->addLayout(gridLayout);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        StartExport = new QPushButton(DebuggerIECWindow);
        StartExport->setObjectName(QString::fromUtf8("StartExport"));

        verticalLayout->addWidget(StartExport);

        StopExport = new QPushButton(DebuggerIECWindow);
        StopExport->setObjectName(QString::fromUtf8("StopExport"));
        StopExport->setEnabled(true);

        verticalLayout->addWidget(StopExport);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout->addLayout(verticalLayout);

        horizontalSpacer_6 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_6);


        retranslateUi(DebuggerIECWindow);

        QMetaObject::connectSlotsByName(DebuggerIECWindow);
    } // setupUi

    void retranslateUi(QDialog *DebuggerIECWindow)
    {
        DebuggerIECWindow->setWindowTitle(QApplication::translate("DebuggerIECWindow", "IEC BUS", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("DebuggerIECWindow", "DATA:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("DebuggerIECWindow", "Floppy Out", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("DebuggerIECWindow", "CLK:", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("DebuggerIECWindow", "DATA:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("DebuggerIECWindow", "C64 Out", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("DebuggerIECWindow", "ATN:", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("DebuggerIECWindow", "CLK:", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        StartExport->setToolTip(QApplication::translate("DebuggerIECWindow", "Die IEC Signale werden als Signal-Zeit-Diagram aufgezeichnet.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        StartExport->setText(QApplication::translate("DebuggerIECWindow", "Signale aufzeichnen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        StopExport->setToolTip(QApplication::translate("DebuggerIECWindow", "Die IEC Signalaufzeichnung wird beendet.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        StopExport->setText(QApplication::translate("DebuggerIECWindow", "Aufzeichnung stoppen", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class DebuggerIECWindow: public Ui_DebuggerIECWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGGER_IEC_WINDOW_H
