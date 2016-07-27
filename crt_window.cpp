//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: crt_window.cpp                        //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// Letzte Änderung am 27.07.2016                //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QMessageBox>
#include <QFontDatabase>

#include "crt_window.h"
#include "ui_crt_window.h"

CrtWindow::CrtWindow(QWidget *parent, QSettings *_ini, C64Class *c64) :
    QDialog(parent),
    ui(new Ui::CrtWindow)
{
    ini = _ini;

    this->c64 = c64;
    crt = c64->crt;

    ui->setupUi(this);

    LedRedOff = new QIcon(":/grafik/RedLED_Off.png");
    LedRedOn = new QIcon(":/grafik/RedLED_On.png");
    ui->FC3_LED->setIcon(*LedRedOff);
    ui->EF_LED->setIcon(*LedRedOff);

    ui->PageEasyFlash->setEnabled(false);
    ui->PageFC->setEnabled(false);
    ui->PageAR->setEnabled(false);

    ui->MoreCRTPage->setMinimumWidth(0);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/lucon.ttf");
    QFont font("Lucida Console",8);

    connect(ui->FileBrowser,SIGNAL(select_file(QString)),this,SLOT(onSelectFile(QString)));
    connect(ui->ChipList->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(onChipList_currentChanged(QModelIndex,QModelIndex)));
    connect(ui->FileBrowser,SIGNAL(FileDoubleClick()),this,SLOT(on_InsertCRT_clicked()));

    ui->FileBrowser->SetFileFilter(QStringList()<<"*.crt");

    isOneShowed = false;
    ChipDataExpand = false;
    CRTIsSelected = false;
    win_exp = false;
    insterted_hwtyp = 0;

    ui->CRTInfo->setColumnWidth(0,80);

    ui->ChipList->setColumnWidth(0,25);
    ui->ChipList->setColumnWidth(1,50);
    ui->ChipList->setColumnWidth(2,35);
    ui->ChipList->setColumnWidth(3,50);
    ui->ChipList->setColumnWidth(4,50);

    ui->ChipData->setColumnCount(17);
    ui->ChipData->setHeaderLabels(QStringList() << "ADR"<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"A"<<"B"<<"C"<<"D"<<"E"<<"F");
    ui->ChipData->setColumnWidth(0,50);
    for(int i=0;i<16;i++) ui->ChipData->setColumnWidth(i+1,24);
}

CrtWindow::~CrtWindow()
{
    if(win_exp)
    {
        this->setMinimumWidth(this->minimumWidth()-200);
        this->resize(this->width()-200,this->height());
    }
    ////////// Save to INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("CrtWindow");
        if(isOneShowed) ini->setValue("Geometry",saveGeometry());
        if(isHidden()) ini->setValue("Show",false);
        else ini->setValue("Show",true);
        ini->setValue("ChipDataExpand",ChipDataExpand);
        ini->setValue("AktDir",ui->FileBrowser->GetAktDir());
        ini->setValue("AktFile",ui->FileBrowser->GetAktFile());
        ini->endGroup();
    }
    ////////////////////////////////////

    delete ui;
}

void CrtWindow::LoadIni(void)
{
    ////////// Load from INI ///////////
    if(ini != 0)
    {
        ini->beginGroup("CrtWindow");
        if(ini->contains("Geometry")) restoreGeometry(ini->value("Geometry").toByteArray());
        //if(ini->value("Show",false).toBool()) show();
        if(ini->value("ChipDataExpand",false).toBool())
        {
            ChipDataExpand = true;
            ui->ViewChipData->setText("-");

            ui->ChipData->setMinimumHeight(130);
            ui->ChipData->setMaximumHeight(130);
        }
        ui->FileBrowser->SetAktDir(ini->value("AktDir","").toString());
        ui->FileBrowser->SetAktFile(ini->value("AktDir","").toString(),ini->value("AktFile","").toString());
        ini->endGroup();
    }
    ////////////////////////////////////
}

void CrtWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
}

void CrtWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void CrtWindow::on_ViewChipData_clicked()
{
    if(ChipDataExpand)
    {
        ChipDataExpand = false;
        ui->ViewChipData->setText("+");

        ui->ChipData->setMinimumHeight(0);
        ui->ChipData->setMaximumHeight(0);

        QRect rect = geometry();
        rect.setHeight(rect.height()-130);
        this->setGeometry(rect);
    }
    else
    {
        ChipDataExpand = true;
        ui->ViewChipData->setText("-");

        QRect rect = geometry();
        rect.setHeight(rect.height()+130);
        this->setGeometry(rect);

        ui->ChipData->setMinimumHeight(130);
        ui->ChipData->setMaximumHeight(130);

        QModelIndex index = ui->ChipList->currentIndex();
        onChipList_currentChanged(index,index);
    }
}

void CrtWindow::onSelectFile(QString filename)
{
    char str00[256];

    if(crt == 0) return;
    int res = crt->GetCRTInfo(filename.toLatin1().data(),&crt_info);
    switch(res)
    {
    case 0:
        CRTIsSelected = true;
        SelCRTFileName = filename;

        ui->CRTInfo->topLevelItem(0)->setText(1,crt_info.Name);
        ui->CRTInfo->topLevelItem(1)->setText(1,crt_info.Version);
        ui->CRTInfo->topLevelItem(2)->setText(1,"[" + QVariant(crt_info.HardwareType).toString() + "] " + crt_info.HardwareTypeString);
        ui->CRTInfo->topLevelItem(3)->setText(1,QVariant(crt_info.EXROM).toString());
        ui->CRTInfo->topLevelItem(4)->setText(1,QVariant(crt_info.GAME).toString());
        ui->CRTInfo->topLevelItem(5)->setText(1,QVariant(crt_info.ChipCount).toString());

        ui->ChipList->clear();
        for(int i=0;i<crt_info.ChipCount;i++)
        {
            QTreeWidgetItem *item = new QTreeWidgetItem(ui->ChipList);
            sprintf(str00,"%2.2d",i);
            item->setText(0,str00);
            item->setTextAlignment(0,Qt::AlignHCenter);

            switch(crt_info.ChipInfo[i].Type)
            {
            case 0:
                item->setText(1,"ROM");
                break;
            case 1:
                item->setText(1,"RAM");
                break;
            case 2:
                item->setText(1,"F-ROM");
                break;
            default:
                item->setText(1,"???");
                break;

            }
            item->setTextAlignment(1,Qt::AlignHCenter);

            sprintf(str00,"$%2.2X",crt_info.ChipInfo[i].BankLocation);
            item->setText(2,str00);
            item->setTextAlignment(2,Qt::AlignHCenter);

            sprintf(str00,"$%4.4X",crt_info.ChipInfo[i].LoadAdress);
            item->setText(3,str00);

            sprintf(str00,"$%4.4X",crt_info.ChipInfo[i].ChipSize);
            item->setText(4,str00);

            ui->ChipList->addTopLevelItem(item);
        }
        ui->ChipList->setCurrentIndex(ui->ChipList->model()->index(0,0,QModelIndex()));

        switch(crt_info.HardwareType)
        {
        case 1: // Action Replay
            if(!win_exp)
            {
                this->resize(this->width()+200,this->height());
                this->setMinimumWidth(this->minimumWidth()+200);
            }
            ui->MoreCRTPage->setMinimumWidth(200);
            ui->MoreCRTPage->setCurrentIndex(2);
            win_exp = true;
            break;
        case 3: // FC_III
            if(!win_exp)
            {
                this->resize(this->width()+200,this->height());
                this->setMinimumWidth(this->minimumWidth()+200);
            }
            ui->MoreCRTPage->setMinimumWidth(200);
            ui->MoreCRTPage->setCurrentIndex(1);
            win_exp = true;
            break;
        case 32: // EasyFlash
            if(!win_exp)
            {
                this->resize(this->width()+200,this->height());
                this->setMinimumWidth(this->minimumWidth()+200);
            }
            ui->MoreCRTPage->setMinimumWidth(200);
            ui->MoreCRTPage->setCurrentIndex(0);
            win_exp = true;
            break;
        default:
            if(win_exp)
            {
                this->setMinimumWidth(this->minimumWidth()-200);
                this->resize(this->width()-200,this->height());
            }
            ui->MoreCRTPage->setMinimumWidth(0);
            win_exp = false;
            break;
        }

        break;
    case 1:
        CRTIsSelected = false;
        /* Fehler beim öffnene */
        break;
    case 2:
        CRTIsSelected = false;
        /* Unbekanntes Format */
        break;
    case 3:
        CRTIsSelected = false;
        /* Kein ROM Image */
        break;
    }
}

void CrtWindow::onChipList_currentChanged(const QModelIndex &current, const QModelIndex &)
{
    char str00[256];
    ui->ChipData->clear();
    if(!current.isValid() || !ChipDataExpand) return;

    int adresse = 0;
    unsigned char *ChipRom = crt_info.ChipInfo[current.row()].BufferPointer;
    int ChipSizeDiv16 = crt_info.ChipInfo[current.row()].ChipSize/16;

    for(int i=0; i<ChipSizeDiv16; i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->ChipData);
        sprintf(str00,"$%4.4X",adresse);
        item->setText(0,str00);

        for (int j=0; j<16;j++)
        {
            sprintf(str00,"%2.2X",*ChipRom++);
            item->setText(j+1,str00);
        }
        adresse += 16;
        ui->ChipData->addTopLevelItem(item);
    }
}

void CrtWindow::on_NewEasyFlashCRT_clicked()
{

}

void CrtWindow::on_InsertCRT_clicked()
{
    if(CRTIsSelected)
    {
        if(0 == c64->LoadCRT(SelCRTFileName.toLatin1().data()))
        {
            ui->PageFC->setEnabled(false);
            ui->PageEasyFlash->setEnabled(false);
            ui->PageAR->setEnabled(false);
            insterted_hwtyp = crt_info.HardwareType;
            switch(insterted_hwtyp)
            {
            case 1:
                ui->PageAR->setEnabled(true);
                break;
            case 3:
                ui->PageFC->setEnabled(true);
                break;
            case 32:
                ui->PageEasyFlash->setEnabled(true);
                break;
            }
        }
        else QMessageBox::warning(this,trUtf8("CRT Fehler"),trUtf8("Fehler beim Laden des ausgewählten CRT Files"));
    }
}

void CrtWindow::on_RemoveCRT_clicked()
{
    ui->PageFC->setEnabled(false);
    ui->PageEasyFlash->setEnabled(false);
    ui->PageAR->setEnabled(false);
    c64->RemoveCRT();
}

void CrtWindow::ChangeLED(int LedNr, bool LedStatus)
{
    switch(LedNr)
    {
    case 0: /// FC3
        if(LedStatus) ui->FC3_LED->setIcon(*LedRedOn);
        else ui->FC3_LED->setIcon(*LedRedOff);
        break;
    case 1: /// EF
        if(LedStatus) ui->EF_LED->setIcon(*LedRedOn);
        else ui->EF_LED->setIcon(*LedRedOff);
        break;
    }
}

void CrtWindow::on_FreezButtonFC3_clicked()
{
    crt->Freeze();
}

void CrtWindow::on_EF_JUMPER0_toggled(bool checked)
{
    crt->SetEasyFlashJumper(!checked);
}

void CrtWindow::on_FreezButtonAR_clicked()
{
    crt->Freeze();
}
