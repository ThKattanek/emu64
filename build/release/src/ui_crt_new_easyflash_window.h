/********************************************************************************
** Form generated from reading UI file 'crt_new_easyflash_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CRT_NEW_EASYFLASH_WINDOW_H
#define UI_CRT_NEW_EASYFLASH_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_CRTNewEasyflashWindow
{
public:
    QVBoxLayout *verticalLayout;
    QWidget *widget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *Filename;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *CRTName;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *CRTNewEasyflashWindow)
    {
        if (CRTNewEasyflashWindow->objectName().isEmpty())
            CRTNewEasyflashWindow->setObjectName(QString::fromUtf8("CRTNewEasyflashWindow"));
        CRTNewEasyflashWindow->resize(277, 187);
        verticalLayout = new QVBoxLayout(CRTNewEasyflashWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        widget = new QWidget(CRTNewEasyflashWindow);
        widget->setObjectName(QString::fromUtf8("widget"));
        QSizePolicy sizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(widget->sizePolicy().hasHeightForWidth());
        widget->setSizePolicy(sizePolicy);
        widget->setMinimumSize(QSize(160, 50));
        widget->setStyleSheet(QString::fromUtf8("border-image: url(:/grafik/easy_flash_logo.png);"));

        verticalLayout->addWidget(widget);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(CRTNewEasyflashWindow);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        Filename = new QLineEdit(CRTNewEasyflashWindow);
        Filename->setObjectName(QString::fromUtf8("Filename"));

        horizontalLayout->addWidget(Filename);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(CRTNewEasyflashWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));

        horizontalLayout_2->addWidget(label_2);

        CRTName = new QLineEdit(CRTNewEasyflashWindow);
        CRTName->setObjectName(QString::fromUtf8("CRTName"));

        horizontalLayout_2->addWidget(CRTName);


        verticalLayout->addLayout(horizontalLayout_2);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(CRTNewEasyflashWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(CRTNewEasyflashWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), CRTNewEasyflashWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), CRTNewEasyflashWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(CRTNewEasyflashWindow);
    } // setupUi

    void retranslateUi(QDialog *CRTNewEasyflashWindow)
    {
        CRTNewEasyflashWindow->setWindowTitle(QApplication::translate("CRTNewEasyflashWindow", "Neues EasyFlash Image", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("CRTNewEasyflashWindow", "Dateiname:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("CRTNewEasyflashWindow", "CRT Name:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class CRTNewEasyflashWindow: public Ui_CRTNewEasyflashWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CRT_NEW_EASYFLASH_WINDOW_H
