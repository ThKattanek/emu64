/********************************************************************************
** Form generated from reading UI file 'memory_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MEMORY_WINDOW_H
#define UI_MEMORY_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QScrollBar>
#include <QtGui/QTableWidget>

QT_BEGIN_NAMESPACE

class Ui_MemoryWindow
{
public:
    QTableWidget *MemoryTable;
    QScrollBar *MemoryScroll;
    QCheckBox *BitAnzeige;
    QCheckBox *OnlyRam;

    void setupUi(QDialog *MemoryWindow)
    {
        if (MemoryWindow->objectName().isEmpty())
            MemoryWindow->setObjectName(QString::fromUtf8("MemoryWindow"));
        MemoryWindow->resize(550, 311);
        MemoryWindow->setMinimumSize(QSize(550, 311));
        MemoryWindow->setMaximumSize(QSize(550, 311));
        MemoryWindow->setWindowTitle(QString::fromUtf8(""));
        MemoryTable = new QTableWidget(MemoryWindow);
        MemoryTable->setObjectName(QString::fromUtf8("MemoryTable"));
        MemoryTable->setGeometry(QRect(5, 5, 521, 278));
        MemoryTable->setFocusPolicy(Qt::NoFocus);
        MemoryTable->setSelectionMode(QAbstractItemView::NoSelection);
        MemoryTable->setShowGrid(false);
        MemoryTable->setGridStyle(Qt::DashLine);
        MemoryTable->horizontalHeader()->setVisible(false);
        MemoryTable->verticalHeader()->setVisible(false);
        MemoryScroll = new QScrollBar(MemoryWindow);
        MemoryScroll->setObjectName(QString::fromUtf8("MemoryScroll"));
        MemoryScroll->setGeometry(QRect(523, 3, 21, 281));
        MemoryScroll->setMaximum(65295);
        MemoryScroll->setSingleStep(16);
        MemoryScroll->setPageStep(256);
        MemoryScroll->setOrientation(Qt::Vertical);
        BitAnzeige = new QCheckBox(MemoryWindow);
        BitAnzeige->setObjectName(QString::fromUtf8("BitAnzeige"));
        BitAnzeige->setGeometry(QRect(10, 290, 111, 17));
        BitAnzeige->setFocusPolicy(Qt::NoFocus);
        BitAnzeige->setChecked(false);
        OnlyRam = new QCheckBox(MemoryWindow);
        OnlyRam->setObjectName(QString::fromUtf8("OnlyRam"));
        OnlyRam->setGeometry(QRect(130, 290, 131, 21));

        retranslateUi(MemoryWindow);

        QMetaObject::connectSlotsByName(MemoryWindow);
    } // setupUi

    void retranslateUi(QDialog *MemoryWindow)
    {
        BitAnzeige->setText(QApplication::translate("MemoryWindow", "Bit Anzeige", 0, QApplication::UnicodeUTF8));
        OnlyRam->setText(QApplication::translate("MemoryWindow", "Nur RAM anzeigen", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(MemoryWindow);
    } // retranslateUi

};

namespace Ui {
    class MemoryWindow: public Ui_MemoryWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MEMORY_WINDOW_H
