#include "button_mod.h"

ButtonMod::ButtonMod(int idx,int idy, QWidget *parent) :
    QPushButton(parent)
{
    idX = idx;
    idY = idy;

    connect(this,SIGNAL(clicked()),this,SLOT(onClicked()));
}

void ButtonMod::onClicked()
{
    emit Clicked(idX,idY);
}
