/****************************************************************************
** Meta object code from reading C++ file 'crt_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/crt_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'crt_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CrtWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      10,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      11,   10,   10,   10, 0x08,
      37,   10,   10,   10, 0x08,
      66,   10,   10,   10, 0x08,
      89,   10,   10,   10, 0x08,
     112,   10,   10,   10, 0x08,
     148,  140,   10,   10, 0x08,
     176,   10,   10,   10, 0x08,
     212,  203,   10,   10, 0x08,
     251,  234,   10,   10, 0x08,
     302,   10,   10,   10, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_CrtWindow[] = {
    "CrtWindow\0\0on_ViewChipData_clicked()\0"
    "on_NewEasyFlashCRT_clicked()\0"
    "on_InsertCRT_clicked()\0on_RemoveCRT_clicked()\0"
    "on_FreezButtonFC3_clicked()\0checked\0"
    "on_EF_JUMPER0_toggled(bool)\0"
    "on_FreezButtonAR_clicked()\0filename\0"
    "onSelectFile(QString)\0current,previous\0"
    "onChipList_currentChanged(QModelIndex,QModelIndex)\0"
    "onTimer1()\0"
};

void CrtWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CrtWindow *_t = static_cast<CrtWindow *>(_o);
        switch (_id) {
        case 0: _t->on_ViewChipData_clicked(); break;
        case 1: _t->on_NewEasyFlashCRT_clicked(); break;
        case 2: _t->on_InsertCRT_clicked(); break;
        case 3: _t->on_RemoveCRT_clicked(); break;
        case 4: _t->on_FreezButtonFC3_clicked(); break;
        case 5: _t->on_EF_JUMPER0_toggled((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 6: _t->on_FreezButtonAR_clicked(); break;
        case 7: _t->onSelectFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 8: _t->onChipList_currentChanged((*reinterpret_cast< const QModelIndex(*)>(_a[1])),(*reinterpret_cast< const QModelIndex(*)>(_a[2]))); break;
        case 9: _t->onTimer1(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData CrtWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CrtWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CrtWindow,
      qt_meta_data_CrtWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CrtWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CrtWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CrtWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CrtWindow))
        return static_cast<void*>(const_cast< CrtWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int CrtWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 10)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 10;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
