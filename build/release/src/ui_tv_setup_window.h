/********************************************************************************
** Form generated from reading UI file 'tv_setup_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TV_SETUP_WINDOW_H
#define UI_TV_SETUP_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QScrollBar>

QT_BEGIN_NAMESPACE

class Ui_TVSetupWindow
{
public:
    QLabel *label;
    QLabel *label_2;
    QLabel *label_3;
    QLabel *label_4;
    QLabel *label_5;
    QLabel *label_6;
    QLabel *label_7;
    QScrollBar *helligkeit_scroll;
    QScrollBar *saettigung_scroll;
    QScrollBar *kontrast_scroll;
    QScrollBar *horblurY_scroll;
    QScrollBar *horblurUV_scroll;
    QScrollBar *phase_scroll;
    QScrollBar *scanline_scroll;
    QLabel *saettigung_out;
    QLabel *helligkeit_out;
    QLabel *kontrast_out;
    QLabel *horblurY_out;
    QLabel *horblurUV_out;
    QLabel *phase_out;
    QLabel *scanline_out;
    QPushButton *Reset;
    QLabel *label_8;
    QScrollBar *distortion_scroll;
    QLabel *distortion_out;

    void setupUi(QDialog *TVSetupWindow)
    {
        if (TVSetupWindow->objectName().isEmpty())
            TVSetupWindow->setObjectName(QString::fromUtf8("TVSetupWindow"));
        TVSetupWindow->setWindowModality(Qt::NonModal);
        TVSetupWindow->resize(355, 155);
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(TVSetupWindow->sizePolicy().hasHeightForWidth());
        TVSetupWindow->setSizePolicy(sizePolicy);
        TVSetupWindow->setMinimumSize(QSize(344, 155));
        TVSetupWindow->setMaximumSize(QSize(360, 155));
        QFont font;
        font.setFamily(QString::fromUtf8("Arial"));
        TVSetupWindow->setFont(font);
        TVSetupWindow->setContextMenuPolicy(Qt::NoContextMenu);
        TVSetupWindow->setAcceptDrops(false);
        TVSetupWindow->setWindowOpacity(1);
        TVSetupWindow->setAutoFillBackground(false);
        TVSetupWindow->setStyleSheet(QString::fromUtf8(""));
        TVSetupWindow->setModal(false);
        label = new QLabel(TVSetupWindow);
        label->setObjectName(QString::fromUtf8("label"));
        label->setEnabled(false);
        label->setGeometry(QRect(7, 7, 113, 16));
        QFont font1;
        font1.setFamily(QString::fromUtf8("Arial"));
        font1.setBold(false);
        font1.setWeight(50);
        label->setFont(font1);
        label->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        label_2 = new QLabel(TVSetupWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(7, 21, 113, 16));
        label_2->setFont(font1);
        label_2->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        label_3 = new QLabel(TVSetupWindow);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        label_3->setEnabled(false);
        label_3->setGeometry(QRect(7, 35, 113, 16));
        label_3->setFont(font1);
        label_3->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        label_4 = new QLabel(TVSetupWindow);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setGeometry(QRect(7, 49, 106, 16));
        label_4->setFont(font1);
        label_4->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        label_5 = new QLabel(TVSetupWindow);
        label_5->setObjectName(QString::fromUtf8("label_5"));
        label_5->setGeometry(QRect(7, 63, 116, 16));
        label_5->setFont(font1);
        label_5->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        label_6 = new QLabel(TVSetupWindow);
        label_6->setObjectName(QString::fromUtf8("label_6"));
        label_6->setGeometry(QRect(7, 77, 116, 16));
        label_6->setFont(font1);
        label_6->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        label_7 = new QLabel(TVSetupWindow);
        label_7->setObjectName(QString::fromUtf8("label_7"));
        label_7->setGeometry(QRect(7, 91, 101, 16));
        label_7->setFont(font1);
        label_7->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        helligkeit_scroll = new QScrollBar(TVSetupWindow);
        helligkeit_scroll->setObjectName(QString::fromUtf8("helligkeit_scroll"));
        helligkeit_scroll->setGeometry(QRect(126, 21, 176, 14));
        helligkeit_scroll->setFocusPolicy(Qt::StrongFocus);
        helligkeit_scroll->setMaximum(100);
        helligkeit_scroll->setOrientation(Qt::Horizontal);
        saettigung_scroll = new QScrollBar(TVSetupWindow);
        saettigung_scroll->setObjectName(QString::fromUtf8("saettigung_scroll"));
        saettigung_scroll->setEnabled(true);
        saettigung_scroll->setGeometry(QRect(126, 7, 176, 14));
        saettigung_scroll->setFocusPolicy(Qt::StrongFocus);
        saettigung_scroll->setMaximum(100);
        saettigung_scroll->setOrientation(Qt::Horizontal);
        kontrast_scroll = new QScrollBar(TVSetupWindow);
        kontrast_scroll->setObjectName(QString::fromUtf8("kontrast_scroll"));
        kontrast_scroll->setEnabled(true);
        kontrast_scroll->setGeometry(QRect(126, 35, 176, 14));
        kontrast_scroll->setFocusPolicy(Qt::StrongFocus);
        kontrast_scroll->setMinimum(0);
        kontrast_scroll->setMaximum(100);
        kontrast_scroll->setOrientation(Qt::Horizontal);
        horblurY_scroll = new QScrollBar(TVSetupWindow);
        horblurY_scroll->setObjectName(QString::fromUtf8("horblurY_scroll"));
        horblurY_scroll->setGeometry(QRect(126, 49, 176, 14));
        horblurY_scroll->setFocusPolicy(Qt::StrongFocus);
        horblurY_scroll->setMinimum(1);
        horblurY_scroll->setMaximum(4);
        horblurY_scroll->setOrientation(Qt::Horizontal);
        horblurUV_scroll = new QScrollBar(TVSetupWindow);
        horblurUV_scroll->setObjectName(QString::fromUtf8("horblurUV_scroll"));
        horblurUV_scroll->setGeometry(QRect(126, 63, 176, 14));
        horblurUV_scroll->setFocusPolicy(Qt::StrongFocus);
        horblurUV_scroll->setMinimum(1);
        horblurUV_scroll->setMaximum(4);
        horblurUV_scroll->setOrientation(Qt::Horizontal);
        phase_scroll = new QScrollBar(TVSetupWindow);
        phase_scroll->setObjectName(QString::fromUtf8("phase_scroll"));
        phase_scroll->setGeometry(QRect(126, 77, 176, 14));
        phase_scroll->setFocusPolicy(Qt::StrongFocus);
        phase_scroll->setMaximum(2000);
        phase_scroll->setOrientation(Qt::Horizontal);
        scanline_scroll = new QScrollBar(TVSetupWindow);
        scanline_scroll->setObjectName(QString::fromUtf8("scanline_scroll"));
        scanline_scroll->setGeometry(QRect(126, 91, 176, 14));
        scanline_scroll->setFocusPolicy(Qt::StrongFocus);
        scanline_scroll->setMaximum(100);
        scanline_scroll->setOrientation(Qt::Horizontal);
        saettigung_out = new QLabel(TVSetupWindow);
        saettigung_out->setObjectName(QString::fromUtf8("saettigung_out"));
        saettigung_out->setEnabled(false);
        saettigung_out->setGeometry(QRect(308, 7, 43, 16));
        saettigung_out->setMinimumSize(QSize(36, 16));
        saettigung_out->setFont(font1);
        saettigung_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        saettigung_out->setText(QString::fromUtf8("-"));
        helligkeit_out = new QLabel(TVSetupWindow);
        helligkeit_out->setObjectName(QString::fromUtf8("helligkeit_out"));
        helligkeit_out->setGeometry(QRect(308, 21, 43, 16));
        helligkeit_out->setFont(font);
        helligkeit_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        helligkeit_out->setText(QString::fromUtf8("-"));
        kontrast_out = new QLabel(TVSetupWindow);
        kontrast_out->setObjectName(QString::fromUtf8("kontrast_out"));
        kontrast_out->setEnabled(false);
        kontrast_out->setGeometry(QRect(308, 35, 43, 16));
        kontrast_out->setFont(font);
        kontrast_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        kontrast_out->setText(QString::fromUtf8("-"));
        horblurY_out = new QLabel(TVSetupWindow);
        horblurY_out->setObjectName(QString::fromUtf8("horblurY_out"));
        horblurY_out->setGeometry(QRect(308, 49, 43, 16));
        horblurY_out->setFont(font);
        horblurY_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        horblurY_out->setText(QString::fromUtf8("-"));
        horblurUV_out = new QLabel(TVSetupWindow);
        horblurUV_out->setObjectName(QString::fromUtf8("horblurUV_out"));
        horblurUV_out->setGeometry(QRect(308, 63, 43, 16));
        horblurUV_out->setFont(font);
        horblurUV_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        horblurUV_out->setText(QString::fromUtf8("-"));
        phase_out = new QLabel(TVSetupWindow);
        phase_out->setObjectName(QString::fromUtf8("phase_out"));
        phase_out->setGeometry(QRect(308, 77, 43, 16));
        phase_out->setFont(font);
        phase_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        phase_out->setText(QString::fromUtf8("-"));
        scanline_out = new QLabel(TVSetupWindow);
        scanline_out->setObjectName(QString::fromUtf8("scanline_out"));
        scanline_out->setGeometry(QRect(308, 91, 43, 16));
        scanline_out->setFont(font);
        scanline_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        scanline_out->setText(QString::fromUtf8("-"));
        Reset = new QPushButton(TVSetupWindow);
        Reset->setObjectName(QString::fromUtf8("Reset"));
        Reset->setGeometry(QRect(28, 128, 309, 22));
        Reset->setFont(font1);
        Reset->setFocusPolicy(Qt::TabFocus);
        Reset->setStyleSheet(QString::fromUtf8("color: rgb(255, 0, 0);"));
        label_8 = new QLabel(TVSetupWindow);
        label_8->setObjectName(QString::fromUtf8("label_8"));
        label_8->setGeometry(QRect(8, 105, 65, 16));
        label_8->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        distortion_scroll = new QScrollBar(TVSetupWindow);
        distortion_scroll->setObjectName(QString::fromUtf8("distortion_scroll"));
        distortion_scroll->setGeometry(QRect(126, 105, 176, 14));
        distortion_scroll->setMaximum(200);
        distortion_scroll->setValue(100);
        distortion_scroll->setOrientation(Qt::Horizontal);
        distortion_out = new QLabel(TVSetupWindow);
        distortion_out->setObjectName(QString::fromUtf8("distortion_out"));
        distortion_out->setGeometry(QRect(308, 105, 43, 16));
        distortion_out->setFont(font);
        distortion_out->setStyleSheet(QString::fromUtf8("color: rgb(0,0, 128);"));
        distortion_out->setText(QString::fromUtf8("-"));
        QWidget::setTabOrder(saettigung_scroll, helligkeit_scroll);
        QWidget::setTabOrder(helligkeit_scroll, kontrast_scroll);
        QWidget::setTabOrder(kontrast_scroll, horblurY_scroll);
        QWidget::setTabOrder(horblurY_scroll, horblurUV_scroll);
        QWidget::setTabOrder(horblurUV_scroll, phase_scroll);
        QWidget::setTabOrder(phase_scroll, scanline_scroll);
        QWidget::setTabOrder(scanline_scroll, Reset);

        retranslateUi(TVSetupWindow);

        QMetaObject::connectSlotsByName(TVSetupWindow);
    } // setupUi

    void retranslateUi(QDialog *TVSetupWindow)
    {
        TVSetupWindow->setWindowTitle(QApplication::translate("TVSetupWindow", "TV/Video Einstellungen", 0, QApplication::UnicodeUTF8));
#ifndef QT_NO_TOOLTIP
        label->setToolTip(QString());
#endif // QT_NO_TOOLTIP
#ifndef QT_NO_STATUSTIP
        label->setStatusTip(QString());
#endif // QT_NO_STATUSTIP
        label->setText(QApplication::translate("TVSetupWindow", "S\303\244ttigung", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("TVSetupWindow", "Helligkeit", 0, QApplication::UnicodeUTF8));
        label_3->setText(QApplication::translate("TVSetupWindow", "Kontrast", 0, QApplication::UnicodeUTF8));
        label_4->setText(QApplication::translate("TVSetupWindow", "Horizontaler Blur (Y)", 0, QApplication::UnicodeUTF8));
        label_5->setText(QApplication::translate("TVSetupWindow", "Horizontaler Blur (UV)", 0, QApplication::UnicodeUTF8));
        label_6->setText(QApplication::translate("TVSetupWindow", "Phasen Verschiebung", 0, QApplication::UnicodeUTF8));
        label_7->setText(QApplication::translate("TVSetupWindow", "Scanline", 0, QApplication::UnicodeUTF8));
        Reset->setText(QApplication::translate("TVSetupWindow", "Alles wieder auf Standard Werte zur\303\274cksetzen", 0, QApplication::UnicodeUTF8));
        label_8->setText(QApplication::translate("TVSetupWindow", "Verzerrung", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class TVSetupWindow: public Ui_TVSetupWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TV_SETUP_WINDOW_H
