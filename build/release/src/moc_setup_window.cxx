/****************************************************************************
** Meta object code from reading C++ file 'setup_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/setup_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'setup_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SetupWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      28,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      60,   13,   12,   12, 0x05,
     103,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
     133,  127,   12,   12, 0x08,
     182,  174,   12,   12, 0x08,
     204,  174,   12,   12, 0x08,
     229,  174,   12,   12, 0x08,
     253,  174,   12,   12, 0x08,
     277,  174,   12,   12, 0x08,
     302,   12,   12,   12, 0x08,
     344,  333,   12,   12, 0x08,
     378,  333,   12,   12, 0x08,
     412,  174,   12,   12, 0x08,
     442,  174,   12,   12, 0x08,
     472,  127,   12,   12, 0x08,
     517,  509,   12,   12, 0x08,
     546,  540,   12,   12, 0x08,
     577,  174,   12,   12, 0x08,
     613,  608,   12,   12, 0x08,
     652,  127,   12,   12, 0x08,
     692,  127,   12,   12, 0x08,
     733,  127,   12,   12, 0x08,
     778,  174,   12,   12, 0x08,
     811,  174,   12,   12, 0x08,
     848,  174,   12,   12, 0x08,
     881,  174,   12,   12, 0x08,
     914,  608,   12,   12, 0x08,
     959,   12,   12,   12, 0x08,
     985,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_SetupWindow[] = {
    "SetupWindow\0\0"
    "fullscreen,palmode,doublemode,bit32mode,filter\0"
    "ChangeGrafikModi(bool,bool,bool,bool,bool)\0"
    "ResetSreenshotCounter()\0index\0"
    "on_C64Farbmodus_currentIndexChanged(int)\0"
    "checked\0on_WPal_toggled(bool)\0"
    "on_WDouble_toggled(bool)\0"
    "on_W16Bit_toggled(bool)\0on_W32Bit_toggled(bool)\0"
    "on_WFilter_toggled(bool)\0"
    "on_ResetSShotCounter_clicked()\0"
    "row,column\0on_VJoySlots_cellChanged(int,int)\0"
    "on_VJoySlots_cellClicked(int,int)\0"
    "on_WAspectRatio_clicked(bool)\0"
    "on_FAspectRatio_clicked(bool)\0"
    "on_MausPort_currentIndexChanged(int)\0"
    "idx,idy\0onClickButton(int,int)\0value\0"
    "on_SIDVolume_valueChanged(int)\0"
    "on_AutoMouseHide_clicked(bool)\0arg1\0"
    "on_AutoMouseHideTime_valueChanged(int)\0"
    "on_FirstSidTyp_currentIndexChanged(int)\0"
    "on_SecondSidTyp_currentIndexChanged(int)\0"
    "on_SecondSidAddress_currentIndexChanged(int)\0"
    "on_SecondSidEnable_toggled(bool)\0"
    "on_SidCycleExactEnable_toggled(bool)\0"
    "on_SidFilterEnable_toggled(bool)\0"
    "on_Sid6ChannelMode_toggled(bool)\0"
    "on_SelectRomSet_currentIndexChanged(QString)\0"
    "on_DeleteRomSet_clicked()\0"
    "on_NewRomSet_clicked()\0"
};

void SetupWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SetupWindow *_t = static_cast<SetupWindow *>(_o);
        switch (_id) {
        case 0: _t->ChangeGrafikModi((*reinterpret_cast< bool(*)>(_a[1])),(*reinterpret_cast< bool(*)>(_a[2])),(*reinterpret_cast< bool(*)>(_a[3])),(*reinterpret_cast< bool(*)>(_a[4])),(*reinterpret_cast< bool(*)>(_a[5]))); break;
        case 1: _t->ResetSreenshotCounter(); break;
        case 2: _t->on_C64Farbmodus_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_WPal_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 4: _t->on_WDouble_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 5: _t->on_W16Bit_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_W32Bit_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 7: _t->on_WFilter_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_ResetSShotCounter_clicked(); break;
        case 9: _t->on_VJoySlots_cellChanged((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 10: _t->on_VJoySlots_cellClicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 11: _t->on_WAspectRatio_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 12: _t->on_FAspectRatio_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 13: _t->on_MausPort_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 14: _t->onClickButton((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 15: _t->on_SIDVolume_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 16: _t->on_AutoMouseHide_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 17: _t->on_AutoMouseHideTime_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 18: _t->on_FirstSidTyp_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 19: _t->on_SecondSidTyp_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 20: _t->on_SecondSidAddress_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 21: _t->on_SecondSidEnable_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 22: _t->on_SidCycleExactEnable_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 23: _t->on_SidFilterEnable_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 24: _t->on_Sid6ChannelMode_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 25: _t->on_SelectRomSet_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 26: _t->on_DeleteRomSet_clicked(); break;
        case 27: _t->on_NewRomSet_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SetupWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SetupWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_SetupWindow,
      qt_meta_data_SetupWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SetupWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SetupWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SetupWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SetupWindow))
        return static_cast<void*>(const_cast< SetupWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int SetupWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 28)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 28;
    }
    return _id;
}

// SIGNAL 0
void SetupWindow::ChangeGrafikModi(bool _t1, bool _t2, bool _t3, bool _t4, bool _t5)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)), const_cast<void*>(reinterpret_cast<const void*>(&_t3)), const_cast<void*>(reinterpret_cast<const void*>(&_t4)), const_cast<void*>(reinterpret_cast<const void*>(&_t5)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void SetupWindow::ResetSreenshotCounter()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
