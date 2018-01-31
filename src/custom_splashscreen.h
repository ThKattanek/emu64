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
// Letzte Änderung am 18.05.2014                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#ifndef CUSTOM_SPLASHSCREEN_H
#define CUSTOM_SPLASHSCREEN_H

#include <QSplashScreen>
#include <QPainter>

class customSplashScreen
 :public QSplashScreen
{

public:
 customSplashScreen(const QPixmap& pixmap);
 ~customSplashScreen();
 virtual void drawContents(QPainter *painter);
 void showStatusMessage(const QString &message, const QColor &color = Qt::black);
 void setMessageRect(QRect rect, int alignment = Qt::AlignLeft);

private:
 QString message;
 int alignement;
 QColor color;
 QRect rect;
};

#endif // CUSTOM_SPLASHSCREEN_H
