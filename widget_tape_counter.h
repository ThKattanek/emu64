#ifndef WIDGET_TAPE_COUNTER_H
#define WIDGET_TAPE_COUNTER_H

#include <QWidget>

namespace Ui {
class WidgetTapeCounter;
}

class WidgetTapeCounter : public QWidget
{
    Q_OBJECT

public:
    explicit WidgetTapeCounter(QWidget *parent = 0);
    ~WidgetTapeCounter();

private:
    Ui::WidgetTapeCounter *ui;
};

#endif // WIDGET_TAPE_COUNTER_H
