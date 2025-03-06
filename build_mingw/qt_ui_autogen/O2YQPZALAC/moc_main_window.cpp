/****************************************************************************
** Meta object code from reading C++ file 'main_window.h'
**
** Created by: The Qt Meta Object Compiler version 68 (Qt 6.8.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../../src/cpp/ui/main_window.h"
#include <QtGui/qtextcursor.h>
#include <QtCore/qmetatype.h>

#include <QtCore/qtmochelpers.h>

#include <memory>


#include <QtCore/qxptype_traits.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'main_window.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 68
#error "This file was generated using the moc from 6.8.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

#ifndef Q_CONSTINIT
#define Q_CONSTINIT
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
QT_WARNING_DISABLE_GCC("-Wuseless-cast")
namespace {
struct qt_meta_tag_ZN6crusty10MainWindowE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN6crusty10MainWindowE = QtMocHelpers::stringData(
    "crusty::MainWindow",
    "encryptFile",
    "",
    "decryptFile",
    "processBatch",
    "openFile",
    "refreshFileList",
    "onFileSelected",
    "QModelIndex",
    "index",
    "showSettings",
    "showKeyManagement",
    "showDeviceManagement",
    "toggle2FA",
    "enabled",
    "configure2FA",
    "showAbout"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN6crusty10MainWindowE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
      12,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    0,   86,    2, 0x08,    1 /* Private */,
       3,    0,   87,    2, 0x08,    2 /* Private */,
       4,    0,   88,    2, 0x08,    3 /* Private */,
       5,    0,   89,    2, 0x08,    4 /* Private */,
       6,    0,   90,    2, 0x08,    5 /* Private */,
       7,    1,   91,    2, 0x08,    6 /* Private */,
      10,    0,   94,    2, 0x08,    8 /* Private */,
      11,    0,   95,    2, 0x08,    9 /* Private */,
      12,    0,   96,    2, 0x08,   10 /* Private */,
      13,    1,   97,    2, 0x08,   11 /* Private */,
      15,    0,  100,    2, 0x08,   13 /* Private */,
      16,    0,  101,    2, 0x08,   14 /* Private */,

 // slots: parameters
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, 0x80000000 | 8,    9,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void,
    QMetaType::Void, QMetaType::Bool,   14,
    QMetaType::Void,
    QMetaType::Void,

       0        // eod
};

Q_CONSTINIT const QMetaObject crusty::MainWindow::staticMetaObject = { {
    QMetaObject::SuperData::link<QMainWindow::staticMetaObject>(),
    qt_meta_stringdata_ZN6crusty10MainWindowE.offsetsAndSizes,
    qt_meta_data_ZN6crusty10MainWindowE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN6crusty10MainWindowE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<MainWindow, std::true_type>,
        // method 'encryptFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'decryptFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'processBatch'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'openFile'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'refreshFileList'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'onFileSelected'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QModelIndex &, std::false_type>,
        // method 'showSettings'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showKeyManagement'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showDeviceManagement'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'toggle2FA'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<bool, std::false_type>,
        // method 'configure2FA'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        // method 'showAbout'
        QtPrivate::TypeAndForceComplete<void, std::false_type>
    >,
    nullptr
} };

void crusty::MainWindow::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<MainWindow *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->encryptFile(); break;
        case 1: _t->decryptFile(); break;
        case 2: _t->processBatch(); break;
        case 3: _t->openFile(); break;
        case 4: _t->refreshFileList(); break;
        case 5: _t->onFileSelected((*reinterpret_cast< std::add_pointer_t<QModelIndex>>(_a[1]))); break;
        case 6: _t->showSettings(); break;
        case 7: _t->showKeyManagement(); break;
        case 8: _t->showDeviceManagement(); break;
        case 9: _t->toggle2FA((*reinterpret_cast< std::add_pointer_t<bool>>(_a[1]))); break;
        case 10: _t->configure2FA(); break;
        case 11: _t->showAbout(); break;
        default: ;
        }
    }
}

const QMetaObject *crusty::MainWindow::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *crusty::MainWindow::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN6crusty10MainWindowE.stringdata0))
        return static_cast<void*>(this);
    return QMainWindow::qt_metacast(_clname);
}

int crusty::MainWindow::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 12)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 12;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 12)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 12;
    }
    return _id;
}
namespace {
struct qt_meta_tag_ZN6crusty21PasswordStrengthMeterE_t {};
} // unnamed namespace


#ifdef QT_MOC_HAS_STRINGDATA
static constexpr auto qt_meta_stringdata_ZN6crusty21PasswordStrengthMeterE = QtMocHelpers::stringData(
    "crusty::PasswordStrengthMeter",
    "updateStrength",
    "",
    "password"
);
#else  // !QT_MOC_HAS_STRINGDATA
#error "qtmochelpers.h not found or too old."
#endif // !QT_MOC_HAS_STRINGDATA

Q_CONSTINIT static const uint qt_meta_data_ZN6crusty21PasswordStrengthMeterE[] = {

 // content:
      12,       // revision
       0,       // classname
       0,    0, // classinfo
       1,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags, initial metatype offsets
       1,    1,   20,    2, 0x0a,    1 /* Public */,

 // slots: parameters
    QMetaType::Void, QMetaType::QString,    3,

       0        // eod
};

Q_CONSTINIT const QMetaObject crusty::PasswordStrengthMeter::staticMetaObject = { {
    QMetaObject::SuperData::link<QProgressBar::staticMetaObject>(),
    qt_meta_stringdata_ZN6crusty21PasswordStrengthMeterE.offsetsAndSizes,
    qt_meta_data_ZN6crusty21PasswordStrengthMeterE,
    qt_static_metacall,
    nullptr,
    qt_incomplete_metaTypeArray<qt_meta_tag_ZN6crusty21PasswordStrengthMeterE_t,
        // Q_OBJECT / Q_GADGET
        QtPrivate::TypeAndForceComplete<PasswordStrengthMeter, std::true_type>,
        // method 'updateStrength'
        QtPrivate::TypeAndForceComplete<void, std::false_type>,
        QtPrivate::TypeAndForceComplete<const QString &, std::false_type>
    >,
    nullptr
} };

void crusty::PasswordStrengthMeter::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    auto *_t = static_cast<PasswordStrengthMeter *>(_o);
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: _t->updateStrength((*reinterpret_cast< std::add_pointer_t<QString>>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject *crusty::PasswordStrengthMeter::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *crusty::PasswordStrengthMeter::qt_metacast(const char *_clname)
{
    if (!_clname) return nullptr;
    if (!strcmp(_clname, qt_meta_stringdata_ZN6crusty21PasswordStrengthMeterE.stringdata0))
        return static_cast<void*>(this);
    return QProgressBar::qt_metacast(_clname);
}

int crusty::PasswordStrengthMeter::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QProgressBar::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 1)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 1;
    }
    if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 1)
            *reinterpret_cast<QMetaType *>(_a[0]) = QMetaType();
        _id -= 1;
    }
    return _id;
}
QT_WARNING_POP
