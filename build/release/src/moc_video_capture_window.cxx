/****************************************************************************
** Meta object code from reading C++ file 'video_capture_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/video_capture_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'video_capture_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_VideoCaptureWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       4,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   19,   19,   19, 0x08,
      46,   19,   19,   19, 0x08,
      79,   71,   19,   19, 0x08,
     109,   19,   19,   19, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_VideoCaptureWindow[] = {
    "VideoCaptureWindow\0\0on_CaptureStart_clicked()\0"
    "on_CaptureStop_clicked()\0checked\0"
    "on_CapturePause_clicked(bool)\0OnTimer1()\0"
};

void VideoCaptureWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        VideoCaptureWindow *_t = static_cast<VideoCaptureWindow *>(_o);
        switch (_id) {
        case 0: _t->on_CaptureStart_clicked(); break;
        case 1: _t->on_CaptureStop_clicked(); break;
        case 2: _t->on_CapturePause_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->OnTimer1(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData VideoCaptureWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject VideoCaptureWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_VideoCaptureWindow,
      qt_meta_data_VideoCaptureWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &VideoCaptureWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *VideoCaptureWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *VideoCaptureWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_VideoCaptureWindow))
        return static_cast<void*>(const_cast< VideoCaptureWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int VideoCaptureWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 4)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 4;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
