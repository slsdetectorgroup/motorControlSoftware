/****************************************************************************
** Meta object code from reading C++ file 'MotorControlGui.h'
**
** Created: Mon Jul 11 16:04:55 2016
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../MotorControlGui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'MotorControlGui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_MotorControlGui[] = {

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
      40,   34,   16,   16, 0x08,
      65,   16,   61,   16, 0x08,
      75,   16,   16,   16, 0x08,
      87,   16,   16,   16, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_MotorControlGui[] = {
    "MotorControlGui\0\0CheckReference()\0"
    "index\0MoveToReference(int)\0int\0Execute()\0"
    "UpdateGUI()\0OptionsMenu()\0"
};

void MotorControlGui::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        MotorControlGui *_t = static_cast<MotorControlGui *>(_o);
        switch (_id) {
        case 0: _t->CheckReference(); break;
        case 1: _t->MoveToReference((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: { int _r = _t->Execute();
            if (_a[0]) *reinterpret_cast< int*>(_a[0]) = _r; }  break;
        case 3: _t->UpdateGUI(); break;
        case 4: _t->OptionsMenu(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData MotorControlGui::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject MotorControlGui::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_MotorControlGui,
      qt_meta_data_MotorControlGui, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &MotorControlGui::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *MotorControlGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *MotorControlGui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_MotorControlGui))
        return static_cast<void*>(const_cast< MotorControlGui*>(this));
    return QWidget::qt_metacast(_clname);
}

int MotorControlGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
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
