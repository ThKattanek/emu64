/****************************************************************************
** Meta object code from reading C++ file 'widget_memory_zeile.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/widget_memory_zeile.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget_memory_zeile.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WidgetMemoryZeile[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      18,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      28,   19,   18,   18, 0x05,

 // slots: signature, parameters, type, tag, flags
      70,   18,   18,   18, 0x08,
     103,   18,   18,   18, 0x08,
     136,   18,   18,   18, 0x08,
     169,   18,   18,   18, 0x08,
     202,   18,   18,   18, 0x08,
     235,   18,   18,   18, 0x08,
     268,   18,   18,   18, 0x08,
     301,   18,   18,   18, 0x08,
     334,   18,   18,   18, 0x08,
     367,   18,   18,   18, 0x08,
     400,   18,   18,   18, 0x08,
     434,   18,   18,   18, 0x08,
     468,   18,   18,   18, 0x08,
     502,   18,   18,   18, 0x08,
     536,   18,   18,   18, 0x08,
     570,   18,   18,   18, 0x08,
     604,   18,   18,   18, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetMemoryZeile[] = {
    "WidgetMemoryZeile\0\0adr,wert\0"
    "ChangeValue(unsigned short,unsigned char)\0"
    "on_EditValue_0_editingFinished()\0"
    "on_EditValue_1_editingFinished()\0"
    "on_EditValue_2_editingFinished()\0"
    "on_EditValue_3_editingFinished()\0"
    "on_EditValue_4_editingFinished()\0"
    "on_EditValue_5_editingFinished()\0"
    "on_EditValue_6_editingFinished()\0"
    "on_EditValue_7_editingFinished()\0"
    "on_EditValue_8_editingFinished()\0"
    "on_EditValue_9_editingFinished()\0"
    "on_EditValue_10_editingFinished()\0"
    "on_EditValue_11_editingFinished()\0"
    "on_EditValue_12_editingFinished()\0"
    "on_EditValue_13_editingFinished()\0"
    "on_EditValue_14_editingFinished()\0"
    "on_EditValue_15_editingFinished()\0"
    "onNoFocus()\0"
};

void WidgetMemoryZeile::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WidgetMemoryZeile *_t = static_cast<WidgetMemoryZeile *>(_o);
        switch (_id) {
        case 0: _t->ChangeValue((*reinterpret_cast< unsigned short(*)>(_a[1])),(*reinterpret_cast< unsigned char(*)>(_a[2]))); break;
        case 1: _t->on_EditValue_0_editingFinished(); break;
        case 2: _t->on_EditValue_1_editingFinished(); break;
        case 3: _t->on_EditValue_2_editingFinished(); break;
        case 4: _t->on_EditValue_3_editingFinished(); break;
        case 5: _t->on_EditValue_4_editingFinished(); break;
        case 6: _t->on_EditValue_5_editingFinished(); break;
        case 7: _t->on_EditValue_6_editingFinished(); break;
        case 8: _t->on_EditValue_7_editingFinished(); break;
        case 9: _t->on_EditValue_8_editingFinished(); break;
        case 10: _t->on_EditValue_9_editingFinished(); break;
        case 11: _t->on_EditValue_10_editingFinished(); break;
        case 12: _t->on_EditValue_11_editingFinished(); break;
        case 13: _t->on_EditValue_12_editingFinished(); break;
        case 14: _t->on_EditValue_13_editingFinished(); break;
        case 15: _t->on_EditValue_14_editingFinished(); break;
        case 16: _t->on_EditValue_15_editingFinished(); break;
        case 17: _t->onNoFocus(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WidgetMemoryZeile::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WidgetMemoryZeile::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetMemoryZeile,
      qt_meta_data_WidgetMemoryZeile, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WidgetMemoryZeile::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WidgetMemoryZeile::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WidgetMemoryZeile::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetMemoryZeile))
        return static_cast<void*>(const_cast< WidgetMemoryZeile*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetMemoryZeile::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 18)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 18;
    }
    return _id;
}

// SIGNAL 0
void WidgetMemoryZeile::ChangeValue(unsigned short _t1, unsigned char _t2)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)), const_cast<void*>(reinterpret_cast<const void*>(&_t2)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
