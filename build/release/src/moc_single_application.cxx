/****************************************************************************
** Meta object code from reading C++ file 'single_application.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/single_application.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'single_application.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_SingleApplication[] = {

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
      28,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      58,   18,   18,   18, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_SingleApplication[] = {
    "SingleApplication\0\0messages\0"
    "messageAvailable(QStringList)\0"
    "checkForMessage()\0"
};

void SingleApplication::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        SingleApplication *_t = static_cast<SingleApplication *>(_o);
        switch (_id) {
        case 0: _t->messageAvailable((*reinterpret_cast< const QStringList(*)>(_a[1]))); break;
        case 1: _t->checkForMessage(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData SingleApplication::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject SingleApplication::staticMetaObject = {
    { &QApplication::staticMetaObject, qt_meta_stringdata_SingleApplication,
      qt_meta_data_SingleApplication, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &SingleApplication::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *SingleApplication::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *SingleApplication::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_SingleApplication))
        return static_cast<void*>(const_cast< SingleApplication*>(this));
    return QApplication::qt_metacast(_clname);
}

int SingleApplication::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QApplication::qt_metacall(_c, _id, _a);
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
void SingleApplication::messageAvailable(const QStringList & _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
