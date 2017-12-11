/****************************************************************************
** Meta object code from reading C++ file 'c64_keyboard_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/c64_keyboard_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'c64_keyboard_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_C64KeyboardWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      19,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_C64KeyboardWindow[] = {
    "C64KeyboardWindow\0\0timer_event()\0"
};

void C64KeyboardWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        C64KeyboardWindow *_t = static_cast<C64KeyboardWindow *>(_o);
        switch (_id) {
        case 0: _t->timer_event(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData C64KeyboardWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject C64KeyboardWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_C64KeyboardWindow,
      qt_meta_data_C64KeyboardWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &C64KeyboardWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *C64KeyboardWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *C64KeyboardWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_C64KeyboardWindow))
        return static_cast<void*>(const_cast< C64KeyboardWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int C64KeyboardWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
