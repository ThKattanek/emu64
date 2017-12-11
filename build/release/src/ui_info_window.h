/********************************************************************************
** Form generated from reading UI file 'info_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_INFO_WINDOW_H
#define UI_INFO_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QListWidget>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_InfoWindow
{
public:
    QWidget *widget;
    QWidget *widget_2;
    QLabel *moreinfosOut;
    QLabel *Emu64Version;
    QLabel *label_2;
    QLabel *label_3;
    QListWidget *listWidget;
    QLabel *label;
    QWidget *widget_3;
    QWidget *widget_4;
    QLabel *label_4;
    QLabel *label_5;

    void setupUi(QDialog *InfoWindow)
    {
        if (InfoWindow->objectName().isEmpty())
            InfoWindow->setObjectName(QString::fromUtf8("InfoWindow"));
        InfoWindow->resize(371, 507);
        widget = new QWidget(InfoWindow);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(188, 56, 106, 76));
        widget->setAutoFillBackground(false);
        widget->setStyleSheet(QString::fromUtf8("border-image: url(:/grafik/thorsten.png);\n"
"\n"
""));
        widget_2 = new QWidget(InfoWindow);
        widget_2->setObjectName(QString::fromUtf8("widget_2"));
        widget_2->setGeometry(QRect(5, 285, 361, 200));
        widget_2->setStyleSheet(QString::fromUtf8("border-image: url(:/grafik/qt_sdl_ffmpeg_logo_300x200.png);"));
        moreinfosOut = new QLabel(InfoWindow);
        moreinfosOut->setObjectName(QString::fromUtf8("moreinfosOut"));
        moreinfosOut->setGeometry(QRect(5, 487, 351, 17));
        QFont font;
        font.setPointSize(8);
        moreinfosOut->setFont(font);
        Emu64Version = new QLabel(InfoWindow);
        Emu64Version->setObjectName(QString::fromUtf8("Emu64Version"));
        Emu64Version->setGeometry(QRect(130, 4, 211, 25));
        QFont font1;
        font1.setPointSize(10);
        font1.setBold(true);
        font1.setWeight(75);
        Emu64Version->setFont(font1);
        Emu64Version->setStyleSheet(QString::fromUtf8("color: rgb(0, 0,100);"));
        Emu64Version->setAlignment(Qt::AlignCenter);
        label_2 = new QLabel(InfoWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(152, 24, 177, 16));
        label_2->setFont(font);
        label_2->setAlignment(Qt::AlignCenter);
        label_2->setOpenExternalLinks(false);
        label_2->setTextInteractionFlags(Qt::LinksAccessibleByMouse);
        label_3 = new QLabel(InfoWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setGeometry(QRect(152, 36, 177, 16));
        label_3->setFont(font);
        label_3->setAlignment(Qt::AlignCenter);
        label_3->setOpenExternalLinks(true);
        listWidget = new QListWidget(InfoWindow);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        new QListWidgetItem(listWidget);
        listWidget->setObjectName(QString::fromUtf8("listWidget"));
        listWidget->setGeometry(QRect(4, 144, 361, 81));
        QFont font2;
        font2.setPointSize(7);
        listWidget->setFont(font2);
        listWidget->setSelectionMode(QAbstractItemView::NoSelection);
        label = new QLabel(InfoWindow);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(4, 128, 165, 16));
        QFont font3;
        font3.setBold(true);
        font3.setWeight(75);
        label->setFont(font3);
        widget_3 = new QWidget(InfoWindow);
        widget_3->setObjectName(QString::fromUtf8("widget_3"));
        widget_3->setGeometry(QRect(4, 4, 120, 120));
        widget_3->setStyleSheet(QString::fromUtf8("border-image: url(:/grafik/emu64_logo_120_120.png);"));
        widget_4 = new QWidget(InfoWindow);
        widget_4->setObjectName(QString::fromUtf8("widget_4"));
        widget_4->setGeometry(QRect(5, 230, 108, 51));
        widget_4->setStyleSheet(QString::fromUtf8("border-image: url(:/grafik/style64.png);"));
        label_4 = new QLabel(InfoWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(120, 245, 91, 16));
        label_4->setFont(font);
        label_4->setAlignment(Qt::AlignCenter);
        label_4->setOpenExternalLinks(true);
        label_5 = new QLabel(InfoWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(125, 230, 236, 16));

        retranslateUi(InfoWindow);

        QMetaObject::connectSlotsByName(InfoWindow);
    } // setupUi

    void retranslateUi(QDialog *InfoWindow)
    {
        InfoWindow->setWindowTitle(QApplication::translate("InfoWindow", "Emu64 Information", 0, QApplication::UnicodeUTF8));
        moreinfosOut->setText(QApplication::translate("InfoWindow", "INFO", 0, QApplication::UnicodeUTF8));
        Emu64Version->setText(QString());
        label_2->setText(QApplication::translate("InfoWindow", "von Thorsten Kattanek", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("InfoWindow", "<html><head/><body><p><a href=\"http://www.emu64-projekt.de\"><span style=\" text-decoration: underline; color:#0057ae;\">www.emu64-projekt.de</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));

        const bool __sortingEnabled = listWidget->isSortingEnabled();
        listWidget->setSortingEnabled(false);
        QListWidgetItem *___qlistwidgetitem = listWidget->item(0);
        ___qlistwidgetitem->setText(QApplication::translate("InfoWindow", "Thomas Daden (+ Emu64 Handbuch, Emu64-Wiki, Emu64 Webspace)", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem1 = listWidget->item(1);
        ___qlistwidgetitem1->setText(QApplication::translate("InfoWindow", "Selmiak (+ Emu64 Logo, engl. \303\234bersetzung, Emu64-Wiki)", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem2 = listWidget->item(2);
        ___qlistwidgetitem2->setText(QApplication::translate("InfoWindow", "Tobias Kattanek (engl. \303\234bersetzung)", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem3 = listWidget->item(3);
        ___qlistwidgetitem3->setText(QApplication::translate("InfoWindow", "Peggy", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem4 = listWidget->item(4);
        ___qlistwidgetitem4->setText(QApplication::translate("InfoWindow", "Andreas Goller", 0, QApplication::UnicodeUTF8));
        QListWidgetItem *___qlistwidgetitem5 = listWidget->item(5);
        ___qlistwidgetitem5->setText(QApplication::translate("InfoWindow", "Hoeppi", 0, QApplication::UnicodeUTF8));
        listWidget->setSortingEnabled(__sortingEnabled);

        label->setText(QApplication::translate("InfoWindow", "Besonderen Dank an ...", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("InfoWindow", "<html><head/><body><p><a href=\"http://www.style64.org/\"><span style=\" text-decoration: underline; color:#0057ae;\">www.style64.org/</span></a></p></body></html>", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("InfoWindow", "Emu64 benutzt C64-Pro-Style TTF von Style64", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class InfoWindow: public Ui_InfoWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_INFO_WINDOW_H
