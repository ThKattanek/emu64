/****************************************************************************
** Meta object code from reading C++ file 'memory_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/memory_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'memory_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MemoryWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   24,   13,   13, 0x08,
      87,   81,   13,   13, 0x08,
     121,   13,   13,   13, 0x08,
     153,   13,   13,   13, 0x08,
     194,  186,   13,   13, 0x08,
     222,  186,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MemoryWindow[] = {
    "MemoryWindow\0\0NoFocus()\0adresse,wert\0"
    "onChangeValue(unsigned short,unsigned char)\0"
    "value\0on_MemoryScroll_valueChanged(int)\0"
    "on_MemoryScroll_sliderPressed()\0"
    "on_MemoryScroll_sliderReleased()\0"
    "checked\0on_BitAnzeige_clicked(bool)\0"
    "on_OnlyRam_clicked(bool)\0"
};

void MemoryWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MemoryWindow *_t = static_cast<MemoryWindow *>(_o);
        switch (_id) {
        case 0: _t->NoFocus(); break;
        case 1: _t->onChangeValue((*reinterpret_cast< unsigned short(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2]))); break;
        case 2: _t->on_MemoryScroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 3: _t->on_MemoryScroll_sliderPressed(); break;
        case 4: _t->on_MemoryScroll_sliderReleased(); break;
        case 5: _t->on_BitAnzeige_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_OnlyRam_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MemoryWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MemoryWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_MemoryWindow,
      qt_meta_data_MemoryWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MemoryWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MemoryWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MemoryWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MemoryWindow))
        return static_cast<void*>(const_cast< MemoryWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int MemoryWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 7)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void MemoryWindow::NoFocus()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
