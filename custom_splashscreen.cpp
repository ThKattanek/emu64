//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: custom_splashscreen.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 28.08.2011                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "custom_splashscreen.h"

customSplashScreen::customSplashScreen(const QPixmap& pixmap)
{
 QSplashScreen::setPixmap(pixmap);
};

customSplashScreen::~customSplashScreen()
{
};

void customSplashScreen::drawContents(QPainter *painter)
{
 QPixmap textPix = QSplashScreen::pixmap();
 painter->setPen(this->color);
 painter->drawText(this->rect, this->alignement, this->message);
};

void customSplashScreen::showStatusMessage(const QString &message, const QColor &color)
{
 this->message = message;
 this->color = color;
 this->showMessage(this->message, this->alignement, this->color);
};

void customSplashScreen::setMessageRect(QRect rect, int alignement)
{
 this->rect = rect;
 this->alignement = alignement;
};
