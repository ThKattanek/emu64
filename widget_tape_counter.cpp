#include "widget_tape_counter.h"
#include "ui_widget_tape_counter.h"

WidgetTapeCounter::WidgetTapeCounter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::WidgetTapeCounter)
{
    ui->setupUi(this);
}

WidgetTapeCounter::~WidgetTapeCounter()
{
    delete ui;
}
