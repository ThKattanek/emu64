#include "widget_bit_leiste.h"
#include "ui_widget_bit_leiste.h"

WidgetBitLeiste::WidgetBitLeiste(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetBitLeiste),
    value(0),
    Enabled(false)
{
    ui->setupUi(this);
}

WidgetBitLeiste::~WidgetBitLeiste()
{
    delete ui;
}

void WidgetBitLeiste::paintEvent(QPaintEvent *e)
{
    QPainter p(this);
    p.setPen(QColor(0,50,0));
    p.setBrush(QBrush(Qt::SolidPattern));
    p.drawRect(0,0,this->width(),this->height());

    if(Enabled)
    {
        p.setPen(QColor(0,255,0));
        unsigned char v = value;
        unsigned char m = 128;

        for(int i=0;i<8;i++)
        {
            if(v&m) p.drawLine(2+i*2,0,3+i*2,0);
            m = m >> 1;
        }
    }
}

void WidgetBitLeiste::SetValue(unsigned short v)
{
    value = v;
    update();
}

void WidgetBitLeiste::EnableBitLeiste(bool status)
{
    Enabled = status;
    update();
}
