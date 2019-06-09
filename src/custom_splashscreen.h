//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: custom_splashscreen.h                 //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 09.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CUSTOM_SPLASHSCREEN_H
#define CUSTOM_SPLASHSCREEN_H

#include <QSplashScreen>
#include <QPainter>

class CustomSplashScreen
 :public QSplashScreen
{

public:
 CustomSplashScreen(const QPixmap& pixmap);
 ~CustomSplashScreen();
 virtual void drawContents(QPainter *painter);
 void ShowStatusMessage(const QString &message, const QColor &color = Qt::black);
 void SetMessageRect(QRect rect, int alignment = Qt::AlignLeft);

private:
 QString message;
 int alignement;
 QColor color;
 QRect rect;
};

#endif // CUSTOM_SPLASHSCREEN_H
