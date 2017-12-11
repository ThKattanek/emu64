/****************************************************************************
** Meta object code from reading C++ file 'tape_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/tape_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'tape_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_TapeWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       8,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      21,   12,   11,   11, 0x08,
      43,   11,   11,   11, 0x08,
      58,   11,   11,   11, 0x08,
      75,   11,   11,   11, 0x08,
      93,   11,   11,   11, 0x08,
     110,   11,   11,   11, 0x08,
     127,   11,   11,   11, 0x08,
     145,   11,   11,   11, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_TapeWindow[] = {
    "TapeWindow\0\0filename\0OnSelectFile(QString)\0"
    "OnRefreshGUI()\0on_Rec_clicked()\0"
    "on_Play_clicked()\0on_Rew_clicked()\0"
    "on_FFw_clicked()\0on_Stop_clicked()\0"
    "on_Volume_clicked()\0"
};

void TapeWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        TapeWindow *_t = static_cast<TapeWindow *>(_o);
        switch (_id) {
        case 0: _t->OnSelectFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 1: _t->OnRefreshGUI(); break;
        case 2: _t->on_Rec_clicked(); break;
        case 3: _t->on_Play_clicked(); break;
        case 4: _t->on_Rew_clicked(); break;
        case 5: _t->on_FFw_clicked(); break;
        case 6: _t->on_Stop_clicked(); break;
        case 7: _t->on_Volume_clicked(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData TapeWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject TapeWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_TapeWindow,
      qt_meta_data_TapeWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &TapeWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *TapeWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *TapeWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_TapeWindow))
        return static_cast<void*>(const_cast< TapeWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int TapeWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 8)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 8;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
