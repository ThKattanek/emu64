/****************************************************************************
** Meta object code from reading C++ file 'widget_sr_edit.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/widget_sr_edit.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget_sr_edit.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WidgetSREdit[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       9,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      20,   14,   13,   13, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   13,   13,   13, 0x08,
      65,   13,   13,   13, 0x08,
      83,   13,   13,   13, 0x08,
     101,   13,   13,   13, 0x08,
     119,   13,   13,   13, 0x08,
     137,   13,   13,   13, 0x08,
     155,   13,   13,   13, 0x08,
     173,   13,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetSREdit[] = {
    "WidgetSREdit\0\0value\0ChangeValue(unsigned char)\0"
    "on_Bit0_clicked()\0on_Bit1_clicked()\0"
    "on_Bit2_clicked()\0on_Bit3_clicked()\0"
    "on_Bit4_clicked()\0on_Bit5_clicked()\0"
    "on_Bit6_clicked()\0on_Bit7_clicked()\0"
};

void WidgetSREdit::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WidgetSREdit *_t = static_cast<WidgetSREdit *>(_o);
        switch (_id) {
        case 0: _t->ChangeValue((*reinterpret_cast< unsigned char(*)>(_a[1]))); break;
        case 1: _t->on_Bit0_clicked(); break;
        case 2: _t->on_Bit1_clicked(); break;
        case 3: _t->on_Bit2_clicked(); break;
        case 4: _t->on_Bit3_clicked(); break;
        case 5: _t->on_Bit4_clicked(); break;
        case 6: _t->on_Bit5_clicked(); break;
        case 7: _t->on_Bit6_clicked(); break;
        case 8: _t->on_Bit7_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WidgetSREdit::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WidgetSREdit::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetSREdit,
      qt_meta_data_WidgetSREdit, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WidgetSREdit::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WidgetSREdit::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WidgetSREdit::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetSREdit))
        return static_cast<void*>(const_cast< WidgetSREdit*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetSREdit::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 9)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 9;
    }
    return _id;
}

// SIGNAL 0
void WidgetSREdit::ChangeValue(unsigned char _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}
QT_END_MOC_NAMESPACE
