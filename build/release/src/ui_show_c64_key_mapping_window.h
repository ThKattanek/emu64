/********************************************************************************
** Form generated from reading UI file 'show_c64_key_mapping_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_SHOW_C64_KEY_MAPPING_WINDOW_H
#define UI_SHOW_C64_KEY_MAPPING_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QPlainTextEdit>

QT_BEGIN_NAMESPACE

class Ui_ShowC64KeyMappingWindow
{
public:
    QHBoxLayout *horizontalLayout;
    QPlainTextEdit *text_out;

    void setupUi(QDialog *ShowC64KeyMappingWindow)
    {
        if (ShowC64KeyMappingWindow->objectName().isEmpty())
            ShowC64KeyMappingWindow->setObjectName(QString::fromUtf8("ShowC64KeyMappingWindow"));
        ShowC64KeyMappingWindow->resize(297, 348);
        horizontalLayout = new QHBoxLayout(ShowC64KeyMappingWindow);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        text_out = new QPlainTextEdit(ShowC64KeyMappingWindow);
        text_out->setObjectName(QString::fromUtf8("text_out"));
        QFont font;
        font.setFamily(QString::fromUtf8("DejaVu Sans Mono"));
        font.setPointSize(10);
        font.setBold(true);
        font.setWeight(75);
        text_out->setFont(font);
        text_out->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        text_out->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        text_out->setReadOnly(true);

        horizontalLayout->addWidget(text_out);


        retranslateUi(ShowC64KeyMappingWindow);

        QMetaObject::connectSlotsByName(ShowC64KeyMappingWindow);
    } // setupUi

    void retranslateUi(QDialog *ShowC64KeyMappingWindow)
    {
        ShowC64KeyMappingWindow->setWindowTitle(QApplication::translate("ShowC64KeyMappingWindow", "Aktuelles C64 Key Mapping", 0, QApplication::UnicodeUTF8));
        text_out->setPlainText(QString());
    } // retranslateUi

};

namespace Ui {
    class ShowC64KeyMappingWindow: public Ui_ShowC64KeyMappingWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_SHOW_C64_KEY_MAPPING_WINDOW_H
