//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: input_box_window.cpp                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 23.04.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "input_box_window.h"
#include "qmessagebox.h"
#include "qpushbutton.h"
#include "qregularexpression.h"
#include "qvalidator.h"
#include "ui_input_box_window.h"

InputBoxWindow::InputBoxWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::InputBoxWindow)
{
    ui->setupUi(this);

    QRegularExpression regex_dec("^[0-9]*$");
    QRegularExpression regex_hex("^[a-fA-F0-9]*$");
    QRegularExpression regex_bin("^[0-1]*$");

    QValidator *validator_dec = new QRegularExpressionValidator(regex_dec, this);
    QValidator *validator_hex = new QRegularExpressionValidator(regex_hex, this);
    QValidator *validator_bin = new QRegularExpressionValidator(regex_bin, this);

    ui->edit_dec->setValidator(validator_dec);
    ui->edit_hex->setValidator(validator_hex);
    ui->edit_bin->setValidator(validator_bin);

    range_min = 0;
    range_max = 255;

    ui->error_message->setHidden(true);
}

InputBoxWindow::~InputBoxWindow()
{
    delete ui;
}

void InputBoxWindow::setValue(uint64_t newValue)
{
    value = newValue;

    ui->edit_dec->setText(QString::number(value, 10));
    ui->edit_hex->setText(QString::number(value, 16).toUpper());
    ui->edit_bin->setText(QString::number(value, 2));
}

uint64_t InputBoxWindow::getValue() const
{
    return value;
}

void InputBoxWindow::setRange_min(uint64_t newRange_min)
{
    range_min = newRange_min;
}

void InputBoxWindow::setRange_max(uint64_t newRange_max)
{
    range_max = newRange_max;
}

void InputBoxWindow::on_edit_dec_textChanged(const QString &arg1)
{
    bool is_ok;
    uint64_t newValue = arg1.toInt(&is_ok, 10);

    if(is_ok)
    {
        value = newValue;
        ui->edit_hex->setText(QString::number(value, 16).toUpper());
        ui->edit_bin->setText(QString::number(value, 2));
    }

    if(value < range_min || value > range_max)
    {
        ui->error_message->setText("Bereich: " + QString::number(range_min, 10) + " - " + QString::number(range_max, 10));
        ui->error_message->setHidden(false);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    }
    else
    {
        ui->error_message->setHidden(true);
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
    }
}

void InputBoxWindow::on_edit_hex_textChanged(const QString &arg1)
{
    bool is_ok;
    uint64_t newValue = arg1.toInt(&is_ok, 16);

    if(is_ok)
    {
        value = newValue;
        ui->edit_dec->setText(QString::number(value, 10));
        ui->edit_bin->setText(QString::number(value, 2));
    }
}

void InputBoxWindow::on_edit_bin_textChanged(const QString &arg1)
{
    bool is_ok;
    uint64_t newValue = arg1.toInt(&is_ok, 2);

    if(is_ok)
    {
        value = newValue;
        ui->edit_hex->setText(QString::number(value, 16).toUpper());
        ui->edit_dec->setText(QString::number(value, 10));
    }
}
