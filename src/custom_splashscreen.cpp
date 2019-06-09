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
// Letzte Änderung am 09.06.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "custom_splashscreen.h"

CustomSplashScreen::CustomSplashScreen(const QPixmap& pixmap)
{
    QSplashScreen::setPixmap(pixmap);
};

CustomSplashScreen::~CustomSplashScreen()
{
};

void CustomSplashScreen::drawContents(QPainter *painter)
{
     QPixmap textPix = QSplashScreen::pixmap();
     painter->setPen(this->color);
     painter->drawText(this->rect, this->alignement, this->message);
};

void CustomSplashScreen::ShowStatusMessage(const QString &message, const QColor &color)
{
     this->message = message;
     this->color = color;
     this->showMessage(this->message, this->alignement, this->color);
};

void CustomSplashScreen::SetMessageRect(QRect rect, int alignement)
{
     this->rect = rect;
     this->alignement = alignement;
};
