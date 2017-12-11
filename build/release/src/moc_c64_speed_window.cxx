/****************************************************************************
** Meta object code from reading C++ file 'c64_speed_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/c64_speed_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'c64_speed_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_C64SpeedWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      14,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      22,   16,   15,   15, 0x08,
      50,   45,   15,   15, 0x08,
      86,   80,   15,   15, 0x08,
     122,   15,   15,   15, 0x08,
     140,   15,   15,   15, 0x08,
     159,   15,   15,   15, 0x08,
     178,   15,   15,   15, 0x08,
     198,   15,   15,   15, 0x08,
     218,   15,   15,   15, 0x08,
     238,   15,   15,   15, 0x08,
     258,   15,   15,   15, 0x08,
     278,   15,   15,   15, 0x08,
     297,   15,   15,   15, 0x08,
     323,  315,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_C64SpeedWindow[] = {
    "C64SpeedWindow\0\0event\0showEvent(QShowEvent*)\0"
    "arg1\0on_C64Speed_valueChanged(int)\0"
    "value\0on_ScrollC64Speed_valueChanged(int)\0"
    "on_Set5_clicked()\0on_Set25_clicked()\0"
    "on_Set50_clicked()\0on_Set100_clicked()\0"
    "on_Set200_clicked()\0on_Set300_clicked()\0"
    "on_Set400_clicked()\0on_Set500_clicked()\0"
    "on_Pause_clicked()\0SetPauseTooltip()\0"
    "checked\0on_WarpMode_clicked(bool)\0"
};

void C64SpeedWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        C64SpeedWindow *_t = static_cast<C64SpeedWindow *>(_o);
        switch (_id) {
        case 0: _t->showEvent((*reinterpret_cast< QShowEvent*(*)>(_a[1]))); break;
        case 1: _t->on_C64Speed_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_ScrollC64Speed_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_Set5_clicked(); break;
        case 4: _t->on_Set25_clicked(); break;
        case 5: _t->on_Set50_clicked(); break;
        case 6: _t->on_Set100_clicked(); break;
        case 7: _t->on_Set200_clicked(); break;
        case 8: _t->on_Set300_clicked(); break;
        case 9: _t->on_Set400_clicked(); break;
        case 10: _t->on_Set500_clicked(); break;
        case 11: _t->on_Pause_clicked(); break;
        case 12: _t->SetPauseTooltip(); break;
        case 13: _t->on_WarpMode_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData C64SpeedWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject C64SpeedWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_C64SpeedWindow,
      qt_meta_data_C64SpeedWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &C64SpeedWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *C64SpeedWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *C64SpeedWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_C64SpeedWindow))
        return static_cast<void*>(const_cast< C64SpeedWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int C64SpeedWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 14)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 14;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
