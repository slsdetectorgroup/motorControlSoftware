/****************************************************************************
** Meta object code from reading C++ file 'MotorWidget.h'
**
** Created: Mon Jul 11 16:04:55 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MotorWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MotorWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MotorWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x05,
      29,   12,   12,   12, 0x05,

 // slots: signature, parameters, type, tag, flags
      47,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MotorWidget[] = {
    "MotorWidget\0\0RelativeMoved()\0"
    "UpdateGUISignal()\0MoveRelative()\0"
};

void MotorWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MotorWidget *_t = static_cast<MotorWidget *>(_o);
        switch (_id) {
        case 0: _t->RelativeMoved(); break;
        case 1: _t->UpdateGUISignal(); break;
        case 2: _t->MoveRelative(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData MotorWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MotorWidget::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MotorWidget,
      qt_meta_data_MotorWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MotorWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MotorWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MotorWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MotorWidget))
        return static_cast<void*>(const_cast< MotorWidget*>(this));
    return QWidget::qt_metacast(_clname);
}

int MotorWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}

// SIGNAL 0
void MotorWidget::RelativeMoved()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void MotorWidget::UpdateGUISignal()
{
    QMetaObject::activate(this, &staticMetaObject, 1, 0);
}
QT_END_MOC_NAMESPACE
