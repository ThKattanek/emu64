/********************************************************************************
** Form generated from reading UI file 'crt_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRT_WINDOW_H
#define UI_CRT_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QGroupBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStackedWidget>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>
#include "tree_widget_mod.h"
#include "widget_file_browse.h"

QT_BEGIN_NAMESPACE

class Ui_CrtWindow
{
public:
    QHBoxLayout *horizontalLayout_4;
    QVBoxLayout *verticalLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout_2;
    QVBoxLayout *verticalLayout;
    WidgetFileBrowse *FileBrowser;
    QHBoxLayout *horizontalLayout;
    QPushButton *InsertCRT;
    QPushButton *RemoveCRT;
    QPushButton *ViewChipData;
    QSpacerItem *horizontalSpacer;
    QVBoxLayout *verticalLayout_2;
    TreeWidgetMod *CRTInfo;
    TreeWidgetMod *ChipList;
    QVBoxLayout *verticalLayout_8;
    QStackedWidget *MoreCRTPage;
    QWidget *PageEasyFlash;
    QWidget *EF_Logo;
    QGroupBox *groupBox_2;
    QRadioButton *EF_JUMPER0;
    QRadioButton *EF_JUMPER1;
    QPushButton *pushButton_2;
    QPushButton *pushButton_3;
    QGroupBox *groupBox_3;
    QLabel *label_2;
    QLabel *label_3;
    QPushButton *EF_LED;
    QWidget *PageFC;
    QPushButton *FreezButtonFC3;
    QWidget *FC3_Logo;
    QGroupBox *groupBox;
    QLabel *FC3WikiLink;
    QPushButton *FC3_LED;
    QWidget *PageAR;
    QPushButton *FreezButtonAR;
    QWidget *FC3_Logo_2;
    TreeWidgetMod *ChipData;
    QPushButton *NewEasyFlashCRT;

    void setupUi(QDialog *CrtWindow)
    {
        if (CrtWindow->objectName().isEmpty())
            CrtWindow->setObjectName(QString::fromUtf8("CrtWindow"));
        CrtWindow->resize(687, 315);
        CrtWindow->setMinimumSize(QSize(480, 0));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        CrtWindow->setFont(font);
        horizontalLayout_4 = new QHBoxLayout(CrtWindow);
        horizontalLayout_4->setSpacing(2);
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        horizontalLayout_4->setContentsMargins(2, 2, 2, 2);
        verticalLayout_4 = new QVBoxLayout();
        verticalLayout_4->setSpacing(2);
        verticalLayout_4->setObjectName(QString::fromUtf8("verticalLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(0);
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        verticalLayout_3 = new QVBoxLayout();
        verticalLayout_3->setSpacing(0);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(2);
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(0);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        FileBrowser = new WidgetFileBrowse(CrtWindow);
        FileBrowser->setObjectName(QString::fromUtf8("FileBrowser"));
        FileBrowser->setMinimumSize(QSize(0, 0));
        QFont font1;
        font1.setPointSize(8);
        FileBrowser->setFont(font1);

        verticalLayout->addWidget(FileBrowser);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setSizeConstraint(QLayout::SetDefaultConstraint);
        InsertCRT = new QPushButton(CrtWindow);
        InsertCRT->setObjectName(QString::fromUtf8("InsertCRT"));
        InsertCRT->setMinimumSize(QSize(100, 23));
        InsertCRT->setMaximumSize(QSize(100, 23));
        InsertCRT->setFont(font1);

        horizontalLayout->addWidget(InsertCRT);

        RemoveCRT = new QPushButton(CrtWindow);
        RemoveCRT->setObjectName(QString::fromUtf8("RemoveCRT"));
        RemoveCRT->setMinimumSize(QSize(110, 23));
        RemoveCRT->setMaximumSize(QSize(110, 23));
        RemoveCRT->setFont(font1);

        horizontalLayout->addWidget(RemoveCRT);

        ViewChipData = new QPushButton(CrtWindow);
        ViewChipData->setObjectName(QString::fromUtf8("ViewChipData"));
        ViewChipData->setMinimumSize(QSize(22, 23));
        ViewChipData->setMaximumSize(QSize(22, 23));
        ViewChipData->setFont(font1);
        ViewChipData->setText(QString::fromUtf8("+"));

        horizontalLayout->addWidget(ViewChipData);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout);


        horizontalLayout_2->addLayout(verticalLayout);

        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setSpacing(2);
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        CRTInfo = new TreeWidgetMod(CrtWindow);
        QFont font2;
        font2.setFamily(QString::fromUtf8("Arial"));
        font2.setPointSize(8);
        font2.setBold(true);
        font2.setWeight(75);
        QTreeWidgetItem *__qtreewidgetitem = new QTreeWidgetItem();
        __qtreewidgetitem->setFont(1, font2);
        __qtreewidgetitem->setFont(0, font2);
        CRTInfo->setHeaderItem(__qtreewidgetitem);
        new QTreeWidgetItem(CRTInfo);
        new QTreeWidgetItem(CRTInfo);
        new QTreeWidgetItem(CRTInfo);
        new QTreeWidgetItem(CRTInfo);
        new QTreeWidgetItem(CRTInfo);
        new QTreeWidgetItem(CRTInfo);
        CRTInfo->setObjectName(QString::fromUtf8("CRTInfo"));
        CRTInfo->setMinimumSize(QSize(239, 106));
        CRTInfo->setMaximumSize(QSize(239, 106));
        QFont font3;
        font3.setFamily(QString::fromUtf8("Arial"));
        font3.setPointSize(8);
        CRTInfo->setFont(font3);
        CRTInfo->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 100);"));
        CRTInfo->setIndentation(0);
        CRTInfo->setItemsExpandable(true);

        verticalLayout_2->addWidget(CRTInfo);

        ChipList = new TreeWidgetMod(CrtWindow);
        QTreeWidgetItem *__qtreewidgetitem1 = new QTreeWidgetItem();
        __qtreewidgetitem1->setFont(4, font2);
        __qtreewidgetitem1->setFont(3, font2);
        __qtreewidgetitem1->setFont(2, font2);
        __qtreewidgetitem1->setFont(1, font2);
        __qtreewidgetitem1->setFont(0, font2);
        ChipList->setHeaderItem(__qtreewidgetitem1);
        ChipList->setObjectName(QString::fromUtf8("ChipList"));
        ChipList->setMinimumSize(QSize(239, 0));
        ChipList->setMaximumSize(QSize(240, 16777215));
        ChipList->setFont(font3);
        ChipList->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 100);"));
        ChipList->setIndentation(0);

        verticalLayout_2->addWidget(ChipList);


        horizontalLayout_2->addLayout(verticalLayout_2);

        verticalLayout_8 = new QVBoxLayout();
        verticalLayout_8->setSpacing(2);
        verticalLayout_8->setObjectName(QString::fromUtf8("verticalLayout_8"));
        MoreCRTPage = new QStackedWidget(CrtWindow);
        MoreCRTPage->setObjectName(QString::fromUtf8("MoreCRTPage"));
        MoreCRTPage->setEnabled(true);
        MoreCRTPage->setMinimumSize(QSize(220, 0));
        MoreCRTPage->setMaximumSize(QSize(0, 16777215));
        MoreCRTPage->setFont(font);
        MoreCRTPage->setStyleSheet(QString::fromUtf8(""));
        PageEasyFlash = new QWidget();
        PageEasyFlash->setObjectName(QString::fromUtf8("PageEasyFlash"));
        PageEasyFlash->setEnabled(true);
        PageEasyFlash->setMinimumSize(QSize(0, 0));
        EF_Logo = new QWidget(PageEasyFlash);
        EF_Logo->setObjectName(QString::fromUtf8("EF_Logo"));
        EF_Logo->setEnabled(false);
        EF_Logo->setGeometry(QRect(15, 5, 161, 51));
        EF_Logo->setStyleSheet(QString::fromUtf8("image: url(:/grafik/easy_flash_logo.png);"));
        groupBox_2 = new QGroupBox(PageEasyFlash);
        groupBox_2->setObjectName(QString::fromUtf8("groupBox_2"));
        groupBox_2->setEnabled(true);
        groupBox_2->setGeometry(QRect(16, 81, 161, 71));
        EF_JUMPER0 = new QRadioButton(groupBox_2);
        EF_JUMPER0->setObjectName(QString::fromUtf8("EF_JUMPER0"));
        EF_JUMPER0->setEnabled(true);
        EF_JUMPER0->setGeometry(QRect(10, 25, 141, 21));
        EF_JUMPER0->setChecked(true);
        EF_JUMPER1 = new QRadioButton(groupBox_2);
        EF_JUMPER1->setObjectName(QString::fromUtf8("EF_JUMPER1"));
        EF_JUMPER1->setEnabled(true);
        EF_JUMPER1->setGeometry(QRect(10, 45, 141, 21));
        pushButton_2 = new QPushButton(PageEasyFlash);
        pushButton_2->setObjectName(QString::fromUtf8("pushButton_2"));
        pushButton_2->setEnabled(false);
        pushButton_2->setGeometry(QRect(16, 156, 161, 23));
        pushButton_3 = new QPushButton(PageEasyFlash);
        pushButton_3->setObjectName(QString::fromUtf8("pushButton_3"));
        pushButton_3->setEnabled(false);
        pushButton_3->setGeometry(QRect(16, 181, 161, 23));
        groupBox_3 = new QGroupBox(PageEasyFlash);
        groupBox_3->setObjectName(QString::fromUtf8("groupBox_3"));
        groupBox_3->setGeometry(QRect(15, 205, 161, 61));
        label_2 = new QLabel(groupBox_3);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(15, 30, 106, 16));
        QFont font4;
        font4.setUnderline(true);
        label_2->setFont(font4);
        label_2->setOpenExternalLinks(true);
        label_3 = new QLabel(PageEasyFlash);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(20, 60, 66, 16));
        EF_LED = new QPushButton(PageEasyFlash);
        EF_LED->setObjectName(QString::fromUtf8("EF_LED"));
        EF_LED->setGeometry(QRect(100, 62, 12, 12));
        MoreCRTPage->addWidget(PageEasyFlash);
        PageFC = new QWidget();
        PageFC->setObjectName(QString::fromUtf8("PageFC"));
        PageFC->setEnabled(true);
        PageFC->setMinimumSize(QSize(0, 0));
        FreezButtonFC3 = new QPushButton(PageFC);
        FreezButtonFC3->setObjectName(QString::fromUtf8("FreezButtonFC3"));
        FreezButtonFC3->setGeometry(QRect(15, 60, 61, 23));
        FC3_Logo = new QWidget(PageFC);
        FC3_Logo->setObjectName(QString::fromUtf8("FC3_Logo"));
        FC3_Logo->setGeometry(QRect(5, 0, 180, 46));
        FC3_Logo->setStyleSheet(QString::fromUtf8("image: url(:/grafik/fc3_logo.png);"));
        groupBox = new QGroupBox(PageFC);
        groupBox->setObjectName(QString::fromUtf8("groupBox"));
        groupBox->setGeometry(QRect(5, 95, 186, 56));
        FC3WikiLink = new QLabel(groupBox);
        FC3WikiLink->setObjectName(QString::fromUtf8("FC3WikiLink"));
        FC3WikiLink->setGeometry(QRect(15, 30, 106, 16));
        FC3WikiLink->setFont(font4);
        FC3WikiLink->setOpenExternalLinks(true);
        FC3_LED = new QPushButton(PageFC);
        FC3_LED->setObjectName(QString::fromUtf8("FC3_LED"));
        FC3_LED->setGeometry(QRect(155, 50, 12, 12));
        MoreCRTPage->addWidget(PageFC);
        PageAR = new QWidget();
        PageAR->setObjectName(QString::fromUtf8("PageAR"));
        PageAR->setEnabled(true);
        FreezButtonAR = new QPushButton(PageAR);
        FreezButtonAR->setObjectName(QString::fromUtf8("FreezButtonAR"));
        FreezButtonAR->setGeometry(QRect(20, 70, 61, 23));
        FC3_Logo_2 = new QWidget(PageAR);
        FC3_Logo_2->setObjectName(QString::fromUtf8("FC3_Logo_2"));
        FC3_Logo_2->setGeometry(QRect(10, 5, 180, 51));
        FC3_Logo_2->setStyleSheet(QString::fromUtf8("image: url(:/grafik/action_replay_logo.png);"));
        MoreCRTPage->addWidget(PageAR);

        verticalLayout_8->addWidget(MoreCRTPage);


        horizontalLayout_2->addLayout(verticalLayout_8);


        verticalLayout_3->addLayout(horizontalLayout_2);

        ChipData = new TreeWidgetMod(CrtWindow);
        QTreeWidgetItem *__qtreewidgetitem2 = new QTreeWidgetItem();
        __qtreewidgetitem2->setText(0, QString::fromUtf8("1"));
        ChipData->setHeaderItem(__qtreewidgetitem2);
        ChipData->setObjectName(QString::fromUtf8("ChipData"));
        ChipData->setEnabled(true);
        ChipData->setMinimumSize(QSize(457, 0));
        ChipData->setMaximumSize(QSize(16777215, 0));
        QFont font5;
        font5.setFamily(QString::fromUtf8("Arial"));
        font5.setPointSize(7);
        ChipData->setFont(font5);
        ChipData->setStyleSheet(QString::fromUtf8("color: rgb(0, 0, 127);"));
        ChipData->setIndentation(0);

        verticalLayout_3->addWidget(ChipData);


        horizontalLayout_3->addLayout(verticalLayout_3);


        verticalLayout_4->addLayout(horizontalLayout_3);

        NewEasyFlashCRT = new QPushButton(CrtWindow);
        NewEasyFlashCRT->setObjectName(QString::fromUtf8("NewEasyFlashCRT"));
        NewEasyFlashCRT->setMinimumSize(QSize(160, 0));
        NewEasyFlashCRT->setMaximumSize(QSize(150, 16777215));
        NewEasyFlashCRT->setFont(font3);
        NewEasyFlashCRT->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        verticalLayout_4->addWidget(NewEasyFlashCRT);


        horizontalLayout_4->addLayout(verticalLayout_4);


        retranslateUi(CrtWindow);

        MoreCRTPage->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(CrtWindow);
    } // setupUi

    void retranslateUi(QDialog *CrtWindow)
    {
        CrtWindow->setWindowTitle(QApplication::translate("CrtWindow", "Steckmodule", 0, QApplication::UnicodeUTF8));
        InsertCRT->setText(QApplication::translate("CrtWindow", "Modul aktivieren", 0, QApplication::UnicodeUTF8));
        RemoveCRT->setText(QApplication::translate("CrtWindow", "Modul deaktivieren", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem = CRTInfo->headerItem();
        ___qtreewidgetitem->setText(1, QApplication::translate("CrtWindow", "Inhalt", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem->setText(0, QApplication::translate("CrtWindow", "Variable", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = CRTInfo->isSortingEnabled();
        CRTInfo->setSortingEnabled(false);
        QTreeWidgetItem *___qtreewidgetitem1 = CRTInfo->topLevelItem(0);
        ___qtreewidgetitem1->setText(0, QApplication::translate("CrtWindow", "Name", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem2 = CRTInfo->topLevelItem(1);
        ___qtreewidgetitem2->setText(0, QApplication::translate("CrtWindow", "Version", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem3 = CRTInfo->topLevelItem(2);
        ___qtreewidgetitem3->setText(0, QApplication::translate("CrtWindow", "Hardware Typ", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem4 = CRTInfo->topLevelItem(3);
        ___qtreewidgetitem4->setText(0, QApplication::translate("CrtWindow", "EXPROM Leitung", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem5 = CRTInfo->topLevelItem(4);
        ___qtreewidgetitem5->setText(0, QApplication::translate("CrtWindow", "GAME Leitung", 0, QApplication::UnicodeUTF8));
        QTreeWidgetItem *___qtreewidgetitem6 = CRTInfo->topLevelItem(5);
        ___qtreewidgetitem6->setText(0, QApplication::translate("CrtWindow", "Chip Anzahl", 0, QApplication::UnicodeUTF8));
        CRTInfo->setSortingEnabled(__sortingEnabled);

        QTreeWidgetItem *___qtreewidgetitem7 = ChipList->headerItem();
        ___qtreewidgetitem7->setText(4, QApplication::translate("CrtWindow", "Gr\303\266\303\237e", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem7->setText(3, QApplication::translate("CrtWindow", "Adresse", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem7->setText(2, QApplication::translate("CrtWindow", "Bank", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem7->setText(1, QApplication::translate("CrtWindow", "Chip Typ", 0, QApplication::UnicodeUTF8));
        ___qtreewidgetitem7->setText(0, QApplication::translate("CrtWindow", "Nr.", 0, QApplication::UnicodeUTF8));
        groupBox_2->setTitle(QApplication::translate("CrtWindow", "Boot Schalter", 0, QApplication::UnicodeUTF8));
        EF_JUMPER0->setText(QApplication::translate("CrtWindow", "Modul starten", 0, QApplication::UnicodeUTF8));
        EF_JUMPER1->setText(QApplication::translate("CrtWindow", "C64 Normal starten", 0, QApplication::UnicodeUTF8));
        pushButton_2->setText(QApplication::translate("CrtWindow", "Flashspeicher Anzeigen", 0, QApplication::UnicodeUTF8));
        pushButton_3->setText(QApplication::translate("CrtWindow", "EasyFlash CRT speichern", 0, QApplication::UnicodeUTF8));
        groupBox_3->setTitle(QApplication::translate("CrtWindow", "Weitere Infos", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("CrtWindow", "<html> <a style = 'text-decoration:none'href ='http://www.c64-wiki.de/index.php/EasyFlash'>Artikel im C64 Wiki</a></html>", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("CrtWindow", "Status LED", 0, QApplication::UnicodeUTF8));
        EF_LED->setText(QString());
        FreezButtonFC3->setText(QApplication::translate("CrtWindow", "FREEZE", 0, QApplication::UnicodeUTF8));
        groupBox->setTitle(QApplication::translate("CrtWindow", "Weitere Infos", 0, QApplication::UnicodeUTF8));
        FC3WikiLink->setText(QApplication::translate("CrtWindow", "<html> <a style = 'text-decoration:none'href ='http://www.c64-wiki.de/index.php/Final_Cartridge_III'>Artikel im C64 Wiki</a></html>", 0, QApplication::UnicodeUTF8));
        FC3_LED->setText(QString());
        FreezButtonAR->setText(QApplication::translate("CrtWindow", "FREEZE", 0, QApplication::UnicodeUTF8));
        NewEasyFlashCRT->setText(QApplication::translate("CrtWindow", "Leeres EasyFlash erstellen", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CrtWindow: public Ui_CrtWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRT_WINDOW_H
