/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/main_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MainWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      36,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      16,   12,   11,   11, 0x0a,
      39,   11,   11,   11, 0x0a,
      48,   11,   11,   11, 0x08,
      85,   78,   11,   11, 0x08,
     115,   11,   11,   11, 0x08,
     144,   11,   11,   11, 0x08,
     175,   11,   11,   11, 0x08,
     206,   11,   11,   11, 0x08,
     250,   11,   11,   11, 0x08,
     286,   11,   11,   11, 0x08,
     334,   11,   11,   11, 0x08,
     378,   11,   11,   11, 0x08,
     411,   11,   11,   11, 0x08,
     454,   11,   11,   11, 0x08,
     485,   11,   11,   11, 0x08,
     533,   11,   11,   11, 0x08,
     574,   11,   11,   11, 0x08,
     606,   11,   11,   11, 0x08,
     647,   11,   11,   11, 0x08,
     683,   11,   11,   11, 0x08,
     718,   11,   11,   11, 0x08,
     749,   11,   11,   11, 0x08,
     784,   11,   11,   11, 0x08,
     818,   11,   11,   11, 0x08,
     854,   11,   11,   11, 0x08,
     889,   11,   11,   11, 0x08,
     920,   11,   11,   11, 0x08,
     955,   11,   11,   11, 0x08,
     989,   11,   11,   11, 0x08,
    1019,   11,   11,   11, 0x08,
    1105, 1058,   11,   11, 0x08,
    1159, 1150,   11,   11, 0x08,
    1184,   11,   11,   11, 0x08,
    1218, 1211,   11,   11, 0x08,
    1237,   11,   11,   11, 0x08,
    1282,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MainWindow[] = {
    "MainWindow\0\0msg\0OnMessage(QStringList)\0"
    "OnInit()\0on_menu_main_info_triggered()\0"
    "action\0slotLanguageChanged(QAction*)\0"
    "on_actionBeenden_triggered()\0"
    "on_actionSoftreset_triggered()\0"
    "on_actionAutostart_triggered()\0"
    "on_actionTV_Video_Einstellungen_triggered()\0"
    "on_actionFloppy_1541_II_triggered()\0"
    "on_actionC64_Tastaturbelegung_ndern_triggered()\0"
    "on_actionVirtuelle_C64_Tastatur_triggered()\0"
    "on_actionSteckmodule_triggered()\0"
    "on_actionDebugger_Disassembler_triggered()\0"
    "on_actionHardreset_triggered()\0"
    "on_actionC64_Programme_direkt_laden_triggered()\0"
    "on_actionEmu64_Einstellungen_triggered()\0"
    "on_actionScreenshot_triggered()\0"
    "on_actionC64_Geschwindigkeit_triggered()\0"
    "on_actionREU_einstecken_triggered()\0"
    "on_actionREU_entfernen_triggered()\0"
    "on_actionREU_laden_triggered()\0"
    "on_actionREU_speichern_triggered()\0"
    "on_actionREU_loeschen_triggered()\0"
    "on_actionGEO_einstecken_triggered()\0"
    "on_actionGEO_entfernen_triggered()\0"
    "on_actionGEO_laden_triggered()\0"
    "on_actionGEO_speichern_triggered()\0"
    "on_actionGEO_loeschen_triggered()\0"
    "on_actionVollbild_triggered()\0"
    "on_actionBandlaufwerk_1530_triggered()\0"
    "fullscreen,palmode,doublemode,bit32mode,filter\0"
    "OnChangeGrafikModi(bool,bool,bool,bool,bool)\0"
    "floppynr\0OnChangeFloppyImage(int)\0"
    "OnResetScreenshotCounter()\0result\0"
    "OnSetupFished(int)\0"
    "on_actionC64_Tastenbelegung_Show_triggered()\0"
    "on_actionVideo_Capture_triggered()\0"
};

void MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MainWindow *_t = static_cast<MainWindow *>(_o);
        switch (_id) {
        case 0: _t->OnMessage((*reinterpret_cast< QStringList(*)>(_a[1]))); break;
        case 1: _t->OnInit(); break;
        case 2: _t->on_menu_main_info_triggered(); break;
        case 3: _t->slotLanguageChanged((*reinterpret_cast< QAction*(*)>(_a[1]))); break;
        case 4: _t->on_actionBeenden_triggered(); break;
        case 5: _t->on_actionSoftreset_triggered(); break;
        case 6: _t->on_actionAutostart_triggered(); break;
        case 7: _t->on_actionTV_Video_Einstellungen_triggered(); break;
        case 8: _t->on_actionFloppy_1541_II_triggered(); break;
        case 9: _t->on_actionC64_Tastaturbelegung_ndern_triggered(); break;
        case 10: _t->on_actionVirtuelle_C64_Tastatur_triggered(); break;
        case 11: _t->on_actionSteckmodule_triggered(); break;
        case 12: _t->on_actionDebugger_Disassembler_triggered(); break;
        case 13: _t->on_actionHardreset_triggered(); break;
        case 14: _t->on_actionC64_Programme_direkt_laden_triggered(); break;
        case 15: _t->on_actionEmu64_Einstellungen_triggered(); break;
        case 16: _t->on_actionScreenshot_triggered(); break;
        case 17: _t->on_actionC64_Geschwindigkeit_triggered(); break;
        case 18: _t->on_actionREU_einstecken_triggered(); break;
        case 19: _t->on_actionREU_entfernen_triggered(); break;
        case 20: _t->on_actionREU_laden_triggered(); break;
        case 21: _t->on_actionREU_speichern_triggered(); break;
        case 22: _t->on_actionREU_loeschen_triggered(); break;
        case 23: _t->on_actionGEO_einstecken_triggered(); break;
        case 24: _t->on_actionGEO_entfernen_triggered(); break;
        case 25: _t->on_actionGEO_laden_triggered(); break;
        case 26: _t->on_actionGEO_speichern_triggered(); break;
        case 27: _t->on_actionGEO_loeschen_triggered(); break;
        case 28: _t->on_actionVollbild_triggered(); break;
        case 29: _t->on_actionBandlaufwerk_1530_triggered(); break;
        case 30: _t->OnChangeGrafikModi((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5]))); break;
        case 31: _t->OnChangeFloppyImage((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 32: _t->OnResetScreenshotCounter(); break;
        case 33: _t->OnSetupFished((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 34: _t->on_actionC64_Tastenbelegung_Show_triggered(); break;
        case 35: _t->on_actionVideo_Capture_triggered(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MainWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MainWindow::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_MainWindow,
      qt_meta_data_MainWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MainWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MainWindow))
        return static_cast<void*>(const_cast< MainWindow*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 36)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 36;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
