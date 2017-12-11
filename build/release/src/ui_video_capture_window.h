/********************************************************************************
** Form generated from reading UI file 'video_capture_window.ui'
**
** Created by: Qt User Interface Compiler version 4.8.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_VIDEO_CAPTURE_WINDOW_H
#define UI_VIDEO_CAPTURE_WINDOW_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QDialog>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QSpinBox>

QT_BEGIN_NAMESPACE

class Ui_VideoCaptureWindow
{
public:
    QPushButton *CaptureStart;
    QPushButton *CapturePause;
    QPushButton *CaptureStop;
    QLabel *TimeOutput;
    QLabel *label;
    QLabel *label_2;
    QSpinBox *AudioBitrate;
    QSpinBox *VideoBitrate;

    void setupUi(QDialog *VideoCaptureWindow)
    {
        if (VideoCaptureWindow->objectName().isEmpty())
            VideoCaptureWindow->setObjectName(QString::fromUtf8("VideoCaptureWindow"));
        VideoCaptureWindow->resize(268, 109);
        QFont font;
        font.setBold(false);
        font.setWeight(50);
        VideoCaptureWindow->setFont(font);
        CaptureStart = new QPushButton(VideoCaptureWindow);
        CaptureStart->setObjectName(QString::fromUtf8("CaptureStart"));
        CaptureStart->setGeometry(QRect(10, 10, 30, 30));
        CaptureStart->setFocusPolicy(Qt::NoFocus);
        QIcon icon;
        icon.addFile(QString::fromUtf8(":/grafik/video_rec_start.png"), QSize(), QIcon::Normal, QIcon::Off);
        CaptureStart->setIcon(icon);
        CaptureStart->setIconSize(QSize(25, 25));
        CapturePause = new QPushButton(VideoCaptureWindow);
        CapturePause->setObjectName(QString::fromUtf8("CapturePause"));
        CapturePause->setGeometry(QRect(40, 10, 30, 30));
        CapturePause->setFocusPolicy(Qt::NoFocus);
        QIcon icon1;
        icon1.addFile(QString::fromUtf8(":/grafik/video_rec_pause_off.png"), QSize(), QIcon::Normal, QIcon::Off);
        icon1.addFile(QString::fromUtf8(":/grafik/video_rec_pause_on.png"), QSize(), QIcon::Normal, QIcon::On);
        CapturePause->setIcon(icon1);
        CapturePause->setIconSize(QSize(25, 25));
        CapturePause->setCheckable(true);
        CapturePause->setChecked(false);
        CaptureStop = new QPushButton(VideoCaptureWindow);
        CaptureStop->setObjectName(QString::fromUtf8("CaptureStop"));
        CaptureStop->setEnabled(true);
        CaptureStop->setGeometry(QRect(70, 10, 30, 30));
        CaptureStop->setFocusPolicy(Qt::NoFocus);
        QIcon icon2;
        icon2.addFile(QString::fromUtf8(":/grafik/video_rec_stop.png"), QSize(), QIcon::Normal, QIcon::Off);
        CaptureStop->setIcon(icon2);
        CaptureStop->setIconSize(QSize(25, 25));
        TimeOutput = new QLabel(VideoCaptureWindow);
        TimeOutput->setObjectName(QString::fromUtf8("TimeOutput"));
        TimeOutput->setEnabled(true);
        TimeOutput->setGeometry(QRect(110, 9, 151, 31));
        QFont font1;
        font1.setPointSize(20);
        font1.setBold(false);
        font1.setWeight(50);
        TimeOutput->setFont(font1);
        label = new QLabel(VideoCaptureWindow);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(10, 47, 121, 21));
        label_2 = new QLabel(VideoCaptureWindow);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        label_2->setGeometry(QRect(10, 80, 121, 21));
        AudioBitrate = new QSpinBox(VideoCaptureWindow);
        AudioBitrate->setObjectName(QString::fromUtf8("AudioBitrate"));
        AudioBitrate->setGeometry(QRect(130, 50, 131, 22));
        VideoBitrate = new QSpinBox(VideoCaptureWindow);
        VideoBitrate->setObjectName(QString::fromUtf8("VideoBitrate"));
        VideoBitrate->setGeometry(QRect(130, 80, 131, 22));

        retranslateUi(VideoCaptureWindow);

        QMetaObject::connectSlotsByName(VideoCaptureWindow);
    } // setupUi

    void retranslateUi(QDialog *VideoCaptureWindow)
    {
        VideoCaptureWindow->setWindowTitle(QApplication::translate("VideoCaptureWindow", "Video Aufnahme", 0, QApplication::UnicodeUTF8));
        CaptureStart->setText(QString());
        CapturePause->setText(QString());
        CaptureStop->setText(QString());
        TimeOutput->setText(QApplication::translate("VideoCaptureWindow", "00:00:00-00", 0, QApplication::UnicodeUTF8));
        label->setText(QApplication::translate("VideoCaptureWindow", " Audio Bitrate [kbit/s]", 0, QApplication::UnicodeUTF8));
        label_2->setText(QApplication::translate("VideoCaptureWindow", "Video Bitrate [kbit/s]", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class VideoCaptureWindow: public Ui_VideoCaptureWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_VIDEO_CAPTURE_WINDOW_H
