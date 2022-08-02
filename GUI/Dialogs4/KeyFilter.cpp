//-------------------------------------------------------------------
// $Id: KeyFilter.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of KeyFilter class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QPushButton>
#include <QLabel>
#include "service.h"
#include "KeyFilter.h"
#include "KeyDialog.h"
#include "GemsMainWindow.h"

//static const char* GEMS_RKEY_HTML = "gdb_rkey.html";

//============================================================
// KeyEdit dialog
//============================================================

KeyFilter::KeyFilter(QWidget* win, size_t irt, const char* key,
                     const char* caption, bool allowTempl ):
    QDialog( win ),
    iRt(irt),
    allowTemplates(allowTempl)
{
    QLineEdit* pEdit;
    QLabel* pLabel;

    setWindowModality(Qt::WindowModal);
    setWindowTitle(caption);

    TDBKey dbKey( rt[irt]->GetDBKey() );

    if( key )
        dbKey.SetKey(key);

    QGridLayout* editBox = new QGridLayout();
    auto editLine = aMod[iRt]->keyEditField();
    
    for( uint ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        pEdit = new QLineEdit(this);
        aEdit.push_back( std::shared_ptr<QLineEdit>(pEdit) );
        QString str = dynamic_cast<TCModule*>(aMod[irt].get())->GetFldHelp(ii);
        pEdit->setToolTip( str);
        pEdit->setMaxLength( dbKey.FldLen(ii) );
        pEdit->setMaximumWidth( (dbKey.FldLen(ii)+2) * pVisorImp->getCharWidth() );
        pEdit->setMinimumWidth( (dbKey.FldLen(ii)+2) * pVisorImp->getCharWidth() );
        string s(dbKey.FldKey(ii), 0, dbKey.FldLen(ii));
        StripLine(s);
        pEdit->setText( s.c_str() );
        connect( pEdit, SIGNAL(editingFinished()), this, SLOT(setKeyLine()) );

        editBox->addWidget( pEdit, ii, 0, Qt::AlignRight);
        pLabel = new QLabel( str, this);
        editBox->addWidget( pLabel, ii, 1);

        if( !allowTemplates && ii < editLine )
            pEdit->setEnabled(false);
    }
    aEdit[0]->setFocus();

    QHBoxLayout* buttonBox = new QHBoxLayout();
    QPushButton* btn;

    btn = new QPushButton(this);
    btn->setText("&Ok");
    btn->setDefault(true);
    connect( btn, SIGNAL(clicked()), this, SLOT(CmOk()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&Reset");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvSetAll()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&From List");
    connect( btn, SIGNAL(clicked()), this, SLOT(EvGetList()) );
    buttonBox->addWidget( btn );

    buttonBox->addStretch();

    btn = new QPushButton(this);
    btn->setText("&Help");
    connect( btn, SIGNAL(clicked()), this, SLOT(CmHelp()) );
    buttonBox->addWidget( btn );

    btn = new QPushButton(this);
    btn->setText("&Cancel");
    // qt3to4    btn->setAccel( Qt::Key_Escape );
    connect( btn, SIGNAL(clicked()), this, SLOT(reject()) );
    buttonBox->addWidget( btn );

    fullKey = new QLineEdit(this);
    fullKey->setText(dbKey.PackKey());
    fullKey->setFocusPolicy( Qt::ClickFocus );
    fullKey->setReadOnly( true );

    QFrame *line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line_3"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    QVBoxLayout* mainBox = new QVBoxLayout(this);

    mainBox->addWidget(fullKey);
    mainBox->addWidget(line);
    mainBox->addLayout(editBox);

    line = new QFrame(this);
    line->setObjectName(QString::fromUtf8("line"));
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    mainBox->addWidget(line);
    mainBox->addLayout(buttonBox);
    // setLayout(mainBox);
}

///  pVisor->SetStatus( aMod[iRt]->GetFldHelp(ii).c_str() );
void KeyFilter::CmHelp()
{                               
    string dbName =  DBM;
    dbName +="_";
    dbName += string(aMod[iRt]->GetName());
    pVisorImp->OpenHelp(  GEMS_REKEY_HTML, dbName.c_str() );
}

void KeyFilter::CmOk()
{
    if( allowTemplates || SetKeyString().find_first_of("*?") == string::npos )
    {
        accept();
        return;
    }

    vfMessage(this, "Key error", "No templates allowed!", vfErr);
}

string KeyFilter::SetKeyString()
{
    //TDBKey dbKey( rt[iRt].GetDBKey() );
    string Key;

    Key = "";
    for( size_t ii=0/*, jj=0*/; ii<aEdit.size(); ii++/*, jj=Key.length()*/)
    {
        string s = aEdit[ii]->text().toStdString();
        Key += s;
        StripLine(Key);
        //Sveta 04/09/01 ????  if( Key.length()-jj < dbKey.FldLen(ii) )
        Key += ":";
    }

    return Key;
}

void KeyFilter::setKeyLine()
{
    fullKey->setText(SetKeyString().c_str());
}

void KeyFilter::EvSetAll()
{
    for( size_t ii=0; ii<aEdit.size(); ii++ )
        aEdit[ii]->setText("*");
    setKeyLine();
}

void KeyFilter::EvGetList()
{
    KeyDialog dlg(this, iRt, SetKeyString().c_str(), "Key template", false);
    if( !dlg.exec() )
        return;

    TDBKey dbKey( rt[iRt]->GetDBKey() );

    dbKey.SetKey(dlg.getKey().c_str());

    for( uint ii=0; ii<dbKey.KeyNumFlds(); ii++)
    {
        string s(dbKey.FldKey(ii), 0, dbKey.FldLen(ii));
        StripLine(s);
        aEdit[ii]->setText( s.c_str() );
    }
    setKeyLine();
}

string KeyFilter::getFilter()
{
    //  if( result )
    return SetKeyString();
}

//--------------------- End of KeyFilter.cpp ---------------------------


