/********************************************************************************
** Form generated from reading UI file 'tape_new_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TAPE_NEW_WINDOW_H
#define UI_TAPE_NEW_WINDOW_H

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

QT_BEGIN_NAMESPACE

class Ui_TapeNewWindow
{
public:
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *TapeName;
    QDialogButtonBox *buttonBox;

    void setupUi(QDialog *TapeNewWindow)
    {
        if (TapeNewWindow->objectName().isEmpty())
            TapeNewWindow->setObjectName(QString::fromUtf8("TapeNewWindow"));
        TapeNewWindow->resize(417, 36);
        horizontalLayout = new QHBoxLayout(TapeNewWindow);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        label = new QLabel(TapeNewWindow);
        label->setObjectName(QString::fromUtf8("label"));

        horizontalLayout->addWidget(label);

        TapeName = new QLineEdit(TapeNewWindow);
        TapeName->setObjectName(QString::fromUtf8("TapeName"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TapeName->sizePolicy().hasHeightForWidth());
        TapeName->setSizePolicy(sizePolicy);

        horizontalLayout->addWidget(TapeName);

        buttonBox = new QDialogButtonBox(TapeNewWindow);
        buttonBox->setObjectName(QString::fromUtf8("buttonBox"));
        QSizePolicy sizePolicy1(QSizePolicy::Fixed, QSizePolicy::Fixed);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(buttonBox->sizePolicy().hasHeightForWidth());
        buttonBox->setSizePolicy(sizePolicy1);
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);

        horizontalLayout->addWidget(buttonBox);


        retranslateUi(TapeNewWindow);
        QObject::connect(buttonBox, SIGNAL(accepted()), TapeNewWindow, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), TapeNewWindow, SLOT(reject()));

        QMetaObject::connectSlotsByName(TapeNewWindow);
    } // setupUi

    void retranslateUi(QDialog *TapeNewWindow)
    {
        TapeNewWindow->setWindowTitle(QApplication::translate("TapeNewWindow", "Dialog", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("TapeNewWindow", "Tape Name", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TapeNewWindow: public Ui_TapeNewWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TAPE_NEW_WINDOW_H
