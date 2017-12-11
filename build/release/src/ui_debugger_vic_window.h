/********************************************************************************
** Form generated from reading UI file 'debugger_vic_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DEBUGGER_VIC_WINDOW_H
#define UI_DEBUGGER_VIC_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QHeaderView>
#include <QtGui/QWidget>
#include "tree_widget_mod.h"

QT_BEGIN_NAMESPACE

class Ui_DebuggerVicWindow
{
public:
    TreeWidgetMod *OutputList;

    void setupUi(QWidget *DebuggerVicWindow)
    {
        if (DebuggerVicWindow->objectName().isEmpty())
            DebuggerVicWindow->setObjectName(QString::fromUtf8("DebuggerVicWindow"));
        DebuggerVicWindow->resize(226, 156);
        OutputList = new TreeWidgetMod(DebuggerVicWindow);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        new QTreeWidgetItem(OutputList);
        OutputList->setObjectName(QString::fromUtf8("OutputList"));
        OutputList->setGeometry(QRect(5, 5, 216, 146));
        QFont font;
        font.setPointSize(8);
        OutputList->setFont(font);
        OutputList->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        OutputList->setAutoScroll(true);
        OutputList->setSelectionMode(QAbstractItemView::NoSelection);
        OutputList->setRootIsDecorated(false);
        OutputList->header()->setVisible(true);

        retranslateUi(DebuggerVicWindow);

        QMetaObject::connectSlotsByName(DebuggerVicWindow);
    } // setupUi

    void retranslateUi(QWidget *DebuggerVicWindow)
    {
        DebuggerVicWindow->setWindowTitle(QApplication::translate("DebuggerVicWindow", "VIC", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = OutputList->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("DebuggerVicWindow", "Inhalt", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("DebuggerVicWindow", "Bezeichnung", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = OutputList->isSortingEnabled();
        OutputList->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = OutputList->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("DebuggerVicWindow", "Aktuelle Rasterzeile", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = OutputList->topLevelItem(1);
        ___qtreewidgetitem2->setText(0, QApplication::translate("DebuggerVicWindow", "Raster Latch", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = OutputList->topLevelItem(2);
        ___qtreewidgetitem3->setText(0, QApplication::translate("DebuggerVicWindow", "Aktueller Zyklus", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = OutputList->topLevelItem(3);
        ___qtreewidgetitem4->setText(0, QApplication::translate("DebuggerVicWindow", "Display Status", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = OutputList->topLevelItem(4);
        ___qtreewidgetitem5->setText(0, QApplication::translate("DebuggerVicWindow", "Grafik Modus", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem6 = OutputList->topLevelItem(5);
        ___qtreewidgetitem6->setText(0, QApplication::translate("DebuggerVicWindow", "Bank", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem7 = OutputList->topLevelItem(6);
        ___qtreewidgetitem7->setText(0, QApplication::translate("DebuggerVicWindow", "Matrix Base", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem8 = OutputList->topLevelItem(7);
        ___qtreewidgetitem8->setText(0, QApplication::translate("DebuggerVicWindow", "Char Base", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem9 = OutputList->topLevelItem(8);
        ___qtreewidgetitem9->setText(0, QApplication::translate("DebuggerVicWindow", "IRQ Leitung", 0, QApplication::UnicodeUTF8));
        OutputList->setSortingEnabled(__sortingEnabled);

    } // retranslateUi

};

namespace Ui {
    class DebuggerVicWindow: public Ui_DebuggerVicWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DEBUGGER_VIC_WINDOW_H
