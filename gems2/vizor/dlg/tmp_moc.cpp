/****************************************************************************
** MainDialog meta object code from reading C++ file 'MainDialog.h'
**
** Created: Mon Nov 5 23:39:27 2001
**      by: The Qt MOC ($Id$)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#undef QT_NO_COMPAT
#include "MainDialog.h"
#include <qmetaobject.h>
#include <qapplication.h>

#include <private/qucomextra_p.h>
#if !defined(Q_MOC_OUTPUT_REVISION) || (Q_MOC_OUTPUT_REVISION != 19)
#error "This file was generated using the moc from 3.0.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

const char *MainDialog::className() const
{
    return "MainDialog";
}

QMetaObject *MainDialog::metaObj = 0;
static QMetaObjectCleanUp cleanUp_MainDialog;

#ifndef QT_NO_TRANSLATION
QString MainDialog::tr( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainDialog", s, c, QApplication::DefaultCodec );
    else
	return QString::fromLatin1( s );
}
#ifndef QT_NO_TRANSLATION_UTF8
QString MainDialog::trUtf8( const char *s, const char *c )
{
    if ( qApp )
	return qApp->translate( "MainDialog", s, c, QApplication::UnicodeUTF8 );
    else
	return QString::fromUtf8( s );
}
#endif // QT_NO_TRANSLATION_UTF8

#endif // QT_NO_TRANSLATION

QMetaObject* MainDialog::staticMetaObject()
{
    if ( metaObj )
	return metaObj;
    QMetaObject* parentObject = MainDialogData::staticMetaObject();
    static const QUMethod slot_0 = {"CmSettings", 0, 0 };
    static const QUMethod slot_1 = {"CmEQDialog", 0, 0 };
    static const QUMethod slot_2 = {"CmAbout", 0, 0 };
    static const QUMethod slot_3 = {"CmHelp", 0, 0 };
    static const QUMethod slot_4 = {"CmExit", 0, 0 };
    static const QUMethod slot_5 = {"CmDBDialog", 0, 0 };
    static const QMetaData slot_tbl[] = {
	{ "CmSettings()", &slot_0, QMetaData::Protected },
	{ "CmEQDialog()", &slot_1, QMetaData::Protected },
	{ "CmAbout()", &slot_2, QMetaData::Protected },
	{ "CmHelp()", &slot_3, QMetaData::Protected },
	{ "CmExit()", &slot_4, QMetaData::Protected },
	{ "CmDBDialog()", &slot_5, QMetaData::Protected }
    };
    metaObj = QMetaObject::new_metaobject(
	"MainDialog", parentObject,
	slot_tbl, 6,
	0, 0,
#ifndef QT_NO_PROPERTIES
	0, 0,
	0, 0,
#endif // QT_NO_PROPERTIES
	0, 0 );
    cleanUp_MainDialog.setMetaObject( metaObj );
    return metaObj;
}

void* MainDialog::qt_cast( const char* clname )
{
    if ( !qstrcmp( clname, "MainDialog" ) ) return (MainDialog*)this;
    return MainDialogData::qt_cast( clname );
}

bool MainDialog::qt_invoke( int _id, QUObject* _o )
{
    switch ( _id - staticMetaObject()->slotOffset() ) {
    case 0: CmSettings(); break;
    case 1: CmEQDialog(); break;
    case 2: CmAbout(); break;
    case 3: CmHelp(); break;
    case 4: CmExit(); break;
    case 5: CmDBDialog(); break;
    default:
	return MainDialogData::qt_invoke(_id,_o);
    }
    return TRUE;
}

bool MainDialog::qt_emit( int _id, QUObject* _o )
{
    return MainDialogData::qt_emit(_id,_o);
}
#ifndef QT_NO_PROPERTIES

bool MainDialog::qt_property( int _id, int _f, QVariant* _v)
{
    return MainDialogData::qt_property( _id, _f, _v);
}
#endif // QT_NO_PROPERTIES
