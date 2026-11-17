//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: memory_window.cpp                     //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 18.06.2023                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QScreen>
#include <QMessageBox>
#include <QStyle>
#include <QWheelEvent>

#include "memory_window.h"
#include "qdebug.h"
#include "ui_memory_window.h"

MemoryWindow::MemoryWindow(QWidget *parent, QSettings* ini) :
    QDialog(parent),
    ui(new Ui::MemoryWindow),
    AktViewAdresse(0),
    NoFocusRun(true),
    ini(ini),
    font_size(10),
    wheel_zoom_delta(0)
{
    c64 = 0;

    memory_rows = 0;

    ui->setupUi(this);

    // Center Window
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), QGuiApplication::screens()[0]->availableGeometry()));

    ui->MemoryTable->setColumnCount(1);

    MemScrDest << "RAM" << "KERNAL" << "BASIC" << "VIC" << "FARBRAM" << "SID" << "CIA1" << "CIA2" << "IO1" << "IO2" << "CHARROM" << "ROM-LO" << "ROM-HI" << "ROM-HI" << "ADR.OPEN";
    FloppyMemScrDest << "RAM" << "VIA1" << "VIA2" << "ROM" << "OPEN";

    for(int i=0;i<256;++i)
    {
        MapReadSource[i] = FMV_OPEN;
        MapWriteDestination[i] = FMV_OPEN;
    }

    for(int i=0;i<8;i++)
    {
        MapReadSource[i] = FMV_RAM;
        MapWriteDestination[i] = FMV_RAM;
    }
    MapReadSource[0x18] = FMV_VIA1;
    MapWriteDestination[0x18] = FMV_VIA1;

    MapReadSource[0x1C] = FMV_VIA2;
    MapWriteDestination[0x1C] = FMV_VIA2;

    for(int i=0;i<64;i++)
    {
        MapReadSource[i+0xC0] = FMV_DOS;
        MapWriteDestination[i+0xC0] = FMV_OPEN;
    }

    for(int i=0; i<MAX_MEMORY_ROW; i++)
    {
        memory_row[i] = nullptr;
    }

    int id = QFontDatabase::addApplicationFont(":/fonts/lucon.ttf");

    QString family = "Lucida Console"; // Fallback
    if (id != -1) {
        const QStringList families = QFontDatabase::applicationFontFamilies(id);
        if (!families.isEmpty())
            family = families.first(); // echte Familie aus der TTF
    }

    memory_font = QFont(family, 10);

    for(int i=0; i<MAX_MEMORY_ROW; i++) memory_row[i] = nullptr;

    if(ini != nullptr)
    {
        ini->beginGroup("MemoryWindow");
        font_size = qBound(6, ini->value("FontSize", 10).toInt(), 40);
        ini->endGroup();
    }

    // Kopfzeile (Zeile 0) und eine Datenzeile; die weiteren Zeilen entstehen passend zur Fensterhoehe
    SetRowCount(2);
    ApplyFontSize(font_size);

    bool geometry_restored = false;
    if(ini != nullptr)
    {
        ini->beginGroup("MemoryWindow");
        if(ini->contains("Geometry")) geometry_restored = restoreGeometry(ini->value("Geometry").toByteArray());
        ini->endGroup();
    }

    if(!geometry_restored)
    {
        // Ohne gespeicherte Groesse zeigt das Fenster zunaechst 17 Zeilen (Kopfzeile + 16 Datenzeilen)
        int row_height = memory_row[0]->height() + 1;
        ui->MemoryTable->setMinimumHeight(17 * row_height + 2 * ui->MemoryTable->frameWidth());
        layout()->activate();
        resize(width(), minimumSizeHint().height());
        ui->MemoryTable->setMinimumHeight(3 * row_height);
    }

    // Mausrad scrollt ueber die Bildlaufleiste (auch ueber den Zeilen-Widgets, die das Rad weiterreichen)
    ui->MemoryTable->installEventFilter(this);
    ui->MemoryTable->viewport()->installEventFilter(this);
}

bool MemoryWindow::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == ui->MemoryTable || obj == ui->MemoryTable->viewport())
    {
        if(event->type() == QEvent::Wheel)
        {
            QWheelEvent* wheel_event = static_cast<QWheelEvent*>(event);

            if(wheel_event->modifiers() & Qt::ControlModifier)
            {
                // Strg + Mausrad: Schriftgroesse aendern (hochaufloesende Raeder werden aufsummiert)
                wheel_zoom_delta += wheel_event->angleDelta().y();
                int notches = wheel_zoom_delta / 120;
                wheel_zoom_delta -= notches * 120;
                if(notches != 0) ApplyFontSize(font_size + notches);
            }
            else
            {
                // Mausrad scrollt ueber die Bildlaufleiste (auch ueber den Zeilen-Widgets, die das Rad weiterreichen)
                QCoreApplication::sendEvent(ui->MemoryScroll, event);
            }
            return true;
        }

        if(event->type() == QEvent::Resize && obj == ui->MemoryTable->viewport())
            UpdateRowCountFromSize();
    }
    return QDialog::eventFilter(obj, event);
}

// Legt die Zeilen an bzw. entfernt sie. Zeile 0 ist die (leere) Kopfzeile, danach folgen die Datenzeilen.
void MemoryWindow::SetRowCount(int rows)
{
    rows = qBound(2, rows, static_cast<int>(MAX_MEMORY_ROW));
    if(rows == memory_rows) return;

    if(rows < memory_rows)
    {
        // Die Zeilen-Widgets werden von der Tabelle geloescht
        for(int i=rows; i<memory_rows; i++) memory_row[i] = nullptr;
        ui->MemoryTable->setRowCount(rows);
    }
    else
    {
        ui->MemoryTable->setRowCount(rows);

        for(int i=memory_rows; i<rows; i++)
        {
            memory_row[i] = new WidgetMemoryZeile(&memory_font, this);
            ui->MemoryTable->setCellWidget(i,0,memory_row[i]);
            ui->MemoryTable->setRowHeight(i,memory_row[i]->height()+1);
            connect(memory_row[i],SIGNAL(ChangeValue(unsigned short,unsigned char)),this,SLOT(onChangeValue(unsigned short,unsigned char)));
            connect(this,SIGNAL(NoFocus()),memory_row[i],SLOT(onNoFocus()));
        }
        memory_row[0]->setEnabled(false);
    }

    memory_rows = static_cast<int16_t>(rows);

    // Letzte moegliche Startadresse, sodass die Datenzeilen noch komplett in $FFFF enden
    int data_rows = memory_rows - 1;
    ui->MemoryScroll->setPageStep(data_rows * 16);
    ui->MemoryScroll->setMaximum(0xFFFF - (data_rows - 1) * 16);

    UpdateMemoryList();
}

void MemoryWindow::UpdateRowCountFromSize()
{
    if(memory_row[0] == nullptr) return;

    int row_height = memory_row[0]->height() + 1;
    SetRowCount(ui->MemoryTable->viewport()->height() / row_height);
}

// Aendert die Schrift aller Zeilen; Spaltenbreite, Zeilenhoehe und Fensterbreite passen sich an
void MemoryWindow::ApplyFontSize(int size)
{
    size = qBound(6, size, 40);
    font_size = size;
    memory_font.setPointSize(size);

    for(int i=0; i<memory_rows; i++)
    {
        memory_row[i]->SetFont(memory_font);
        ui->MemoryTable->setRowHeight(i, memory_row[i]->height()+1);
    }

    int row_width = memory_row[0]->width();
    int row_height = memory_row[0]->height() + 1;

    ui->MemoryTable->setColumnWidth(0, row_width);
    ui->MemoryTable->setMinimumWidth(row_width);
    ui->MemoryTable->setMaximumWidth(row_width);
    ui->MemoryTable->setMinimumHeight(row_height * 3);

    // Fensterbreite an die neue Zeilenbreite anpassen
    if(layout() != nullptr) layout()->activate();
    resize(minimumSizeHint().width(), height());

    UpdateRowCountFromSize();
}

MemoryWindow::~MemoryWindow()
{
    if(ini != nullptr)
    {
        ini->beginGroup("MemoryWindow");
        ini->setValue("Geometry", saveGeometry());
        ini->setValue("FontSize", font_size);
        ini->endGroup();
    }

    delete ui;
}

void MemoryWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
    ChangeSource(AktSource);
}

void MemoryWindow::SetC64Pointer(C64Class *_c64)
{
    c64 = _c64;
}

void MemoryWindow::ChangeSource(int source)
{
    AktSource = source;
    if(AktSource > 0)
    {
        AktFloppyNr = AktSource - 1;
        ui->OnlyRam->setEnabled(false);
    }
    else ui->OnlyRam->setEnabled(true);

    switch(AktSource)
    {
    case 0:
        setWindowTitle(QString(tr("C64 Speicher")));
        break;
    case 1:
        setWindowTitle(QString(tr("Floppy #08 Speicher")));
        break;
    case 2:
        setWindowTitle(QString(tr("Floppy #09Speicher")));
        break;
    case 3:
        setWindowTitle(QString(tr("Floppy #10 Speicher")));
        break;
    case 4:
        setWindowTitle(QString(tr("Floppy #11 Speicher")));
        break;
    }
}

void MemoryWindow::onChangeValue(unsigned short adresse, unsigned char wert)
{
    if(AktSource > 0)
    {
        c64->floppy[AktFloppyNr]->WriteByte(adresse,wert);
    }
    else
    {
        c64->WriteC64Byte(adresse,wert);
    }
}

void MemoryWindow::UpdateMemoryList(void)
{
    if(c64 == 0) return;
    if(isHidden()) return;

    WidgetMemoryZeile *w;
    unsigned char puffer[16];
    unsigned char* ram_puffer;

    for(int i=0;i<memory_rows-1;i++)
    {
        if(AktSource > 0)
        {
            for(int x=0;x<16;x++) puffer[x] = c64->floppy[AktFloppyNr]->ReadByte(AktViewAdresse + (i*16) + x);
        }
        else
        {
            if(ui->OnlyRam->isChecked())
            {
                ram_puffer = c64->GetRAMPointer(AktViewAdresse + (i*16));
                for(int x=0;x<16;x++) puffer[x] = ram_puffer[x];
            }
            else for(int x=0;x<16;x++) puffer[x] = c64->ReadC64Byte(AktViewAdresse + (i*16) + x);
        }
        w = (WidgetMemoryZeile*)ui->MemoryTable->cellWidget(i+1,0);

        unsigned char page = (AktViewAdresse + (i*16)) >> 8;
        unsigned char mem_read_source;
        unsigned char mem_write_destination;

        if(AktSource > 0)
        {
            mem_read_source = MapReadSource[page];
            mem_write_destination = MapWriteDestination[page];
            w->Fill(AktViewAdresse + (i*16),puffer,FloppyMemScrDest[mem_read_source],FloppyMemScrDest[mem_write_destination]);
        }
        else
        {
            mem_read_source = c64->GetMapReadSource(page);
            mem_write_destination = c64->GetMapWriteDestination(page);
            w->Fill(AktViewAdresse + (i*16),puffer,MemScrDest[mem_read_source],MemScrDest[mem_write_destination]);
        }
    }
}

void MemoryWindow::on_MemoryScroll_valueChanged(int value)
{
    unsigned short v = value / 16;
    AktViewAdresse = v * 16;
    if(NoFocusRun) emit NoFocus();
    UpdateMemoryList();
}

void MemoryWindow::on_MemoryScroll_sliderPressed()
{
    emit NoFocus();
    NoFocusRun = false;
}

void MemoryWindow::on_MemoryScroll_sliderReleased()
{
    NoFocusRun = true;
}

void MemoryWindow::on_OnlyRam_clicked(bool)
{
    UpdateMemoryList();
}

void MemoryWindow::on_jump_address_returnPressed()
{
    bool ok;
    QString in_str;
    int value;

    in_str = ui->jump_address->text();

    if(in_str.left(1) == "$") in_str.replace(0,1,"0x"); // Steht am Anfang ein '$' wird dieses in '0X' gewandelt
    value = in_str.toInt(&ok,0);
    if(!ok)
    {
        QMessageBox::warning(this,tr("Eingabefehler..."),tr("Es wurde kein gültiges Zahlenformat benutzt !"));
            return;
    }

    if(value < 0 || value > 0xffff)
            QMessageBox::warning(this, tr("Eingabefehler..."), tr("Die Zieladresse muss zwischen $0000 (0) und $FFFF (65535) liegen"));

    ui->MemoryScroll->setValue(value);

    // Zeile der Adresse relativ zur (durch die Leiste begrenzten) Startadresse; Zeile 0 ist die Kopfzeile
    int row = 1 + (value - AktViewAdresse) / 16;
    if(row >= 1 && row < memory_rows && memory_row[row] != nullptr)
        memory_row[row]->SelectColumb(value % 16);
}

