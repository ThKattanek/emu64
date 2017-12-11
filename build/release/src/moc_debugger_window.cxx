/****************************************************************************
** Meta object code from reading C++ file 'debugger_window.h'
**
** Created by: The Qt Meta Object Compiler version 63 (Qt 4.8.7)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/debugger_window.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'debugger_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.7. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DebuggerWindow[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      33,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      20,   16,   15,   15, 0x08,
      52,   46,   15,   15, 0x08,
      97,   91,   15,   15, 0x08,
     134,   15,   15,   15, 0x08,
     157,   15,   15,   15, 0x08,
     188,   15,   15,   15, 0x08,
     211,   15,   15,   15, 0x08,
     248,  242,   15,   15, 0x08,
     289,   15,   15,   15, 0x08,
     326,  321,   15,   15, 0x08,
     364,   15,   15,   15, 0x08,
     401,   15,   15,   15, 0x08,
     429,   15,   15,   15, 0x08,
     456,   15,   15,   15, 0x08,
     482,   46,   15,   15, 0x08,
     518,  242,   15,   15, 0x08,
     560,   46,   15,   15, 0x08,
     594,   15,   15,   15, 0x08,
     633,  621,   15,   15, 0x08,
     685,   15,   15,   15, 0x08,
     712,   15,   15,   15, 0x08,
     741,   15,   15,   15, 0x08,
     770,   15,   15,   15, 0x08,
     801,   46,   15,   15, 0x08,
     836,   15,   15,   15, 0x08,
     862,   15,   15,   15, 0x08,
     885,   15,   15,   15, 0x08,
     906,   15,   15,   15, 0x08,
     923,   15,   15,   15, 0x08,
     948,  940,   15,   15, 0x08,
     975,  940,   15,   15, 0x08,
    1000,  940,   15,   15, 0x08,
    1025,  940,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DebuggerWindow[] = {
    "DebuggerWindow\0\0pos\0onShowContextMenu(QPoint)\0"
    "value\0onSr_widget_ValueChange(unsigned char)\0"
    "label\0onReg_label_clicked(LabelWidgetMod*)\0"
    "on_OneOpcode_clicked()\0"
    "on_EingabeFeld_returnPressed()\0"
    "on_OneZyklus_clicked()\0"
    "on_CycleCounterReset_clicked()\0index\0"
    "on_ChangeSource_currentIndexChanged(int)\0"
    "on_AssAdresseIn_returnPressed()\0arg1\0"
    "on_AssMnemonicIn_textChanged(QString)\0"
    "on_AssAdressierungIn_returnPressed()\0"
    "on_AnimationStart_clicked()\0"
    "on_AnimationStop_clicked()\0"
    "onTimerAnimationRefresh()\0"
    "on_AnimationSpeed_valueChanged(int)\0"
    "on_DisAssTable_doubleClicked(QModelIndex)\0"
    "on_DisAssScroll_valueChanged(int)\0"
    "on_AddBreakpoint_clicked()\0item,column\0"
    "on_BreakpointTree_itemChanged(QTreeWidgetItem*,int)\0"
    "on_DelBreakpoint_clicked()\0"
    "on_LoadBreakpoints_clicked()\0"
    "on_SaveBreakpoints_clicked()\0"
    "on_DelAllBreakpoints_clicked()\0"
    "on_HistoryScroll_valueChanged(int)\0"
    "on_ExportDisAss_clicked()\0"
    "onChangeFloppyStatus()\0on_MemEdit_clicked()\0"
    "on_VIC_clicked()\0on_IEC_clicked()\0"
    "checked\0on_man_lines_clicked(bool)\0"
    "on_rdy_led_clicked(bool)\0"
    "on_irq_led_clicked(bool)\0"
    "on_nmi_led_clicked(bool)\0"
};

void DebuggerWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        DebuggerWindow *_t = static_cast<DebuggerWindow *>(_o);
        switch (_id) {
        case 0: _t->onShowContextMenu((*reinterpret_cast< const QPoint(*)>(_a[1]))); break;
        case 1: _t->onSr_widget_ValueChange((*reinterpret_cast< unsigned char(*)>(_a[1]))); break;
        case 2: _t->onReg_label_clicked((*reinterpret_cast< LabelWidgetMod*(*)>(_a[1]))); break;
        case 3: _t->on_OneOpcode_clicked(); break;
        case 4: _t->on_EingabeFeld_returnPressed(); break;
        case 5: _t->on_OneZyklus_clicked(); break;
        case 6: _t->on_CycleCounterReset_clicked(); break;
        case 7: _t->on_ChangeSource_currentIndexChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 8: _t->on_AssAdresseIn_returnPressed(); break;
        case 9: _t->on_AssMnemonicIn_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 10: _t->on_AssAdressierungIn_returnPressed(); break;
        case 11: _t->on_AnimationStart_clicked(); break;
        case 12: _t->on_AnimationStop_clicked(); break;
        case 13: _t->onTimerAnimationRefresh(); break;
        case 14: _t->on_AnimationSpeed_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 15: _t->on_DisAssTable_doubleClicked((*reinterpret_cast< const QModelIndex(*)>(_a[1]))); break;
        case 16: _t->on_DisAssScroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 17: _t->on_AddBreakpoint_clicked(); break;
        case 18: _t->on_BreakpointTree_itemChanged((*reinterpret_cast< QTreeWidgetItem*(*)>(_a[1])),(*reinterpret_cast< int(*)>(_a[2]))); break;
        case 19: _t->on_DelBreakpoint_clicked(); break;
        case 20: _t->on_LoadBreakpoints_clicked(); break;
        case 21: _t->on_SaveBreakpoints_clicked(); break;
        case 22: _t->on_DelAllBreakpoints_clicked(); break;
        case 23: _t->on_HistoryScroll_valueChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 24: _t->on_ExportDisAss_clicked(); break;
        case 25: _t->onChangeFloppyStatus(); break;
        case 26: _t->on_MemEdit_clicked(); break;
        case 27: _t->on_VIC_clicked(); break;
        case 28: _t->on_IEC_clicked(); break;
        case 29: _t->on_man_lines_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 30: _t->on_rdy_led_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 31: _t->on_irq_led_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        case 32: _t->on_nmi_led_clicked((*reinterpret_cast< bool(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData DebuggerWindow::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject DebuggerWindow::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_DebuggerWindow,
      qt_meta_data_DebuggerWindow, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DebuggerWindow::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DebuggerWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DebuggerWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DebuggerWindow))
        return static_cast<void*>(const_cast< DebuggerWindow*>(this));
    return QDialog::qt_metacast(_clname);
}

int DebuggerWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 33)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 33;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
