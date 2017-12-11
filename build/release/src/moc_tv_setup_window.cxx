/****************************************************************************
** Meta object code from reading C++ file 'tv_setup_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/tv_setup_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tv_setup_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TVSetupWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   15,   14,   14, 0x08,
      60,   15,   14,   14, 0x08,
      99,   15,   14,   14, 0x08,
     136,   15,   14,   14, 0x08,
     173,   15,   14,   14, 0x08,
     211,   15,   14,   14, 0x08,
     245,   15,   14,   14, 0x08,
     282,   14,   14,   14, 0x08,
     301,   15,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TVSetupWindow[] = {
    "TVSetupWindow\0\0value\0"
    "on_helligkeit_scroll_valueChanged(int)\0"
    "on_saettigung_scroll_valueChanged(int)\0"
    "on_kontrast_scroll_valueChanged(int)\0"
    "on_horblurY_scroll_valueChanged(int)\0"
    "on_horblurUV_scroll_valueChanged(int)\0"
    "on_phase_scroll_valueChanged(int)\0"
    "on_scanline_scroll_valueChanged(int)\0"
    "on_Reset_clicked()\0"
    "on_distortion_scroll_valueChanged(int)\0"
};

void TVSetupWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TVSetupWindow *_t = static_cast<TVSetupWindow *>(_o);
        switch (_id) {
        case 0: _t->on_helligkeit_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->on_saettigung_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->on_kontrast_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_horblurY_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: _t->on_horblurUV_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->on_phase_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 6: _t->on_scanline_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->on_Reset_clicked(); break;
        case 8: _t->on_distortion_scroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TVSetupWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TVSetupWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TVSetupWindow,
      qt_meta_data_TVSetupWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TVSetupWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TVSetupWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TVSetupWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TVSetupWindow))
        return static_cast<void*>(const_cast< TVSetupWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int TVSetupWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
