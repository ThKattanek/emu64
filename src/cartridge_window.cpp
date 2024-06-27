//////////////////////////////////////////////////
//                                              //
// Emu64                                        //
// von Thorsten Kattanek                        //
//                                              //
// #file: cartridge_window.cpp                  //
//                                              //
// Dieser Sourcecode ist Copyright geschützt!   //
// Geistiges Eigentum von Th.Kattanek           //
//                                              //
// www.emu64.de                                 //
//                                              //
//////////////////////////////////////////////////

#include <QMessageBox>
#include <QFontDatabase>
#include <QWidget>

#include "./cartridge_window.h"
#include "./ui_cartridge_window.h"
#include "./utils.h"

CartridgeWindow::CartridgeWindow(QWidget *parent, QSettings *_ini, C64Class *c64, QString tmp_path) :
    QDialog(parent),
    ui(new Ui::CartridgeWindow)
{
    ini = _ini;

    this->c64 = c64;
    crt = c64->crt;
    this->tmp_path = tmp_path;

    ui->setupUi(this);

    ui->ExportChip->setDisabled(true);
    ui->FileBrowser->SetTempDir(tmp_path);

    LedRedOff = new QIcon(":/grafik/red_led_off.png");
    LedRedOn = new QIcon(":/grafik/red_led_on.png");
    ui->FC3_LED->setIcon(*LedRedOff);
    ui->EF_LED->setIcon(*LedRedOff);

    for(int i=0; i<LED_NUM; i++)
        LedStatus[i] = LedStatusOld[i] = false;

    ui->PageEasyFlash->setEnabled(false);
    ui->PageFC->setEnabled(false);
    ui->PageAR->setEnabled(false);

    ui->MoreCRTPage->setMinimumWidth(0);

    QFontDatabase fontDB;
    fontDB.addApplicationFont(":/fonts/lucon.ttf");
    QFont font1("Lucida Console",9);

    ui->ChipData->setFont(font1);
    ui->ChipList->setFont(font1);
    ui->CRTInfo->setFont(font1);

    /// ChipList
    ui->ChipList->setColumnCount(5);
    ui->ChipList->setHeaderLabels(QStringList() << tr("Nr.") << tr("Chip Typ") << tr("Bank") << tr("Adresse") << tr("Größe"));

    int totalWidth = 0;
    for (int i = 0; i < ui->ChipList->columnCount(); i++)
    {
        //ui->ChipList->headerItem()->setTextAlignment(i, Qt::AlignCenter);
        ui->ChipList->header()->setSectionResizeMode(i, QHeaderView::Stretch);
        ui->ChipList->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        totalWidth += ui->ChipList->columnWidth(i);
    }

    ui->ChipList->setMinimumWidth(totalWidth);
    ui->CRTInfo->setMinimumWidth(totalWidth);

    /// CRTInfo
    ui->CRTInfo->setColumnCount(2);
    ui->CRTInfo->setHeaderLabels(QStringList() << tr("Variable") << tr("Inhalt"));

    QStringList crt_info_vnames;
    crt_info_vnames << tr("Name") << tr("Version") << tr("Hardware Typ") << tr("EXROM Leitung") << tr("GAME Leitung") << tr("Chip Anzahl");

    for(int i=0; i < crt_info_vnames.count(); i++)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(ui->CRTInfo);
        item->setText(0, crt_info_vnames[i]);
        ui->CRTInfo->addTopLevelItem(item);
    }

    for (int i = 0; i < ui->CRTInfo->columnCount(); i++)
    {
        ui->CRTInfo->header()->setSectionResizeMode(i, QHeaderView::Stretch);
        ui->CRTInfo->header()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
    }

    ui->CRTInfo->setMinimumHeight(170);
    ui->CRTInfo->setMaximumHeight(170);

    connect(ui->FileBrowser,SIGNAL(SelectFile(QString)),this,SLOT(onSelectFile(QString)));
    connect(ui->ChipList->selectionModel(),SIGNAL(currentChanged(QModelIndex,QModelIndex)),this,SLOT(onChipList_currentChanged(QModelIndex,QModelIndex)));
    connect(ui->FileBrowser,SIGNAL(FileDoubleClick()),this,SLOT(on_InsertCRT_clicked()));

    ui->FileBrowser->SetFileFilter(QStringList()<<"*.crt");

    isOneShowed = false;
    ChipDataExpand = false;
    CRTIsSelected = false;
    win_exp = false;
    insterted_hwtyp = 0;

    ui->CRTInfo->setColumnWidth(0,80);

    ui->ChipData->setColumnCount(17);
    ui->ChipData->setHeaderLabels(QStringList() << "ADR"<<"0"<<"1"<<"2"<<"3"<<"4"<<"5"<<"6"<<"7"<<"8"<<"9"<<"A"<<"B"<<"C"<<"D"<<"E"<<"F");
    ui->ChipData->setColumnWidth(0,50);
    for(int i=0;i<16;i++) ui->ChipData->setColumnWidth(i+1,24);

    ChangeLED(1, false);

    timer1 = new QTimer(this);
    timer1->setInterval(20);
    connect(timer1,SIGNAL(timeout()),this,SLOT(onTimer1()));
    timer1->start();
}

CartridgeWindow::~CartridgeWindow()
{
    if(win_exp)
    {
        this->setMinimumWidth(this->minimumWidth()-200);
        this->resize(this->width()-200,this->height());
    }
    ////////// Save to INI ///////////
    if(ini != nullptr)
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

void CartridgeWindow::LoadIni(void)
{
    ////////// Load from INI ///////////
    if(ini != nullptr)
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

bool CartridgeWindow::SetCartridgeImage(QString filename)
{
    QFileInfo fi = QFileInfo(filename);

    ui->FileBrowser->SetAktDir(fi.absolutePath());
    ui->FileBrowser->SetAktFile(fi.absolutePath(),fi.fileName());
    return CRTIsSelected;
}

void CartridgeWindow::ConnectCrt()
{
    on_InsertCRT_clicked();
}

void CartridgeWindow::DisconnectCrt()
{
    on_RemoveCRT_clicked();
}

void CartridgeWindow::showEvent(QShowEvent*)
{
    isOneShowed = true;
    timer1->start();
}

void CartridgeWindow::hideEvent(QHideEvent*)
{
    timer1->stop();
}

void CartridgeWindow::RetranslateUi()
{
    ui->retranslateUi(this);
    this->update();
}

void CartridgeWindow::on_ViewChipData_clicked()
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

void CartridgeWindow::onSelectFile(QString filename)
{
    char str00[256];

    if(crt == nullptr) return;

	FILE *file = qfopen(filename, "rb");
	if(file != nullptr)
	{
		int res = crt->GetCartridgeInfo(file,&crt_info);
		switch(res)
		{
		case 0:
			CRTIsSelected = true;
            ui->ExportChip->setDisabled(false);

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
                sprintf(str00,"%3.3d",i);
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
                    this->resize(this->width()+250,this->height());
                    this->setMinimumWidth(this->minimumWidth()+250);
				}
                ui->MoreCRTPage->setMinimumWidth(250);
				ui->MoreCRTPage->setCurrentIndex(2);
				win_exp = true;
				break;
			case 3: // FC_III
				if(!win_exp)
				{
                    this->resize(this->width()+250,this->height());
                    this->setMinimumWidth(this->minimumWidth()+250);
				}
                ui->MoreCRTPage->setMinimumWidth(250);
				ui->MoreCRTPage->setCurrentIndex(1);
				win_exp = true;
				break;
			case 32: // EasyFlash
				if(!win_exp)
				{
                    this->resize(this->width()+250,this->height());
                    this->setMinimumWidth(this->minimumWidth()+250);
				}
                ui->MoreCRTPage->setMinimumWidth(250);
				ui->MoreCRTPage->setCurrentIndex(0);
				win_exp = true;
				break;
			default:
				if(win_exp)
				{
                    this->setMinimumWidth(this->minimumWidth()-250);
                    this->resize(this->width()-250,this->height());
				}
				ui->MoreCRTPage->setMinimumWidth(0);
				win_exp = false;
				break;
			}

			break;
		case 1:
			CRTIsSelected = false;
            ui->ExportChip->setDisabled(true);
			/* Fehler beim öffnene */
			break;
		case 2:
			CRTIsSelected = false;
            ui->ExportChip->setDisabled(true);
            /* Unbekanntes Format */
			break;
		case 3:
			CRTIsSelected = false;
            ui->ExportChip->setDisabled(true);
			/* Kein ROM Image */
			break;
		}
	}
}

void CartridgeWindow::onChipList_currentChanged(const QModelIndex &current, const QModelIndex &)
{
    char str00[256];
    ui->ChipData->clear();
    if(!current.isValid() || !ChipDataExpand) return;

    int adresse = 0;
    uint8_t *ChipRom = crt_info.ChipInfo[current.row()].BufferPointer;
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

void CartridgeWindow::onTimer1()
{
    if(LedStatusOld[0] != LedStatus[0])
    {
        if(LedStatus[0]) ui->FC3_LED->setIcon(*LedRedOn);
        else ui->FC3_LED->setIcon(*LedRedOff);
    }

    if(LedStatusOld[1] != LedStatus[1])
    {
        if(LedStatus[1]) ui->EF_LED->setIcon(*LedRedOn);
        else ui->EF_LED->setIcon(*LedRedOff);
    }

    for(int i=0; i<LED_NUM; i++)
        LedStatusOld[i] = LedStatus[i];
}

void CartridgeWindow::on_NewEasyFlashCRT_clicked()
{
    CRTNewEasyflashWindow *crt_new_ef_window = new CRTNewEasyflashWindow(this);

    if(crt_new_ef_window->exec())
    {
        QString filename = crt_new_ef_window->GetFilename();
        QString cartridge_name = crt_new_ef_window->GetCartridgeName();
        QString fullpath;

        if(filename.right(4).toUpper() != ".CRT")
            filename += ".crt";

        fullpath = ui->FileBrowser->GetAktDir() + "/" + filename;

        QFile file(fullpath);
        if(!file.exists())
        {
			FILE *fp = qfopen(fullpath, "wb");
			if(c64->CreateNewEasyFlashImage(fp, cartridge_name.toLocal8Bit()))
            {
                QMessageBox::critical(this,tr("Fehler!"),tr("Es konnte kein neues EasyFlash Image erstellt werden."));
            }
            else
            {
                ui->FileBrowser->SetAktDir(ui->FileBrowser->GetAktDir());
                ui->FileBrowser->SetAktFile(ui->FileBrowser->GetAktDir(),filename);
            }
        }
        else
        {
            if(QMessageBox::Yes == QMessageBox::question(this,tr("Achtung!"),tr("Eine Datei mit diesen Namen existiert schon!\nSoll diese überschrieben werden?"),QMessageBox::Yes | QMessageBox::No))
            {
				FILE *fp = qfopen(fullpath, "wb");
				if(c64->CreateNewEasyFlashImage(fp, cartridge_name.toLocal8Bit()))
                {
                    QMessageBox::critical(this,tr("Fehler!"),tr("Es konnte kein neues EasyFlash Image erstellt werden."));
                }
                else
                {
                    ui->FileBrowser->SetAktDir(ui->FileBrowser->GetAktDir());
                    ui->FileBrowser->SetAktFile(ui->FileBrowser->GetAktDir(),filename);
                }
            }
        }
    }

    delete crt_new_ef_window;
}

void CartridgeWindow::on_InsertCRT_clicked()
{
    if(CRTIsSelected)
    {
		FILE *file = qfopen(SelCRTFileName, "rb");
		if(file != nullptr)
		{
			if(0 == c64->LoadCRT(file))
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
			else QMessageBox::warning(this,tr("CRT Fehler"),tr("Fehler beim Laden des ausgewählten CRT Files"));

			fclose(file);
		}
		else QMessageBox::critical(this,"ERROR","Fehler beim öffnen des CRT");
    }
}

void CartridgeWindow::on_RemoveCRT_clicked()
{
    ui->PageFC->setEnabled(false);
    ui->PageEasyFlash->setEnabled(false);
    ui->PageAR->setEnabled(false);
    c64->RemoveCRT();
}

void CartridgeWindow::ChangeLED(int LedNr, bool LedStatus)
{
    if(LedNr < LED_NUM)
    {
        this->LedStatus[LedNr] = LedStatus;
    }
}

void CartridgeWindow::on_FreezButtonFC3_clicked()
{
    crt->Freeze();
}

void CartridgeWindow::on_EF_JUMPER0_toggled(bool checked)
{
    crt->SetEasyFlashJumper(!checked);
}

void CartridgeWindow::on_FreezButtonAR_clicked()
{
    crt->Freeze();
}

void CartridgeWindow::on_ExportChip_clicked()
{
    QString filename;
    QString fileext;

    QStringList filters;
    filters << tr("ChipRom Datei (*.bin)")
            << tr("Alle Dateien (*.*)");

    if(!CustomSaveFileDialog::GetSaveFileName(this, tr("ChipRom speichern"), filters, &filename, &fileext))
    {
        return;
    }
    return;

    int chip_nr = ui->ChipList->currentIndex().row();

    uint8_t *ChipRom = crt_info.ChipInfo[chip_nr].BufferPointer;
    int ChipSize = crt_info.ChipInfo[chip_nr].ChipSize;

    FILE *file = qfopen(filename, "wb");
    if(file != nullptr)
    {
        fwrite(ChipRom, ChipSize, 1, file);
        fclose(file);
    }
}

