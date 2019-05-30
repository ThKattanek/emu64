//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: c64_keyboard_window.cpp               //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek       	//
//                                              //
// Letzte Änderung am 30.05.2019                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include "c64_keyboard_window.h"
#include "ui_c64_keyboard_window.h"

static const int Y_MIN[7] = {12,60,107,153,153,177,199};
static const int Y_MAX[7] = {59,106,152,198,174,198,247};
static const int KEYS_Y[7] ={18,17,17,15,2,2,1};
static const int X_MIN[7][18] = {{18,67,114,161,208,255,303,350,397,444,492,539,585,633,680,727,797,834},
                                {18,90,138,184,232,279,326,374,420,469,515,561,609,656,704,797,834},
                                {9,55,103,150,197,244,291,339,387,433,480,527,575,622,669,797,834},
                                {9,55,127,174,221,268,316,363,410,457,505,552,600,797,834},
                                {669,718},
                                {669,718},
                                {137}};
static const int X_MAX[7][18] = {{66,113,160,207,254,302,349,396,443,491,538,584,632,679,726,774,833,868},
                                {89,137,183,231,278,325,373,419,468,514,560,608,655,703,774,833,868},
                                {54,102,149,196,243,290,338,386,432,479,526,574,621,668,765,833,868},
                                {54,126,173,220,267,315,362,409,456,504,551,599,668,833,868},
                                {717,765},
                                {717,765},
                                {561}};

/// TansTabelle von LeyLayout zu C64Matrix
static unsigned short VK_TO_C64[7][18] = {{0x71,0x70,0x73,0x10,0x13,0x20,0x23,0x30,0x33,0x40,0x43,0x50,0x53,0x60,0x63,0x00,0x04,0x84},
                                         {0x72,0x76,0x11,0x16,0x21,0x26,0x31,0x36,0x41,0x46,0x51,0x56,0x61,0x66,0x80,0x05,0x85},
                                         {0x77,0x17,0x12,0x15,0x22,0x25,0x32,0x35,0x42,0x45,0x52,0x55,0x62,0x65,0x01,0x06,0x86},
                                         {0x75,0x17,0x14,0x27,0x24,0x37,0x34,0x47,0x44,0x57,0x54,0x67,0x64,0x03,0x83},
                                         {0x87,0x82},
                                         {0x07,0x02},
                                         {0x74}};
static bool VK_RAST[7][18];

static uint8_t KeyMatrixToPA_LM[8];
static uint8_t KeyMatrixToPB_LM[8];
static uint8_t KeyMatrixToPA_RM[8];
static uint8_t KeyMatrixToPB_RM[8];

C64KeyboardWindow::C64KeyboardWindow(QWidget *parent, QSettings *ini, C64Class *c64) :
    QDialog(parent),
    ui(new Ui::C64KeyboardWindow)
{
    this->c64 = c64;
    this->ini = ini;

    ui->setupUi(this);

    for(int i=0;i<7;i++)
        for(int j=0;j<18;j++)
            VK_RAST[i][j] = false;

    ws_x = width();
    ws_y = height();

    RecKeyPress = false;
    RecKeyAktX = 0xFF;
    RecKeyAktY = 0xFF;

    KeyMatrixToPA = nullptr;
    KeyMatrixToPB = nullptr;

    Recording = false;

    isOneShowed = false;
    AKT_Y_KEY_OLD = 0xFF;
    AKT_Y_KEY = 0xFF;
    AKT_X_KEY_OLD = 0xFF;
    AKT_X_KEY = 0xFF;

    setMouseTracking(true);

    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()),this,SLOT(timer_event()));
    timer->start(200);

    ////////// Load from INI ///////////
    if(ini != nullptr)
    {
        ini->beginGroup("C64KeyboardWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        //if(ini->value("Show",false).toBool()) show();
        ini->endGroup();
    }
    ////////////////////////////////////
}

C64KeyboardWindow::~C64KeyboardWindow(void)
{
    ////////// Save to INI ///////////
    if(ini != nullptr)
    {
        ini->beginGroup("C64KeyboardWindow");
        if(isOneShowed) ini->setValue("Geometry",saveGeometry());
        if(isHidden()) ini->setValue("Show",false);
        else ini->setValue("Show",true);
        ini->endGroup();
    }
    ////////////////////////////////////
    delete timer;
    delete ui;
}

void C64KeyboardWindow::RetranslateUi(void)
{
    ui->retranslateUi(this);
    this->update();
}

void C64KeyboardWindow::resizeEvent(QResizeEvent *event)
{
    scaling_x = static_cast<float_t>(event->size().width()) / 880.0f;
    scaling_y = static_cast<float_t>(event->size().height()) / 260.0f;
}

void C64KeyboardWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
}

void C64KeyboardWindow::mouseMoveEvent(QMouseEvent *event)
{
    static int xmin,xmax,ymin,ymax;

    QPoint mousePos = event->pos();

    int mx = mousePos.x();
    int my = mousePos.y();

    for(int i=0;i<7;i++)
    {
        ymin = int(Y_MIN[i] * scaling_y);
        ymax = int(Y_MAX[i] * scaling_y);

        if(my>=ymin && my<=ymax)
        {
            AKT_Y_KEY = i;

            for(int i=0;i<KEYS_Y[AKT_Y_KEY];i++)
            {
                xmin = int(X_MIN[AKT_Y_KEY][i] * scaling_x);
                xmax = int(X_MAX[AKT_Y_KEY][i] * scaling_x);

                if(mx>=xmin && mx<=xmax)
                {
                    AKT_X_KEY = i;
                    goto Lend;
                }
                else AKT_X_KEY = 0xFF;
            }
        }
        else AKT_Y_KEY = 0xFF;
    }

Lend:

    if((AKT_Y_KEY != AKT_Y_KEY_OLD) || (AKT_X_KEY != AKT_X_KEY_OLD))
    {
        AKT_Y_KEY_OLD = AKT_Y_KEY;
        AKT_X_KEY_OLD = AKT_X_KEY;

        if((AKT_Y_KEY != 0xFF) && (AKT_X_KEY != 0xFF))
        {
            update();
            if ((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
            {                
                if(!Recording)
                {
                    for(int i=0;i<8;i++)
                    {
                        KeyMatrixToPA_LM[i]=0;
                        KeyMatrixToPB_LM[i]=0;
                    }
                    unsigned short C64Key = VK_TO_C64[AKT_Y_KEY][AKT_X_KEY];

                    VK_RAST[AKT_Y_KEY][AKT_X_KEY] = false;
                    KeyMatrixToPB_RM[(C64Key>>4)&0xF] &= ~(1<<(C64Key&0xF));
                    KeyMatrixToPB_LM[(C64Key>>4)&0xF] |= 1<<(C64Key&0xF);

                    KeyMatrixToPA_RM[C64Key&0xF] &= ~(1<<((C64Key>>4)&0xF));
                    KeyMatrixToPA_LM[C64Key&0xF] |= 1<<((C64Key>>4)&0xF);

                    for(int i=0;i<8;i++)
                    {
                        if (KeyMatrixToPA != nullptr) KeyMatrixToPA[i] = KeyMatrixToPA_LM[i] | KeyMatrixToPA_RM[i];
                        if (KeyMatrixToPB != nullptr) KeyMatrixToPB[i] = KeyMatrixToPB_LM[i] | KeyMatrixToPB_RM[i];
                    }
                }
            }
        }
        else
        {
            update();
        }
    }
}

void C64KeyboardWindow::mousePressEvent(QMouseEvent *event)
{
   //QPoint mouse_offset = event->pos();

    if(!Recording)
    {
        if((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
        {
            if(AKT_X_KEY == 0xFF || AKT_Y_KEY == 0xFF) return;

            uint16_t C64Key = VK_TO_C64[AKT_Y_KEY][AKT_X_KEY];

            //// RESTORE TASTE ---> NMI C64 ///
            if((C64Key & 0x0100) == 0x0100)
            {
                    //C64::SetRestore();
                    return;
            }
            ///////////////////////////////////
            VK_RAST[AKT_Y_KEY][AKT_X_KEY] = false;
            KeyMatrixToPB_RM[(C64Key>>4)&0xF] &= ~(1<<(C64Key&0xF));
            KeyMatrixToPB_LM[(C64Key>>4)&0xF] |= 1<<(C64Key&0xF);

            KeyMatrixToPA_RM[C64Key&0xF] &= ~(1<<((C64Key>>4)&0xF));
            KeyMatrixToPA_LM[C64Key&0xF] |= 1<<((C64Key>>4)&0xF);


        }

        if((event->buttons() & Qt::RightButton) == Qt::RightButton)
        {
            if(AKT_X_KEY == 0xFF || AKT_Y_KEY == 0xFF) return;

            uint16_t C64Key = VK_TO_C64[AKT_Y_KEY][AKT_X_KEY];

            if(VK_RAST[AKT_Y_KEY][AKT_X_KEY])
            {
                VK_RAST[AKT_Y_KEY][AKT_X_KEY] = false;
                KeyMatrixToPB_RM[(C64Key>>4)&0xF] &= ~(1<<(C64Key&0xF));
                KeyMatrixToPA_RM[C64Key&0xF] &= ~(1<<((C64Key>>4)&0xF));
            }
            else
            {
                VK_RAST[AKT_Y_KEY][AKT_X_KEY] = true;
                KeyMatrixToPB_RM[(C64Key>>4)&0xF] |= 1<<(C64Key&0xF);
                KeyMatrixToPA_RM[C64Key&0xF] |= 1<<((C64Key>>4)&0xF);
            }
        }

        for(int i=0;i<8;i++)
        {
            KeyMatrixToPA[i] = KeyMatrixToPA_LM[i] | KeyMatrixToPA_RM[i];
            KeyMatrixToPB[i] = KeyMatrixToPB_LM[i] | KeyMatrixToPB_RM[i];
        }
    }
    else
    {
        if((event->buttons() & Qt::LeftButton) == Qt::LeftButton)
        {
            if(AKT_X_KEY == 0xFF || AKT_Y_KEY == 0xFF) return;

            RecKeyPress = true;
            blink_flip = true;
            RecTimeOut = 24;

            // Restore Taste
            /*
            if((AKT_Y_KEY == 1) && (AKT_X_KEY == 14))
            {
                if(c64 != nullptr) c64->StopRecKeyMap();
                RecKeyPress = false;
                blink_flip = false;
                return;
            }
            */

            // Shift Lock Taste
            if((AKT_Y_KEY == 2) && (AKT_X_KEY == 1))
            {
                if(c64 != nullptr) c64->StopRecKeyMap();
                RecKeyPress = false;
                blink_flip = false;
                return;
            }

            RecKeyAktX = AKT_X_KEY;
            RecKeyAktY = AKT_Y_KEY;
            update();

            uint8_t matrix_code = VK_TO_C64[RecKeyAktY][RecKeyAktX];

            c64->SetFocusToC64Window();
            c64->StartRecKeyMap(matrix_code);
            timer->start(200);
        }
    }
}

void C64KeyboardWindow::mouseReleaseEvent(QMouseEvent*)
{
    if(!Recording)
    {
        for(int i=0;i<8;i++)
        {
            KeyMatrixToPB_LM[i]=0;
            KeyMatrixToPB[i] = KeyMatrixToPB_LM[i] | KeyMatrixToPB_RM[i];
            KeyMatrixToPA[i] = KeyMatrixToPA_LM[i] | KeyMatrixToPA_RM[i];
        }
    }
}

void C64KeyboardWindow::paintEvent(QPaintEvent*)
{
    QPainter painter(this);
    QPen pen1(QBrush(QColor(255,255,0)),3);
    QPen pen2(QBrush(QColor(255,0,0)),3);
    QPen pen3(QBrush(QColor(0,0,255)),3);

    if((AKT_Y_KEY != 0xFF) && (AKT_X_KEY != 0xFF))
    {
        painter.setPen(pen3);
        painter.drawRect(X_MIN[AKT_Y_KEY][AKT_X_KEY] * scaling_x,
                         Y_MIN[AKT_Y_KEY] * scaling_y,
                         (X_MAX[AKT_Y_KEY][AKT_X_KEY]* scaling_x) - (X_MIN[AKT_Y_KEY][AKT_X_KEY] * scaling_x),
                         (Y_MAX[AKT_Y_KEY] * scaling_y) - (Y_MIN[AKT_Y_KEY] * scaling_y));
    }

    for(int y=0;y<7;y++)
    {
        for(int x=0;x<18;x++)
        {
            if(VK_RAST[y][x])
            {
                painter.setPen(pen1);
                painter.drawRect(X_MIN[y][x] * scaling_x,
                               Y_MIN[y] * scaling_y,
                               (X_MAX[y][x]* scaling_x) - (X_MIN[y][x] * scaling_x),
                               (Y_MAX[y] * scaling_y) - (Y_MIN[y] * scaling_y));
            }
        }
    }
    if(Recording)
    {
        painter.setPen(pen2);
        painter.drawRect(0,0,this->width(),this->height());
        if(RecKeyPress)
        {
            if(blink_flip)
            {
                painter.setPen(pen2);
                painter.drawRect(X_MIN[RecKeyAktY][RecKeyAktX] * scaling_x,
                               Y_MIN[RecKeyAktY] * scaling_y,
                               (X_MAX[RecKeyAktY][RecKeyAktX]* scaling_x) - (X_MIN[RecKeyAktY][RecKeyAktX] * scaling_x),
                               (Y_MAX[RecKeyAktY] * scaling_y) - (Y_MIN[RecKeyAktY] * scaling_y));
            }
        }
    }
    painter.end();
}

void C64KeyboardWindow::timer_event(void)
{
    RecTimeOut--;
    if(RecTimeOut == 0 || c64->GetRecKeyMapStatus() == false)
    {
        timer->stop();
        c64->StopRecKeyMap();
        RecKeyPress = false;
        blink_flip = false;
    }
    else
    {
        if(blink_flip)
        {
                blink_flip = false;
        }
        else blink_flip = true;
    }
    update();
}
