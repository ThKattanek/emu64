#include "oscilloscope_widget.h"
#include "ui_oscilloscope_widget.h"

#include <QPainter>
#include <QDebug>

OscilloscopeWidget::OscilloscopeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OscilloscopeWidget)
{
    ui->setupUi(this);

    // default colors
    background_color.setRgb(0,120,170);
    line_color.setRgb(0,220,0);
    raster_color.setRgb(0,0,0);
    trigger_line_color.setRgb(150,150,150);

    vertical_position = 0.5f;
    horizontal_position = 0.5f;
    amplifire = 1.0f;

    output_plane_xpos = 0;

    ringbuffer_pos_r = 0;
    ringbuffer_pos_w = 0;

    trigger = false;
    trigger_typ = TRIGGER_OFF;
    trigger_level = 0.0f;

    refresh = false;
    timer1 = new QTimer(this);
    connect(timer1,SIGNAL(timeout()),this,SLOT(OnRefresh()));
    timer1->start(10);

    output_line_pen = new QPen(Qt::green, 1.0, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin);
}

OscilloscopeWidget::~OscilloscopeWidget()
{
    timer1->stop();
    delete ui;
}

void OscilloscopeWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    painter.fillRect(0,0,this->width(),this->height(), background_color);

    DrawRaster(painter, this->width(), this->height());

    painter.setRenderHint(QPainter::Antialiasing,true);

    DrawData(painter, this->width());
    DrawTriggerLevel(painter, this->width(), this->height());
}

void OscilloscopeWidget::DrawRaster(QPainter &painter, int width, int height)
{
    painter.setPen(raster_color);

    float half_height = height / 2;
    float half_width = width / 2;
    int line_len0 = 5;
    int line_len1 = 2;

    // Vertical Lines
    float div_h = width / 10;
    float div_v = height / 8;
    for(int x=0; x<10; x++)
    {
        painter.drawLine(static_cast<int>(x * div_h),0 , static_cast<int>(x * div_h), static_cast<int>(this->height()));
    }
    painter.drawLine(width-1, 0 , width-1,this->height());

    // Center horizonal Scale
    div_h = width / 50;
    for(int x=0; x<50; x++)
    {
        painter.drawLine(static_cast<int>(x * div_h), static_cast<int>(half_height - line_len0/2), static_cast<int>(x * div_h), static_cast<int>(half_height + line_len0/2));
        painter.drawLine(static_cast<int>(x * div_h), static_cast<int>((half_height - line_len1/2) + div_v*2), static_cast<int>(x * div_h), static_cast<int>((half_height + line_len1/2) + div_v * 2));
        painter.drawLine(static_cast<int>(x * div_h), static_cast<int>((half_height - line_len1/2) - div_v*2) ,static_cast<int>(x * div_h), static_cast<int>((half_height + line_len1/2) - div_v * 2));

        painter.drawPoint(static_cast<int>(x * div_h), static_cast<int>(half_height + div_v * 2.5f));
        painter.drawPoint(static_cast<int>(x * div_h), static_cast<int>(half_height - div_v * 2.5f));
    }

    // HLines
    // Horizont lines
    for(int y=0; y<8; y++)
    {
        painter.drawLine(0, static_cast<int>(y * div_v), static_cast<int>(width), static_cast<int>(y * div_v));
    }
    painter.drawLine(0, height-1 , width, height-1);

    // Center vertical Scale
    div_h = height / 40;
    for(int y=0; y<40; y++)
    {
        painter.drawLine(static_cast<int>(half_width - line_len0/2), static_cast<int>(y * div_h), static_cast<int>(half_width + line_len0/2), static_cast<int>(y * div_h));
    }
}

void OscilloscopeWidget::DrawTriggerLevel(QPainter &painter, int width, int height)
{
    if(trigger_typ != TRIGGER_OFF)
    {
        float y_div_pix = height/8.0f;
        float y_add = height * vertical_position;
        int y = static_cast<int>(trigger_level * -1.0f * y_div_pix + y_add);

        painter.setPen(trigger_line_color);

        painter.drawLine(0,y,width,y);
    }
}

void OscilloscopeWidget::DrawData(QPainter &painter, int width)
{
    int x1,x2,y1,y2;

    painter.setPen(*output_line_pen);

    x1 = 0;
    y1 = tmp_output[0];

    for(x2=1; x2<width; x2++)
    {
         y2 = tmp_output[x2];
         painter.drawLine(x1,y1,x2,y2);

         x1 = x2;
         y1 = y2;
    }
}

void OscilloscopeWidget::NextAudioData(float *data, int length)
{
    // Es wird angenommen das ein Stereo Stream hier ankommt
    // Das müsste noch angepasst weden um Flexibler zu sein
    // Der linke Kanal wird hier rausgezogen

    float y_div_pix = height()/8.0f;
    float y_factor = amplifire;
    float y_add = height() * vertical_position;

    for(int i=0; i<length; i++)
    {
        current_value = data[i] * y_factor;

        switch (trigger_typ)
        {
        case TRIGGER_OFF:
            break;
        case RISING_EDGE:
            if((old_value < trigger_level) && (current_value >= trigger_level) && !trigger)
            {
                trigger = true;
                output_plane_xpos = 0;
                ringbuffer_pos_r = ringbuffer_pos_w - width() * horizontal_position;
                ringbuffer_pos_r &= 0x1fff;
            }
            break;
        case FALLING_EDGE:
            if((old_value > trigger_level) && (current_value <= trigger_level) && !trigger)
            {
                trigger = true;
                output_plane_xpos = 0;
                ringbuffer_pos_r = ringbuffer_pos_w - width() * horizontal_position;
                ringbuffer_pos_r &= 0x1fff;
            }
            break;
        }

        old_value = current_value;

        ringbuffer[ringbuffer_pos_w++] = static_cast<int>(current_value * -1.0f * y_div_pix + y_add);
        ringbuffer_pos_w &= 0x1fff;

        output_plane_xpos ++;

        if(output_plane_xpos >= width())
        {
            trigger = false;
            output_plane_xpos = 0;

            for(int x=0; x<width(); x++)
            {
                tmp_output[x] = ringbuffer[ringbuffer_pos_r++];
                ringbuffer_pos_r &= 0x1fff;
            }
            refresh = true;
        }
    }
}

void OscilloscopeWidget::NextAudioData(uint8_t *data, int length)
{
    int16_t *buffer = reinterpret_cast<int16_t*>(data);

    float *new_data = new float[length];
    for(int i=0; i<length/2; i++)
        new_data[i] = static_cast<float>(buffer[i]) / 0x7fff;
    NextAudioData(new_data, length/2);
    delete [] new_data;
}

void OscilloscopeWidget::OnRefresh()
{
    if(refresh)
    {
        update();
        refresh = false;
    }
}

QColor OscilloscopeWidget::CalcFadeColor(QColor src_color, QColor dst_color, float fade)
{
    // ((dst_color - scr_color) * fade) + src_color
    int r = static_cast<int>(src_color.red() + ((dst_color.red() - src_color.red()) * fade));
    int g = static_cast<int>(src_color.green() + ((dst_color.green() - src_color.green()) * fade));
    int b = static_cast<int>(src_color.blue() + ((dst_color.blue() - src_color.blue()) * fade));

    return QColor(r,g,b);
}

void OscilloscopeWidget::SetSamplerate(float samplerate)
{
    this->samplerate = samplerate;
}

void OscilloscopeWidget::SetBackgroundColor(QColor color)
{
    background_color = color;
}

void OscilloscopeWidget::SetLineColor(QColor color)
{
    line_color = color;
}

void OscilloscopeWidget::SetTriggerLineColor(QColor color)
{
    trigger_line_color = color;
}

void OscilloscopeWidget::SetRasterColor(QColor color)
{
    raster_color = color;
}

void OscilloscopeWidget::SetVerticalPosition(float value)
{
    vertical_position = value;
}

void OscilloscopeWidget::SetAmplifire(float value)
{
    amplifire = value;
}

void OscilloscopeWidget::SetTriggerTyp(int value)
{
    trigger_typ = value;
}

void OscilloscopeWidget::SetTriggerLevel(float value)
{
    trigger_level = value;
}
