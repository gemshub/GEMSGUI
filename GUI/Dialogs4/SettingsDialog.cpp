//-------------------------------------------------------------------
// $Id: SettingsDialog.cpp 290 2004-07-07 11:11:38Z andy $
//
// Implementation of SettingsDialog class
//
// Copyright (C) 1996-2007  A.Rysin, S.Dmytriyeva
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (https://qt.io/download-open-source)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the GPL v.3 license

//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <qfontdialog.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qspinbox.h>
#include <qlabel.h>
#include <qradiobutton.h>
#include <qvariant.h>

#include "SettingsDialog.h"
#include "service.h"
#include "visor.h"
#include "help.h"
#include "GemsMainWindow.h"


SettingsDialog::SettingsDialog (QWidget* parent)
    : QDialog(parent )
{

    setupUi(this);
    cellFont = pVisorImp->getCellFont();

    setWindowTitle( "GEM-Selektor: Settings and preferences" );

    pUpdateInterval->setValue( pVisorImp->updateInterval() );

    pLocalDocDir->setText(pVisor->localDocDir().c_str());
    pRemoteHTML->setText(pVisor->remoteHTML().c_str());
    //pLocalDoc->setChecked(pVisor->localDoc());
    //pRemoteDoc->setChecked(!pVisor->localDoc());
    pSysDBDir->setText(pVisor->sysGEMDir().c_str());
    pUserDBDir->setText(pVisor->userGEMDir().c_str());
    pUserDBDir->setText(pVisor->userProfDir().c_str());
    pFontRawName->setText(cellFont.toString());
    pNumDigits->setValue(pVisorImp->getDoubleDigits());
    pUpdateInterval->setValue(pVisorImp->updateInterval());
    pConfigAutosave->setChecked(pVisorImp->getConfigAutosave());
//    pConfigAutosave->setChecked( true );

    if( pVisor->getElemPrMode() )
    {   rbNewPrMode->setChecked( true );
        rbOldPrMode->setChecked( false );
    }
    else
    {   rbNewPrMode->setChecked( false );
        rbOldPrMode->setChecked( true );
    }
    pBuiltinTDB->setText(pVisor->defaultBuiltinTDBL().c_str());

    connect(butGenerate, SIGNAL(clicked()), this, SLOT(CmHelpGenerate()));

    pFontRawName->setReadOnly(true);	// no meaning for Win32 (now)
}


SettingsDialog::~SettingsDialog()
{}

void SettingsDialog::languageChange()
{
    retranslateUi(this);
}

void
SettingsDialog::accept()
{
    CmApply();
    QDialog::accept();
}

void
SettingsDialog::CmApply()
{
    pVisorImp->setCellFont(cellFont);

    pVisorImp->setDoubleDigits(pNumDigits->value());
    pVisorImp->setUpdateInterval( pUpdateInterval->value() );
    pVisorImp->setConfigAutosave( pConfigAutosave->isChecked() );
    pVisor->setElemPrMode(rbNewPrMode->isChecked());

    pVisor->setLocalDocDir(pLocalDocDir->text().toLatin1().data());
    pVisor->setRemoteHTML(pRemoteHTML->text().toLatin1().data());

    pVisor->setDefaultBuiltinTDB(pBuiltinTDB->text().toLatin1().data());

    //pVisor->setLocalDoc(pLocalDoc->isChecked());

    //pVisorImp->Update(true);
}


void
SettingsDialog::CmHelp()
{
  pVisorImp->OpenHelp( GEMS_SETUP_HTML, nullptr );
}

void SettingsDialog::CmHelpGenerate()
{
  try
    {
           QString qhpFile = pRemoteHTML->text()+"gems3helpconfig.qhp";
             HelpConfigurator rr;
             if( rr.readDir(pRemoteHTML->text().toLatin1().data()))
                rr.writeFile(qhpFile.toLatin1().data());

             //if( HelpWindow::pDia )
             //{
             //   HelpWindow::pDia->close();
             //   delete HelpWindow::pDia;
             //   build new file
                if (!pVisorImp->proc)
                    pVisorImp->proc = new QProcess();
            
                if (pVisorImp->proc->state() != QProcess::Running) 
                {
                    
                   QString docPath =  pRemoteHTML->text();
                   QString app;
            #ifdef __unix
            #ifdef __APPLE__
//                    app += QLatin1String("/Applications/Gems3.app/Contents/MacOS/qcollectiongenerator");    // expected to work
                    app += QLatin1String("qcollectiongenerator");
            #else
// app = pVisor->sysGEMDir().c_str() + QLatin1String("/qcollectiongenerator");
//                   app = QLatin1String(getenv("HOME"));
//                   app += QLatin1String("/Gems3-app/qcollectiongenerator");
                     app += QLatin1String("qcollectiongenerator");
            #endif
            #else    // windows
                    app += QLatin1String("qcollectiongenerator.exe");
            #endif        
                    QStringList args;
                    args << docPath + QLatin1String("gems3helpconfig.qhcp")
                        << QLatin1String("-o")
                        << docPath + QLatin1String("gems3help.qhc");
                        ;

                    pVisorImp->proc->start(app, args);
                    cout << app.toLatin1().data() << endl;
                    cout << args[2].toLatin1().data() << endl;
            
                    if (!pVisorImp->proc->waitForStarted()) 
                    {
                        Error( "Gems3", "Unable to launch qcollectiongenerator");
                    }    
                }
             // open it
             //   (new HelpWindow(  0  ));
             // }

   }
   catch(TError& e)
   {
       vfMessage(this, e.title.c_str(), e.mess.c_str() );
   }
}


void SettingsDialog::CmSysDBDirSelect()
{
    qWarning("SettingsDialogData::CmSysDBDirSelect(): Not implemented yet");
}

void SettingsDialog::CmUserDBDirSelect()
{
    qWarning("SettingsDialogData::CmUserDBDirSelect(): Not implemented yet");
}

/*
void
SettingsDialog::CmSysDBDirSelect()
{
    QString dir( pDBDir->text() );

    QFileDialog file_dlg( dir, QString::null, this, 0, true);
    file_dlg.setMode(QFileDialog::DirectoryOnly);
    if( file_dlg.exec() )
    {
//	pDBDir->setText( file_dlg.dirPath() );
	pDBDir->setText( "Changing DB dirs on the fly is not supported! :(");
    }
}


void
SettingsDialog::CmUserDBDirSelect()
{
    QString dir( pUserDBDir->text() );

    QFileDialog file_dlg( dir, QString::null, this, 0, true);
    file_dlg.setMode(QFileDialog::DirectoryOnly);
    if( file_dlg.exec() )
    {
//	pUserDBDir->setText( file_dlg.dirPath() );
	pUserDBDir->setText( "Changing DB dirs on the fly is not supported! :(");
    }
}
*/

void
SettingsDialog::CmChangeFont()
{
    bool ok;
    QFont selected_font = QFontDialog::getFont(&ok, cellFont, this);

    if( ok )
    {
        cellFont = selected_font;
        pFontRawName->setText( cellFont.toString() );
    }
}

void
SettingsDialog::CmDefaultFont()
{
    cellFont = pVisorImp->getDefaultFont();
    pFontRawName->setText( cellFont.toString() );
}

//--------------------- End of SettingsDialog.cpp ---------------------------

