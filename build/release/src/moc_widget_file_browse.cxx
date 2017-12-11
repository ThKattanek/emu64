/****************************************************************************
** Meta object code from reading C++ file 'widget_file_browse.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/widget_file_browse.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'widget_file_browse.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_WidgetFileBrowse[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       3,       // signalCount

 // signals: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x05,
      45,   36,   17,   17, 0x05,
      72,   65,   17,   17, 0x05,

 // slots: signature, parameters, type, tag, flags
     100,   17,   17,   17, 0x08,
     147,  141,   17,   17, 0x08,
     198,   17,   17,   17, 0x08,
     221,  141,   17,   17, 0x08,
     274,  266,   17,   17, 0x08,
     306,   17,   17,   17, 0x08,
     332,   17,   17,   17, 0x08,
     357,   17,   17,   17, 0x08,
     383,  378,   17,   17, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_WidgetFileBrowse[] = {
    "WidgetFileBrowse\0\0FileDoubleClick()\0"
    "filename\0SelectFile(QString)\0status\0"
    "WriteProtectedChanged(bool)\0"
    "on_listWidget_zip_itemSelectionChanged()\0"
    "index\0on_listView_filebrowser_doubleClicked(QModelIndex)\0"
    "on_to_parent_clicked()\0"
    "on_listView_filebrowser_clicked(QModelIndex)\0"
    "checked\0on_WriteProtected_clicked(bool)\0"
    "on_view_refresh_clicked()\0"
    "on_delete_file_clicked()\0on_to_home_clicked()\0"
    "arg1\0on_drive_list_currentIndexChanged(QString)\0"
};

void WidgetFileBrowse::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        WidgetFileBrowse *_t = static_cast<WidgetFileBrowse *>(_o);
        switch (_id) {
        case 0: _t->FileDoubleClick(); break;
        case 1: _t->SelectFile((*reinterpret_cast< QString(*)>(_a[1]))); break;
        case 2: _t->WriteProtectedChanged((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 3: _t->on_listWidget_zip_itemSelectionChanged(); break;
        case 4: _t->on_listView_filebrowser_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 5: _t->on_to_parent_clicked(); break;
        case 6: _t->on_listView_filebrowser_clicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 7: _t->on_WriteProtected_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 8: _t->on_view_refresh_clicked(); break;
        case 9: _t->on_delete_file_clicked(); break;
        case 10: _t->on_to_home_clicked(); break;
        case 11: _t->on_drive_list_currentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData WidgetFileBrowse::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject WidgetFileBrowse::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_WidgetFileBrowse,
      qt_meta_data_WidgetFileBrowse, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &WidgetFileBrowse::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *WidgetFileBrowse::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *WidgetFileBrowse::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_WidgetFileBrowse))
        return static_cast<void*>(const_cast< WidgetFileBrowse*>(this));
    return QWidget::qt_metacast(_clname);
}

int WidgetFileBrowse::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    return _id;
}

// SIGNAL 0
void WidgetFileBrowse::FileDoubleClick()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void WidgetFileBrowse::SelectFile(QString _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void WidgetFileBrowse::WriteProtectedChanged(bool _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}
QT_END_MOC_NAMESPACE
