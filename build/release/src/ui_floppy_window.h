/********************************************************************************
** Form generated from reading UI file 'floppy_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FLOPPY_WINDOW_H
#define UI_FLOPPY_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QTableWidget>
#include <QtGui/QVBoxLayout>
#include "widget_file_browse.h"

QT_BEGIN_NAMESPACE

class Ui_FloppyWindow
{
public:
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QComboBox *FloppySelect;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *CreateNewD64;
    QSpacerItem *horizontalSpacer_7;
    QSpacerItem *horizontalSpacer_5;
    WidgetFileBrowse *FileBrowser;
    QVBoxLayout *verticalLayout;
    QSpacerItem *horizontalSpacer_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *DiskName;
    QSpacerItem *horizontalSpacer_2;
    QTableWidget *D64FileTable;
    QHBoxLayout *horizontalLayout;
    QCheckBox *D64FileTableBigSize;
    QSpacerItem *horizontalSpacer_4;
    QCheckBox *ViewSplatFiles;
    QSpacerItem *horizontalSpacer_3;

    void setupUi(QDialog *FloppyWindow)
    {
        if (FloppyWindow->objectName().isEmpty())
            FloppyWindow->setObjectName(QString::fromUtf8("FloppyWindow"));
        FloppyWindow->resize(692, 424);
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setBold(false);
        font.setWeight(50);
        FloppyWindow->setFont(font);
        verticalLayout_2 = new QVBoxLayout(FloppyWindow);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(0);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        FloppySelect = new QComboBox(FloppyWindow);
        FloppySelect->setObjectName(QString::fromUtf8("FloppySelect"));

        horizontalLayout_2->addWidget(FloppySelect);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_2);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(0);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(0);
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        CreateNewD64 = new QPushButton(FloppyWindow);
        CreateNewD64->setObjectName(QString::fromUtf8("CreateNewD64"));

        horizontalLayout_5->addWidget(CreateNewD64);

        horizontalSpacer_7 = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_5->addItem(horizontalSpacer_7);


        verticalLayout_3->addLayout(horizontalLayout_5);

        horizontalSpacer_5 = new QSpacerItem(40, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

        verticalLayout_3->addItem(horizontalSpacer_5);

        FileBrowser = new WidgetFileBrowse(FloppyWindow);
        FileBrowser->setObjectName(QString::fromUtf8("FileBrowser"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(FileBrowser->sizePolicy().hasHeightForWidth());
        FileBrowser->setSizePolicy(sizePolicy);
        FileBrowser->setMinimumSize(QSize(200, 0));

        verticalLayout_3->addWidget(FileBrowser);


        horizontalLayout_4->addLayout(verticalLayout_3);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setSizeConstraint(QLayout::SetMinimumSize);
        horizontalSpacer_6 = new QSpacerItem(40, 0, QSizePolicy::Fixed, QSizePolicy::Minimum);

        verticalLayout->addItem(horizontalSpacer_6);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        DiskName = new QLabel(FloppyWindow);
        DiskName->setObjectName(QString::fromUtf8("DiskName"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        DiskName->setFont(font1);
        DiskName->setStyleSheet(QString::fromUtf8("color: rgb(50, 50, 255);"));
        DiskName->setFrameShape(QFrame::Box);
        DiskName->setFrameShadow(QFrame::Plain);
        DiskName->setLineWidth(1);
        DiskName->setAlignment(Qt::AlignBottom|Qt::AlignLeading|Qt::AlignLeft);

        horizontalLayout_3->addWidget(DiskName);

        horizontalSpacer_2 = new QSpacerItem(0, 10, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_3);

        D64FileTable = new QTableWidget(FloppyWindow);
        D64FileTable->setObjectName(QString::fromUtf8("D64FileTable"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(D64FileTable->sizePolicy().hasHeightForWidth());
        D64FileTable->setSizePolicy(sizePolicy1);
        QPalette palette;
        QBrush brush(QColor(0, 0, 100, 255));
        brush.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Active, QPalette::Base, brush);
        palette.setBrush(QPalette::Inactive, QPalette::Base, brush);
        QBrush brush1(QColor(255, 255, 255, 255));
        brush1.setStyle(Qt::SolidPattern);
        palette.setBrush(QPalette::Disabled, QPalette::Base, brush1);
        D64FileTable->setPalette(palette);
        D64FileTable->setFocusPolicy(Qt::NoFocus);
        D64FileTable->setSelectionMode(QAbstractItemView::SingleSelection);
        D64FileTable->setSelectionBehavior(QAbstractItemView::SelectRows);
        D64FileTable->setShowGrid(false);
        D64FileTable->verticalHeader()->setVisible(false);

        verticalLayout->addWidget(D64FileTable);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        D64FileTableBigSize = new QCheckBox(FloppyWindow);
        D64FileTableBigSize->setObjectName(QString::fromUtf8("D64FileTableBigSize"));

        horizontalLayout->addWidget(D64FileTableBigSize);

        horizontalSpacer_4 = new QSpacerItem(20, 20, QSizePolicy::Fixed, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_4);

        ViewSplatFiles = new QCheckBox(FloppyWindow);
        ViewSplatFiles->setObjectName(QString::fromUtf8("ViewSplatFiles"));

        horizontalLayout->addWidget(ViewSplatFiles);

        horizontalSpacer_3 = new QSpacerItem(40, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer_3);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_4->addLayout(verticalLayout);


        verticalLayout_2->addLayout(horizontalLayout_4);


        retranslateUi(FloppyWindow);

        QMetaObject::connectSlotsByName(FloppyWindow);
    } // setupUi

    void retranslateUi(QDialog *FloppyWindow)
    {
        FloppyWindow->setWindowTitle(QApplication::translate("FloppyWindow", "Floppy 1541 II (Diskeditor)", 0, QApplication::UnicodeUTF8));
        FloppySelect->clear();
        FloppySelect->insertItems(0, QStringList()
         << QApplication::translate("FloppyWindow", "Floppy 1541 #08", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FloppyWindow", "Floppy 1541 #09", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FloppyWindow", "Floppy 1541 #10", 0, QApplication::UnicodeUTF8)
         << QApplication::translate("FloppyWindow", "Floppy 1541 #11", 0, QApplication::UnicodeUTF8)
        );
        CreateNewD64->setText(QApplication::translate("FloppyWindow", "D64 Erstellen", 0, QApplication::UnicodeUTF8));
        DiskName->setText(QApplication::translate("FloppyWindow", "Name", 0, QApplication::UnicodeUTF8));
        D64FileTableBigSize->setText(QApplication::translate("FloppyWindow", "Blindenmodus", 0, QApplication::UnicodeUTF8));
        ViewSplatFiles->setText(QApplication::translate("FloppyWindow", "Gel\303\266schte Dateien (Splat-Files) anzeigen", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FloppyWindow: public Ui_FloppyWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FLOPPY_WINDOW_H
