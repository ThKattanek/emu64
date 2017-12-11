/****************************************************************************
** Meta object code from reading C++ file 'button_mod.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/button_mod.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'button_mod.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ButtonMod[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      19,   11,   10,   10, 0x05,

 // slots: signature, parameters, type, tag, flags
      36,   10,   10,   10, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_ButtonMod[] = {
    "ButtonMod\0\0idx,idy\0Clicked(int,int)\0"
    "onClicked()\0"
};

void ButtonMod::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        ButtonMod *_t = static_cast<ButtonMod *>(_o);
        switch (_id) {
        case 0: _t->Clicked((*reinterpret_cast< int(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 1: _t->onClicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData ButtonMod::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject ButtonMod::staticMetaObject = {
    { &QPushButton::staticMetaObject, qt_meta_stringdata_ButtonMod,
      qt_meta_data_ButtonMod, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ButtonMod::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ButtonMod::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ButtonMod::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ButtonMod))
        return static_cast<void*>(const_cast< ButtonMod*>(this));
    return QPushButton::qt_metacast(_clname);
}

int ButtonMod::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QPushButton::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}

// SIGNAL 0
void ButtonMod::Clicked(int _t1, int _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
