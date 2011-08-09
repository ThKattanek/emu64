#ifndef WIDGET_BIT_LEISTE_H
#define WIDGET_BIT_LEISTE_H

#include <QtCore>
#include <QtGui>

namespace Ui {
    class WidgetBitLeiste;
}

class WidgetBitLeiste : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetBitLeiste(QWidget *parent = 0);
    ~WidgetBitLeiste();
    void SetValue(unsigned short v);
    void EnableBitLeiste(bool status);

private slots:
     void paintEvent(QPaintEvent *);

private:
    Ui::WidgetBitLeiste *ui;
    unsigned short value;
    bool Enabled;
};

#endif // WIDGET_BIT_LEISTE_H
