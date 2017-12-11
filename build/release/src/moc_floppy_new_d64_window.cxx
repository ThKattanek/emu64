/****************************************************************************
** Meta object code from reading C++ file 'floppy_new_d64_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/floppy_new_d64_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'floppy_new_d64_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_FloppyNewD64Window[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      25,   20,   19,   19, 0x08,
      58,   20,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_FloppyNewD64Window[] = {
    "FloppyNewD64Window\0\0arg1\0"
    "on_Diskname_textChanged(QString)\0"
    "on_DIskID_textChanged(QString)\0"
};

void FloppyNewD64Window::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        FloppyNewD64Window *_t = static_cast<FloppyNewD64Window *>(_o);
        switch (_id) {
        case 0: _t->on_Diskname_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->on_DIskID_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData FloppyNewD64Window::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject FloppyNewD64Window::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_FloppyNewD64Window,
      qt_meta_data_FloppyNewD64Window, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &FloppyNewD64Window::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *FloppyNewD64Window::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *FloppyNewD64Window::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_FloppyNewD64Window))
        return static_cast<void*>(const_cast< FloppyNewD64Window*>(this));
    return QDialog::qt_metacast(_clname);
}

int FloppyNewD64Window::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
