/****************************************************************************
** Meta object code from reading C++ file 'new_romset_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/new_romset_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'new_romset_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_NewRomSetWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      17,   16,   16,   16, 0x08,
      45,   16,   16,   16, 0x08,
      72,   16,   16,   16, 0x08,
      98,   16,   16,   16, 0x08,
     132,  127,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_NewRomSetWindow[] = {
    "NewRomSetWindow\0\0on_sel_kernal_rom_clicked()\0"
    "on_sel_basic_rom_clicked()\0"
    "on_sel_char_rom_clicked()\0"
    "on_sel_dos1541_rom_clicked()\0arg1\0"
    "on_RomSetName_textChanged(QString)\0"
};

void NewRomSetWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        NewRomSetWindow *_t = static_cast<NewRomSetWindow *>(_o);
        switch (_id) {
        case 0: _t->on_sel_kernal_rom_clicked(); break;
        case 1: _t->on_sel_basic_rom_clicked(); break;
        case 2: _t->on_sel_char_rom_clicked(); break;
        case 3: _t->on_sel_dos1541_rom_clicked(); break;
        case 4: _t->on_RomSetName_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData NewRomSetWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject NewRomSetWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_NewRomSetWindow,
      qt_meta_data_NewRomSetWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &NewRomSetWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *NewRomSetWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *NewRomSetWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_NewRomSetWindow))
        return static_cast<void*>(const_cast< NewRomSetWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int NewRomSetWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
