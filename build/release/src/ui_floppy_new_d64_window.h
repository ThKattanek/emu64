/********************************************************************************
** Form generated from reading UI file 'floppy_new_d64_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_FLOPPY_NEW_D64_WINDOW_H
#define UI_FLOPPY_NEW_D64_WINDOW_H

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

QT_BEGIN_NAMESPACE

class Ui_FloppyNewD64Window
{
public:
    QVBoxLayout *verticalLayout;
    QVBoxLayout *verticalLayout_2;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QLineEdit *Filename;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *Diskname;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *DIskID;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *FloppyNewD64Window)
    {
        if (FloppyNewD64Window->objectName().isEmpty())
            FloppyNewD64Window->setObjectName(QString::fromUtf8("FloppyNewD64Window"));
        FloppyNewD64Window->resize(292, 123);
        verticalLayout = new QVBoxLayout(FloppyNewD64Window);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout_2 = new QVBoxLayout();
        verticalLayout_2->setObjectName(QString::fromUtf8("verticalLayout_2"));
        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_3 = new QLabel(FloppyNewD64Window);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(label_3->sizePolicy().hasHeightForWidth());
        label_3->setSizePolicy(sizePolicy);
        label_3->setMinimumSize(QSize(55, 0));

        horizontalLayout_4->addWidget(label_3);

        Filename = new QLineEdit(FloppyNewD64Window);
        Filename->setObjectName(QString::fromUtf8("Filename"));

        horizontalLayout_4->addWidget(Filename);


        verticalLayout_2->addLayout(horizontalLayout_4);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_2 = new QLabel(FloppyNewD64Window);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(55, 0));

        horizontalLayout_3->addWidget(label_2);

        Diskname = new QLineEdit(FloppyNewD64Window);
        Diskname->setObjectName(QString::fromUtf8("Diskname"));

        horizontalLayout_3->addWidget(Diskname);


        verticalLayout_2->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label = new QLabel(FloppyNewD64Window);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(55, 0));

        horizontalLayout_2->addWidget(label);

        DIskID = new QLineEdit(FloppyNewD64Window);
        DIskID->setObjectName(QString::fromUtf8("DIskID"));
        DIskID->setMaximumSize(QSize(30, 16777215));
        DIskID->setMaxLength(2);

        horizontalLayout_2->addWidget(DIskID);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_2->addItem(horizontalSpacer);


        verticalLayout_2->addLayout(horizontalLayout_2);


        verticalLayout->addLayout(verticalLayout_2);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        buttonBox = new QDialogButtonBox(FloppyNewD64Window);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        verticalLayout->addLayout(horizontalLayout);


        retranslateUi(FloppyNewD64Window);
        QObject::connect(buttonBox, SIGNAL(accepted()), FloppyNewD64Window, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), FloppyNewD64Window, SLOT(reject()));

        QMetaObject::connectSlotsByName(FloppyNewD64Window);
    } // setupUi

    void retranslateUi(QDialog *FloppyNewD64Window)
    {
        FloppyNewD64Window->setWindowTitle(QApplication::translate("FloppyNewD64Window", "Neues C64 Diskimage", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("FloppyNewD64Window", "Dateiname:", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("FloppyNewD64Window", "Diskname:", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("FloppyNewD64Window", "Disk ID:", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class FloppyNewD64Window: public Ui_FloppyNewD64Window {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_FLOPPY_NEW_D64_WINDOW_H
