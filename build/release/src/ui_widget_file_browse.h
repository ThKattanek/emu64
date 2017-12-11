/********************************************************************************
** Form generated from reading UI file 'widget_file_browse.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_WIDGET_FILE_BROWSE_H
#define UI_WIDGET_FILE_BROWSE_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QCheckBox>
#include <QtGui/QComboBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLineEdit>
#include <QtGui/QListView>
#include <QtGui/QListWidget>
#include <QtGui/QSpacerItem>
#include <QtGui/QToolButton>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_WidgetFileBrowse
{
public:
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout_2;
    QSpacerItem *horizontalSpacer;
    QLineEdit *AktPath;
    QHBoxLayout *horizontalLayout_4;
    QComboBox *drive_list;
    QToolButton *to_parent;
    QToolButton *to_home;
    QToolButton *view_refresh;
    QToolButton *delete_file;
    QSpacerItem *horizontalSpacer_2;
    QListView *listView_filebrowser;
    QListWidget *listWidget_zip;
    QHBoxLayout *horizontalLayout_3;
    QCheckBox *WriteProtected;

    void setupUi(QWidget *WidgetFileBrowse)
    {
        if (WidgetFileBrowse->objectName().isEmpty())
            WidgetFileBrowse->setObjectName(QString::fromUtf8("WidgetFileBrowse"));
        WidgetFileBrowse->resize(173, 360);
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        font.setPointSize(7);
        WidgetFileBrowse->setFont(font);
        WidgetFileBrowse->setWindowTitle(QString::fromUtf8("Form"));
        horizontalLayout = new QHBoxLayout(WidgetFileBrowse);
        horizontalLayout->setSpacing(0);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(2);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout->addLayout(horizontalLayout_2);

        AktPath = new QLineEdit(WidgetFileBrowse);
        AktPath->setObjectName(QString::fromUtf8("AktPath"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(AktPath->sizePolicy().hasHeightForWidth());
        AktPath->setSizePolicy(sizePolicy);
        AktPath->setReadOnly(true);

        verticalLayout->addWidget(AktPath);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        drive_list = new QComboBox(WidgetFileBrowse);
        drive_list->setObjectName(QString::fromUtf8("drive_list"));

        horizontalLayout_4->addWidget(drive_list);

        to_parent = new QToolButton(WidgetFileBrowse);
        to_parent->setObjectName(QString::fromUtf8("to_parent"));
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/grafik/go-up.png"), QSize(), QIcon::Normal, QIcon::Off);
        to_parent->setIcon(icon);
        to_parent->setIconSize(QSize(16, 16));

        horizontalLayout_4->addWidget(to_parent);

        to_home = new QToolButton(WidgetFileBrowse);
        to_home->setObjectName(QString::fromUtf8("to_home"));
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/grafik/go-home.png"), QSize(), QIcon::Normal, QIcon::Off);
        to_home->setIcon(icon1);
        to_home->setIconSize(QSize(16, 16));

        horizontalLayout_4->addWidget(to_home);

        view_refresh = new QToolButton(WidgetFileBrowse);
        view_refresh->setObjectName(QString::fromUtf8("view_refresh"));
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/grafik/view-refresh.png"), QSize(), QIcon::Normal, QIcon::Off);
        view_refresh->setIcon(icon2);
        view_refresh->setIconSize(QSize(16, 16));

        horizontalLayout_4->addWidget(view_refresh);

        delete_file = new QToolButton(WidgetFileBrowse);
        delete_file->setObjectName(QString::fromUtf8("delete_file"));
        QIcon icon3;
        icon3.addFile(QString::fromUtf8(":/grafik/delete.png"), QSize(), QIcon::Normal, QIcon::Off);
        delete_file->setIcon(icon3);
        delete_file->setIconSize(QSize(16, 16));

        horizontalLayout_4->addWidget(delete_file);

        horizontalSpacer_2 = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_4->addItem(horizontalSpacer_2);


        verticalLayout->addLayout(horizontalLayout_4);

        listView_filebrowser = new QListView(WidgetFileBrowse);
        listView_filebrowser->setObjectName(QString::fromUtf8("listView_filebrowser"));

        verticalLayout->addWidget(listView_filebrowser);

        listWidget_zip = new QListWidget(WidgetFileBrowse);
        listWidget_zip->setObjectName(QString::fromUtf8("listWidget_zip"));
        listWidget_zip->setMinimumSize(QSize(0, 100));
        listWidget_zip->setMaximumSize(QSize(16777215, 100));

        verticalLayout->addWidget(listWidget_zip);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        WriteProtected = new QCheckBox(WidgetFileBrowse);
        WriteProtected->setObjectName(QString::fromUtf8("WriteProtected"));

        horizontalLayout_3->addWidget(WriteProtected);


        verticalLayout->addLayout(horizontalLayout_3);


        horizontalLayout->addLayout(verticalLayout);


        retranslateUi(WidgetFileBrowse);

        QMetaObject::connectSlotsByName(WidgetFileBrowse);
    } // setupUi

    void retranslateUi(QWidget *WidgetFileBrowse)
    {
#ifndef QT_NO_TOOLTIP
        to_parent->setToolTip(QApplication::translate("WidgetFileBrowse", "Eine Verzeichnisebene h\303\266her", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        to_parent->setText(QApplication::translate("WidgetFileBrowse", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        to_home->setToolTip(QApplication::translate("WidgetFileBrowse", "Wechselt in das Homeverzeichnis", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        to_home->setText(QApplication::translate("WidgetFileBrowse", "Home", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        view_refresh->setToolTip(QApplication::translate("WidgetFileBrowse", "Aktualisieren", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        view_refresh->setText(QApplication::translate("WidgetFileBrowse", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        delete_file->setToolTip(QApplication::translate("WidgetFileBrowse", "Datei l\303\266schen", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        delete_file->setText(QApplication::translate("WidgetFileBrowse", "...", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        WriteProtected->setToolTip(QApplication::translate("WidgetFileBrowse", "Setzt den Schreibschutz der Datei.\n"
"Ein nachtr\303\244gliches setzen verhindert das \303\274berschreiben des Images.", 0, QApplication::UnicodeUTF8));
#endif // QT_NO_TOOLTIP
        WriteProtected->setText(QApplication::translate("WidgetFileBrowse", "Disk Schreibschutz", 0, QApplication::UnicodeUTF8));
        Q_UNUSED(WidgetFileBrowse);
    } // retranslateUi

};

namespace Ui {
    class WidgetFileBrowse: public Ui_WidgetFileBrowse {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_WIDGET_FILE_BROWSE_H
