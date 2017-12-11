/********************************************************************************
** Form generated from reading UI file 'new_romset_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_NEW_ROMSET_WINDOW_H
#define UI_NEW_ROMSET_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFrame>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QVBoxLayout>

QT_BEGIN_NAMESPACE

class Ui_NewRomSetWindow
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *RomSetName;
    QLabel *ExistInfo;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *kernal_rom;
    QLabel *KernalSizeError;
    QPushButton *sel_kernal_rom;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *basic_rom;
    QLabel *BasicSizeError;
    QPushButton *sel_basic_rom;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *char_rom;
    QLabel *CharSizeError;
    QPushButton *sel_char_rom;
    QHBoxLayout *horizontalLayout_5;
    QLabel *label_5;
    QLineEdit *dos1514_rom;
    QLabel *DosSizeError;
    QPushButton *sel_dos1541_rom;
    QFrame *line;
    QLabel *label_6;
    QSpacerItem *verticalSpacer;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *NewRomSetWindow)
    {
        if (NewRomSetWindow->objectName().isEmpty())
            NewRomSetWindow->setObjectName(QString::fromUtf8("NewRomSetWindow"));
        NewRomSetWindow->resize(609, 246);
        verticalLayout = new QVBoxLayout(NewRomSetWindow);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(NewRomSetWindow);
        label->setObjectName(QString::fromUtf8("label"));
        label->setMinimumSize(QSize(90, 0));
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        label->setFont(font);

        horizontalLayout->addWidget(label);

        RomSetName = new QLineEdit(NewRomSetWindow);
        RomSetName->setObjectName(QString::fromUtf8("RomSetName"));

        horizontalLayout->addWidget(RomSetName);

        ExistInfo = new QLabel(NewRomSetWindow);
        ExistInfo->setObjectName(QString::fromUtf8("ExistInfo"));
        QFont font1;
        font1.setBold(true);
        font1.setWeight(75);
        ExistInfo->setFont(font1);
        ExistInfo->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        horizontalLayout->addWidget(ExistInfo);


        verticalLayout->addLayout(horizontalLayout);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        label_2 = new QLabel(NewRomSetWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setMinimumSize(QSize(90, 0));

        horizontalLayout_2->addWidget(label_2);

        kernal_rom = new QLineEdit(NewRomSetWindow);
        kernal_rom->setObjectName(QString::fromUtf8("kernal_rom"));
        kernal_rom->setEnabled(true);
        kernal_rom->setReadOnly(true);

        horizontalLayout_2->addWidget(kernal_rom);

        KernalSizeError = new QLabel(NewRomSetWindow);
        KernalSizeError->setObjectName(QString::fromUtf8("KernalSizeError"));
        KernalSizeError->setFont(font1);
        KernalSizeError->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        horizontalLayout_2->addWidget(KernalSizeError);

        sel_kernal_rom = new QPushButton(NewRomSetWindow);
        sel_kernal_rom->setObjectName(QString::fromUtf8("sel_kernal_rom"));

        horizontalLayout_2->addWidget(sel_kernal_rom);


        verticalLayout->addLayout(horizontalLayout_2);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label_3 = new QLabel(NewRomSetWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setMinimumSize(QSize(90, 0));

        horizontalLayout_3->addWidget(label_3);

        basic_rom = new QLineEdit(NewRomSetWindow);
        basic_rom->setObjectName(QString::fromUtf8("basic_rom"));
        basic_rom->setReadOnly(true);

        horizontalLayout_3->addWidget(basic_rom);

        BasicSizeError = new QLabel(NewRomSetWindow);
        BasicSizeError->setObjectName(QString::fromUtf8("BasicSizeError"));
        BasicSizeError->setFont(font1);
        BasicSizeError->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        horizontalLayout_3->addWidget(BasicSizeError);

        sel_basic_rom = new QPushButton(NewRomSetWindow);
        sel_basic_rom->setObjectName(QString::fromUtf8("sel_basic_rom"));

        horizontalLayout_3->addWidget(sel_basic_rom);


        verticalLayout->addLayout(horizontalLayout_3);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setObjectName(QString::fromUtf8("horizontalLayout_4"));
        label_4 = new QLabel(NewRomSetWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setMinimumSize(QSize(90, 0));

        horizontalLayout_4->addWidget(label_4);

        char_rom = new QLineEdit(NewRomSetWindow);
        char_rom->setObjectName(QString::fromUtf8("char_rom"));
        char_rom->setReadOnly(true);

        horizontalLayout_4->addWidget(char_rom);

        CharSizeError = new QLabel(NewRomSetWindow);
        CharSizeError->setObjectName(QString::fromUtf8("CharSizeError"));
        CharSizeError->setFont(font1);
        CharSizeError->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        horizontalLayout_4->addWidget(CharSizeError);

        sel_char_rom = new QPushButton(NewRomSetWindow);
        sel_char_rom->setObjectName(QString::fromUtf8("sel_char_rom"));

        horizontalLayout_4->addWidget(sel_char_rom);


        verticalLayout->addLayout(horizontalLayout_4);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setObjectName(QString::fromUtf8("horizontalLayout_5"));
        label_5 = new QLabel(NewRomSetWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setMinimumSize(QSize(90, 0));

        horizontalLayout_5->addWidget(label_5);

        dos1514_rom = new QLineEdit(NewRomSetWindow);
        dos1514_rom->setObjectName(QString::fromUtf8("dos1514_rom"));
        dos1514_rom->setReadOnly(true);

        horizontalLayout_5->addWidget(dos1514_rom);

        DosSizeError = new QLabel(NewRomSetWindow);
        DosSizeError->setObjectName(QString::fromUtf8("DosSizeError"));
        DosSizeError->setFont(font1);
        DosSizeError->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        horizontalLayout_5->addWidget(DosSizeError);

        sel_dos1541_rom = new QPushButton(NewRomSetWindow);
        sel_dos1541_rom->setObjectName(QString::fromUtf8("sel_dos1541_rom"));

        horizontalLayout_5->addWidget(sel_dos1541_rom);


        verticalLayout->addLayout(horizontalLayout_5);

        line = new QFrame(NewRomSetWindow);
        line->setObjectName(QString::fromUtf8("line"));
        line->setFrameShape(QFrame::HLine);
        line->setFrameShadow(QFrame::Sunken);

        verticalLayout->addWidget(line);

        label_6 = new QLabel(NewRomSetWindow);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));

        verticalLayout->addWidget(label_6);

        verticalSpacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Fixed);

        verticalLayout->addItem(verticalSpacer);

        buttonBox = new QDialogButtonBox(NewRomSetWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        buttonBox->setCenterButtons(false);

        verticalLayout->addWidget(buttonBox);


        retranslateUi(NewRomSetWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), NewRomSetWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), NewRomSetWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(NewRomSetWindow);
    } // setupUi

    void retranslateUi(QDialog *NewRomSetWindow)
    {
        NewRomSetWindow->setWindowTitle(QApplication::translate("NewRomSetWindow", "Neues ROM Set erstellen ...", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("NewRomSetWindow", "Name des Set's", 0, QApplication::UnicodeUTF8));
        ExistInfo->setText(QString());
        label_2->setText(QApplication::translate("NewRomSetWindow", "Kernal Rom", 0, QApplication::UnicodeUTF8));
        KernalSizeError->setText(QString());
        sel_kernal_rom->setText(QApplication::translate("NewRomSetWindow", "...", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("NewRomSetWindow", "Basic Rom", 0, QApplication::UnicodeUTF8));
        BasicSizeError->setText(QString());
        sel_basic_rom->setText(QApplication::translate("NewRomSetWindow", "...", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("NewRomSetWindow", "Char Rom", 0, QApplication::UnicodeUTF8));
        CharSizeError->setText(QString());
        sel_char_rom->setText(QApplication::translate("NewRomSetWindow", "...", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("NewRomSetWindow", "1541 Dos Rom", 0, QApplication::UnicodeUTF8));
        DosSizeError->setText(QString());
        sel_dos1541_rom->setText(QApplication::translate("NewRomSetWindow", "...", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("NewRomSetWindow", "Es muss nicht zwingend ein ROM ausgew\303\244hlt werden. Ist ein Feld leer so wird das entsprechende Original ROM verwendet.", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class NewRomSetWindow: public Ui_NewRomSetWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_NEW_ROMSET_WINDOW_H
