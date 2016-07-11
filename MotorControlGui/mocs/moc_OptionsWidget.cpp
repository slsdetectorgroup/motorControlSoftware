/****************************************************************************
** Meta object code from reading C++ file 'OptionsWidget.h'
**
** Created: Mon Jul 11 16:04:56 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../OptionsWidget.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'OptionsWidget.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_OptionsWidget[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       1,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
      37,   31,   14,   14, 0x08,
      60,   14,   14,   14, 0x08,
      75,   14,   14,   14, 0x08,
      88,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_OptionsWidget[] = {
    "OptionsWidget\0\0OptionsClosed()\0index\0"
    "ChangeMotorWidget(int)\0MoveAbsolute()\0"
    "MoveToHome()\0SetOption()\0"
};

void OptionsWidget::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        OptionsWidget *_t = static_cast<OptionsWidget *>(_o);
        switch (_id) {
        case 0: _t->OptionsClosed(); break;
        case 1: _t->ChangeMotorWidget((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->MoveAbsolute(); break;
        case 3: _t->MoveToHome(); break;
        case 4: _t->SetOption(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData OptionsWidget::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject OptionsWidget::staticMetaObject = {
    { &QFrame::staticMetaObject, qt_meta_stringdata_OptionsWidget,
      qt_meta_data_OptionsWidget, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &OptionsWidget::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *OptionsWidget::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *OptionsWidget::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_OptionsWidget))
        return static_cast<void*>(const_cast< OptionsWidget*>(this));
    return QFrame::qt_metacast(_clname);
}

int OptionsWidget::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QFrame::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 5)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void OptionsWidget::OptionsClosed()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}
QT_END_MOC_NAMESPACE
