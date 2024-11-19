//-------------------------------------------------------------------
// $Id: module_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GemsMainWindow class
//
// Copyright (C) 2011  S.Dmytriyeva
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

#include <QMdiArea>

#if QT_VERSION >= 0x050000
#include <QtWidgets>
#else
#include <QtGui>
#endif

#include "visor.h"
#include "m_param.h"
#include "GemsMainWindow.h"
#include "SettingsDialog.h"
#include "NewSystemDialog.h"
#include "GraphDialogN.h"
#include "ui_GemsMainWindow4.h"

void TVisorImp::updateMenus()
{
    // addition work with MdiChild
    bool hasMdiChild = (mdiArea->activeSubWindow() != nullptr);
    ui->menubar->clear();
    ui->toolBar_2->hide();
    ui->toolBar_3->hide();
    ui->toolBar_4->hide();
    ui->toolBar_5->hide();

    ui->menubar->addAction(ui->menu_Modules->menuAction());

    if( hasMdiChild )
    {
        TCModuleImp *mdwin = activeMdiChild();
        if( mdwin && mdwin->getViewMode()   )
        { // if we open graph, submodules and ... hide menu_record !!!
             if( mdwin->rtNum() >= MD_RMULTS
                    && pVisor->ProfileMode == MDD_SYSTEM )
            {
                changeModulesKeys( RT_SYSEQ  );
                ui->menubar->addAction(ui->menu_Record->menuAction());
                ui->action_Calculate->setEnabled(false);
                ui->toolBar_2->show();
                ui->toolBar_3->show();
                ui->toolBar_5->show();
                ui->menubar->addAction(ui->smenu_Data->menuAction());
                ui->menubar->addAction(ui->smenu_Calc->menuAction());
                ui->menubar->addAction(ui->smenu_View->menuAction());
                ui->menubar->addAction(ui->smenu_Print->menuAction());
            }
            else
            {
                changeModulesKeys( mdwin->rtNum()  );
                ui->menubar->addAction(ui->menu_Record->menuAction());
                ui->action_Calculate->setEnabled(true);
                ui->toolBar_2->show();
                ui->toolBar_4->show();
                ui->toolBar_5->show();
                if( mdwin->rtNum() ==  RT_RTPARM ||  mdwin->rtNum() ==  RT_PROCES ||
                        mdwin->rtNum() ==  RT_UNSPACE ||  mdwin->rtNum() ==  RT_GTDEMO ||
                        mdwin->rtNum() ==  RT_GEM2MT )
                    ui->action_Plot->setEnabled(true);
                else
                    ui->action_Plot->setEnabled(false);

                if( pVisor->ProfileMode == MDD_DATABASE)
                {
                    ui->menubar->addAction(ui->menuRecord_List->menuAction());
                    ui->menubar->addAction(ui->menu_Database_Files->menuAction());
                }
            }
        }
        else
        {   // for Chemical Systems must be other menu in future !!!
            NewSystemDialog *syswin = activeNewSystem();
            if( syswin )
            { // if we open graph, submodules and ... hide menu_record !!!
                changeModulesKeys( RT_SYSEQ );
                ui->menubar->addAction(ui->menu_Record->menuAction());
                ui->action_Calculate->setEnabled(false);
                ui->toolBar_2->show();
                ui->toolBar_3->show();
                ui->toolBar_5->show();
                ui->menubar->addAction(ui->smenu_Data->menuAction());
                ui->menubar->addAction(ui->smenu_Calc->menuAction());
                ui->menubar->addAction(ui->smenu_View->menuAction());
                ui->menubar->addAction(ui->smenu_Print->menuAction());
            }
            else
                changeModulesKeys( -1 );
        }
    }
    else
        changeModulesKeys( -1 );

    if(hasMdiChild)
    {
        disconnect( pModuleName, &QComboBox::currentTextChanged, this, &TVisorImp::setActiveSubWindowName);
        pModuleName->setCurrentText( nameMdiChild( mdiArea->activeSubWindow()->widget() ).c_str());
        connect( pModuleName, &QComboBox::currentTextChanged, this, &TVisorImp::setActiveSubWindowName);
    }
    //else
    //    pModuleName->clear();

    ui->menubar->addAction(ui->menuWindow->menuAction());
    ui->menubar->addAction(ui->smenu_Help->menuAction());

    ui->actClose->setEnabled(hasMdiChild);
    ui->actCloseAll->setEnabled(hasMdiChild);
    ui->actTile->setEnabled(hasMdiChild);
    ui->actCascade->setEnabled(hasMdiChild);
    separatorAct->setVisible(hasMdiChild);
}


//enabled = !(pVisor->ProfileMode == MDD_SYSTEM && LoadMessage::pDia );
void TVisorImp::setMenuEnabled( bool menuEnabled )
{
    ui->menu_Record->setEnabled(menuEnabled);
    ui->smenu_Data->setEnabled(menuEnabled);
    ui->smenu_Calc->setEnabled(menuEnabled);
    ui->smenu_View->setEnabled(menuEnabled);
    ui->smenu_Print->setEnabled(menuEnabled);
    ui->toolBar->setEnabled(menuEnabled);
    ui->toolBar_2->setEnabled(menuEnabled);
    ui->toolBar_4->setEnabled(menuEnabled);
    ui->toolBar_3->setEnabled(menuEnabled);
    ui->toolBar_5->setEnabled(menuEnabled);
    itemWidget1->setEnabled(menuEnabled);
}

string TVisorImp::nameMdiChild( QWidget *p )
{
    string mdName;

    TCModuleImp *child = qobject_cast<TCModuleImp *>(p);
    if(child)
        mdName = child->moduleName();
    else
    {
        NewSystemDialog *wn = qobject_cast<NewSystemDialog *>(p);
        if( wn )
            mdName = "SingleSystem";
        else
        {
            GraphDialog *dlg = qobject_cast<GraphDialog *>(p);
            if( dlg )
                mdName = dlg->moduleName();
            else
                mdName = "undefined";
        }
    }
    return mdName;
}

QIcon TVisorImp::iconMdiChild( QWidget *p )
{
    string iconName;

    TCModuleImp *child = qobject_cast<TCModuleImp *>(p);
    if(child)
        iconName = child->iconFile();
    else
    {
        NewSystemDialog *wn = qobject_cast<NewSystemDialog *>(p);
        if( wn )
            iconName = ":/modules/Icons/SysEqModuleIcon.png";
        else
        {
            GraphDialog *dlg = qobject_cast<GraphDialog *>(p);
            if( dlg )
                iconName = dlg->iconFile();
            else
                iconName = ":/Icons/gems50.png";
        }
    }
    QIcon icon;
    icon.addFile(QString::fromUtf8(iconName.c_str()), QSize(), QIcon::Normal, QIcon::Off);
    return icon;
}

int TVisorImp::nRTofActiveSubWindow()
{
    int nRT = -1;

    TCModuleImp *child = activeMdiChild();
    if(child)
    {
        nRT = child->rtNumRecord();
    }
    else
    {
        NewSystemDialog *wn = activeNewSystem();
        if( wn )
            nRT = RT_SYSEQ;
    }
    return nRT;
}


void TVisorImp::updateWindowMenu()
{
    ui->menuWindow->clear();
    ui->menuWindow->addAction(ui->actClose);
    ui->menuWindow->addAction(ui->actCloseAll);
    ui->menuWindow->addSeparator();
    ui->menuWindow->addAction(ui->actTile);
    ui->menuWindow->addAction(ui->actCascade);
    ui->menuWindow->addAction(separatorAct);

    QList<QMdiSubWindow *> windows = mdiArea->subWindowList();
    separatorAct->setVisible(!windows.isEmpty());
    QString mdName;

    for (int i = 0; i < windows.size(); ++i)
    {
        mdName = nameMdiChild( windows.at(i)->widget() ).c_str();
        QString text;
        if (i < 9)
            text = tr("&%1 %2").arg(i + 1).arg(mdName);
        else
            text = tr("%1 %2").arg(i + 1).arg(mdName);

        QAction *action  = ui->menuWindow->addAction(text);
        action->setCheckable(true);
        action ->setChecked(windows.at(i) == mdiArea->activeSubWindow());
        QWidget * awindow = windows.at(i);
        connect(action, &QAction::triggered, this, [this, awindow ](){ setActiveSubWindow(awindow); });
    }
}


void  TVisorImp::moveToolBar( int , int )
{
    auto size =  ui->toolBar->size().width();

    if(pVisor->ProfileMode == MDD_SYSTEM ) {
        size = max(size, toolProject->size().width());
        ui->toolBar->setFixedWidth(size);
        toolProject->setFixedWidth(size);
    }
    else {
        size = max(size, toolDataBase->size().width());
        ui->toolBar->setFixedWidth(size);
        toolDataBase->setFixedWidth(size);
    }

    ui->toolModule->setFixedWidth(splH->widget(0)->width());
}

// -----------------------------------------------------------
// Actions and commands

//  Connect all actions
void TVisorImp::setActions()
{

    // Modules
    connect( ui->actionIComp, SIGNAL( triggered()), this, SLOT(CmIComp()));
    connect( ui->actionDComp, SIGNAL( triggered()), this, SLOT(CmDComp()));
    connect( ui->actionReacDC, SIGNAL( triggered()), this, SLOT(CmReacDC()));
    connect( ui->actionRTparm, SIGNAL( triggered()), this, SLOT(CmRTparm()));
    connect( ui->actionPhase, SIGNAL( triggered()), this, SLOT(CmPhase()));
    connect( ui->actionCompos, SIGNAL( triggered()), this, SLOT(CmCompos()));

    connect( ui->actionSysEq, SIGNAL( triggered()), this, SLOT(CmSysEq()));
    connect( ui->actionProcess, SIGNAL( triggered()), this, SLOT(CmProcess()));
    connect( ui->actionGEM2MT, SIGNAL( triggered()), this, SLOT(CmGEM2MT()));
    connect( ui->actionGtDemo, SIGNAL( triggered()), this, SLOT(CmGtDemo()));
    connect( ui->actionDualTh, SIGNAL( triggered()), this, SLOT(CmDualTh()));
    connect( ui->actionUnSpace, SIGNAL( triggered()), this, SLOT(CmUnSpace()));
    connect( ui->actionProject, SIGNAL( triggered()), this, SLOT(CmProject()));
    connect( ui->actionSDref, SIGNAL( triggered()), this, SLOT(CmSDref()));
    connect( ui->actionConst, SIGNAL( triggered()), this, SLOT(CmConst()));
    connect( ui->actionExit, SIGNAL( triggered()), this, SLOT(close()));

    // Help
    connect( ui->sactionHelp, SIGNAL( triggered()), this, SLOT(CmHelp()));
    connect( ui->sactionHowto, SIGNAL( triggered()), this, SLOT(CmHowto()));
    connect( ui->sactionScript, SIGNAL( triggered()), this, SLOT(CmScript()));
    connect( ui->sactionMoreBCC, SIGNAL( triggered()), this, SLOT(CmMoreBCC()));
    connect( ui->sactionMoreIPM, SIGNAL( triggered()), this, SLOT(CmMoreIPM()));
    connect( ui->sactionAbout, SIGNAL( triggered()), this, SLOT(CmHelpAbout()));
    connect( ui->actionAuthors, SIGNAL( triggered()), this, SLOT(CmHelpAuthors()));
    connect( ui->actionThanks, SIGNAL( triggered()), this, SLOT(CmHelpThanks()));
    connect( ui->actionLicense, SIGNAL( triggered()), this, SLOT(CmHelpLicense()));

    connect( ui->action_Help_2, SIGNAL( triggered()), this, SLOT(CmHelp2()));
    connect( ui->action_Preferences, SIGNAL( triggered()), this, SLOT(CmSettingth()));
    connect( ui->actionTutorial, SIGNAL( triggered()), this, SLOT(CmTutorial()));

    // Record
    connect( ui->action_Create, SIGNAL( triggered()), this, SLOT(CmCreate()));
    connect( ui->action_New, SIGNAL( triggered()), this, SLOT(CmNew()));
    connect( ui->action_Display, SIGNAL( triggered()), this, SLOT(CmShow()));
    connect( ui->action_Remake, SIGNAL( triggered()), this, SLOT(CmDerive()));
    connect( ui->action_Calculate, SIGNAL( triggered()), this, SLOT(CmCalc()));
    connect( ui->action_Save, SIGNAL( triggered()), this, SLOT(CmSave()));
    connect( ui->actionSave_As, SIGNAL( triggered()), this, SLOT(CmSaveAs()));
    connect( ui->action_Delete, SIGNAL( triggered()), this, SLOT(CmDelete()));
    connect( ui->action_Plot, SIGNAL( triggered()), this, SLOT(CmPlot()));
    connect( ui->action_Print, SIGNAL( triggered()), this, SLOT(CmPrint()));
    //connect( ui->action_Close, SIGNAL( triggered()), this, SLOT(CloseWin()));
    connect( ui->actionNext, SIGNAL( triggered()), this, SLOT(CmNext()));
    connect( ui->actionPrevious, SIGNAL( triggered()), this, SLOT(CmPrevious()));
    connect( ui->actionFilter, SIGNAL( triggered()), this, SLOT(CmFilter()));
    connect( ui->actionFile, SIGNAL( triggered()), this, SLOT(EvFileDown()));

    // Record list
    connect( ui->action_Copy, SIGNAL( triggered()), this, SLOT(CmCopyList()));
    connect( ui->action_Rename, SIGNAL( triggered()), this, SLOT(CmRenameList()));
    connect( ui->action_Move, SIGNAL( triggered()), this, SLOT(CmTransferList()));
    connect( ui->action_Delete_list, SIGNAL( triggered()), this, SLOT(CmDeleteList()));
    connect( ui->action_List, SIGNAL( triggered()), this, SLOT(CmKeysToTXT()));
    connect( ui->action_Backup, SIGNAL( triggered()), this, SLOT(CmBackup()));
    connect( ui->action_Restore, SIGNAL( triggered()), this, SLOT(CmRestore()));
    connect( ui->action_Export, SIGNAL( triggered()), this, SLOT(CmExport()));
    connect( ui->action_Import, SIGNAL( triggered()), this, SLOT(CmImport()));
    connect( ui->actionExport_Json, SIGNAL( triggered()), this, SLOT(CmBackuptoJson()));
    connect( ui->actionImport_Json, SIGNAL( triggered()), this, SLOT(CmRestorefromJson()));

    // Database files
    connect( ui->action_Selection, SIGNAL( triggered()), this, SLOT(CmReOpenFileList()));
    connect( ui->action_Add_Link, SIGNAL( triggered()), this, SLOT(CmAddOpenFile()));
    connect( ui->action_New_2, SIGNAL( triggered()), this, SLOT(CmAddFileToList()));
    connect( ui->action_Compress, SIGNAL( triggered()), this, SLOT(CmRebildFile()));

    // Window
    connect( ui->actClose, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));
    connect(ui->actCloseAll, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));
    connect(ui->actTile, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));
    connect(ui->actCascade, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));
    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);
    ui->menuWindow->addAction(separatorAct);
    updateWindowMenu();
    connect(ui->menuWindow, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    //NewSystemDialog
    connect( ui->sactionOpenMTPARAM, SIGNAL(triggered()), this, SLOT(CmOpen_MTPARAM()));
    connect( ui->sactionOpen_SYSTEM_ICOMP, SIGNAL(triggered()), this, SLOT(CmOpen_SYSTEM_ICOMP()));
    connect( ui->sactionINSERT_SYSTEM, SIGNAL(triggered()), this, SLOT(CmInsert_SYSTEM()));
    connect( ui->sactionOutMulti, SIGNAL(triggered()), this, SLOT(CmOutMulti()));
    connect( ui->sactionReadMulti, SIGNAL(triggered()), this, SLOT(CmReadMulti()));
    connect( ui->actionExport_JSON_files, SIGNAL(triggered()), this, SLOT(CmProjectExportJson()));
    connect( ui->actionImport_JSON_files, SIGNAL(triggered()), this, SLOT(CmProjectImportJson()));

    connect( ui->saction_BCC, SIGNAL(triggered()), this, SLOT(CmRunBCC()));
    connect( ui->saction_IPM, SIGNAL(triggered()), this, SLOT(CmRunIPM()));
    //connect( actionSimplex, SIGNAL( triggered()), this, SLOT(CmSimplex()));
    //connect( actionPrecise, SIGNAL( triggered()), this, SLOT(CmPrecise()));
    //connect( actionStepwise, SIGNAL( triggered()), this, SLOT(CmStepwise()));

    connect( ui->sactionEQUSTAT_ICOMP, SIGNAL( triggered()), this, SLOT(CmOpen_EQSTAT_ICOMP()));
    connect( ui->sactionOpenMULTI, SIGNAL( triggered()), this, SLOT(CmOpen_MULTI()));

    connect( ui->sactionPrintMtparm, SIGNAL( triggered()), this, SLOT(CmPrintMtparam()));
    connect( ui->sactionPrintSystem, SIGNAL( triggered()), this, SLOT(CmPrintSystem()));
    connect( ui->sactionPrintEqstat, SIGNAL( triggered()), this, SLOT(CmPrintEqstat()));
    connect( ui->sactionPrInput, SIGNAL( triggered()), this, SLOT(CmPrInput()));
    connect( ui->sactionPrResults, SIGNAL( triggered()), this, SLOT(CmPrResults()));

    pModuleName = new QComboBox( ui->toolModule );
    pModuleName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    connect( pModuleName, &QComboBox::currentTextChanged, this, &TVisorImp::setActiveSubWindowName);
    ui->toolModule->addWidget( pModuleName );

    pLine = new QLineEdit( ui->toolBar_6 );
    pLine->setEnabled( true );
    pLine->setFocusPolicy( Qt::ClickFocus );
    pLine->setReadOnly( true );
    pLine->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    ui->toolBar_6->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    ui->toolBar_6->addWidget( pLine ); // setStretchableWidget( pLine );

}

// menu commands

void TVisorImp::CmIComp()
{
    OpenModule(this, RT_ICOMP,0,true, true);
}

void TVisorImp::CmDComp()
{
    OpenModule(this, RT_DCOMP,0,true, true);
}

void TVisorImp::CmReacDC()
{
    OpenModule(this, RT_REACDC,0,true, true);
}

void TVisorImp::CmRTparm()
{
    OpenModule(this, RT_RTPARM,0,true, true);
}

void TVisorImp::CmPhase()
{
    OpenModule(this, RT_PHASE,0,true, true);
}

void TVisorImp::CmCompos()
{
    OpenModule(this, RT_COMPOS,0,true, true);
}

void TVisorImp::CmSysEq()
{
    OpenModule(this, RT_SYSEQ,0,true, true);
}

void TVisorImp::CmProcess()
{
    OpenModule(this, RT_PROCES,0,true, true);
}

void TVisorImp::CmGEM2MT()
{
    OpenModule(this, RT_GEM2MT,0,true, true);
}

void TVisorImp::CmGtDemo()
{
    OpenModule(this, RT_GTDEMO,0,true, true);
}

void TVisorImp::CmDualTh()
{
    OpenModule(this, RT_DUALTH,0,true, true);
}

void TVisorImp::CmUnSpace()
{
    OpenModule(this, RT_UNSPACE,0,true, true);
}

void TVisorImp::CmProject()
{
    OpenModule(this, RT_PARAM,1,true, true);
}

void TVisorImp::CmSDref()
{
    OpenModule(this, RT_SDATA,0,true, true);
}

void TVisorImp::CmConst()
{
    OpenModule(this, RT_CONST,0,true, true);
}


void TVisorImp::CmSettingth()
{
    SettingsDialog dlg(this);
    dlg.exec();
}

void TVisorImp::CmTutorial()
{
    OpenHelp( GEMS_TUTOR_HTML );
}

void TVisorImp::CmHelp()
{
    TCModuleImp *actwin = activeMdiChild();
    if( actwin )
    {
        aMod[actwin->rtNum()]->CmHelp();
    }
    else
    {
        NewSystemDialog *wn = activeNewSystem();
        if( wn )
            OpenHelp( GEMS_ONESYS_HTML );
        else
            if( pVisor->ProfileMode == MDD_DATABASE )
                OpenHelp( GEMS_TDBAS_HTML );
            else
                OpenHelp( GEMS_MODES_HTML );
    }
}

void TVisorImp::CmMoreBCC()
{                        // Create this HTML file
    OpenHelp( GEMS_HOWTOB_HTML );
}

void TVisorImp::CmMoreIPM()
{                        // Create this HTML file
    OpenHelp( GEMS_IPM_HTML );
}

void TVisorImp::CmHowto()
{
    OpenHelp( GEMS_HOWTO_HTML );
}

void TVisorImp::CmHelpAbout()
{
    OpenHelp( GEMS_ABOUT_HTML );
}

void TVisorImp::CmHelpAuthors()
{
    OpenHelp( GEMS_AUTHORS_HTML );
}

void TVisorImp::CmHelpThanks()
{
    OpenHelp( GEMS_THANKS_HTML );
}

void TVisorImp::CmHelpLicense()
{
    OpenHelp( GEMS_LICENSE_HTML );
}

//TCM_EV_COMMAND(CM_SCRIPT, CmScript);
void TVisorImp::CmScript()
{
    TCModuleImp *actwin = activeMdiChild();
    if( actwin )
    {

        dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmScript();
    }
    else
    {   NewSystemDialog *wn = activeNewSystem();
        if( wn )
            dynamic_cast<TCModule*>(aMod[RT_SYSEQ].get())->CmScript();
    }
}

//-----------------------------------------------------------------------------
// old commands

//TCM_EV_COMMAND(CM_RECCREATE, CmCreate);
//TCM_EV_COMMAND(CM_121, CmCreateinProfile);
void TVisorImp::CmCreate()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
    {   wn->CmCreate();
        NewSystemDialog::pDia->setCurrentTab(0);
    }
    else
    {
        TCModuleImp *actwin = activeMdiChild();
        if( actwin )
        {  if(pVisor->ProfileMode == MDD_DATABASE)
                dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmCreate();
            else
                dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmCreateinProfile();
        }
    }
}

//TCM_EV_COMMAND(CM_RECNEW, CmNew);
//TCM_EV_COMMAND(CM_12, CmNewinProfile);
void TVisorImp::CmNew()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
    {   wn->CmNew();
        NewSystemDialog::pDia->setCurrentTab(0);
    }
    else
    {  TCModuleImp *actwin = activeMdiChild();
        if( actwin )
        {  if(pVisor->ProfileMode == MDD_DATABASE)
                dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmNew();
            else
                dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmNewinProfile();
        }
    }
}

//TCM_EV_COMMAND(CM_11, CmLoadinProfile);
void TVisorImp::CmShow( const char * key )
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
    {
        wn->CmSelect( key );
        defineModuleKeysList( RT_SYSEQ );
    }
    else
    {
        TCModuleImp *actwin = activeMdiChild();
        if( actwin )
        {
            if(pVisor->ProfileMode == MDD_DATABASE)
                dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmShow( key );
            else
                dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmLoadinProfile( key );

            defineModuleKeysList( actwin->rtNumRecord() );
            QMdiSubWindow * grDlg = findMdiGraph(actwin->moduleName().c_str());
            if( grDlg )
            {
                //// grDlg->widget()->close();
                QMdiSubWindow *wn1 = findMdiChild(actwin->moduleName().c_str());
                //mdiArea->setActiveSubWindow( grDlg );
                //mdiArea->closeActiveSubWindow();
                mdiArea->setActiveSubWindow( wn1 );
                CmPlot();
                tbKeys->setFocus();
            }
        }
    }
}

//TCM_EV_COMMAND(CM_RECDERIVE, CmDerive);
void TVisorImp::CmDerive()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmRemake();
    else
    {
        TCModuleImp *actwin = activeMdiChild();
        if( actwin )
            dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmDerive();
    }
}

//TCM_EV_COMMAND(CM_RECCALC, CmCalc);
void TVisorImp::CmCalc()
{
    TCModuleImp *actwin = activeMdiChild();
    if( actwin )
        dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmCalc();
}

void TVisorImp::CmSave()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
    {   wn->CmSave();
        //defineModuleKeysList( RT_SYSEQ );
    }
    else
    {
        TCModuleImp *actwin = activeMdiChild();
        if( actwin )
        {
            dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmSaveM();
            // defineModuleKeysList( actwin->rtNumRecord() );
        }
    }
}

//TCM_EV_COMMAND(CM_RECSAVEAS, CmSaveAs);
void TVisorImp::CmSaveAs()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
    {   wn->CmSaveAs();
        //defineModuleKeysList( RT_SYSEQ );
    }
    else
    {
        TCModuleImp *actwin = activeMdiChild();
        if( actwin )
        {
            dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmSaveAs();
            // defineModuleKeysList( actwin->rtNumRecord() );
        }
    }
}

//TCM_EV_COMMAND(CM_RECDELETE, CmDelete);
void TVisorImp::CmDelete()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
    {   wn->CmDelete();
        defineModuleKeysList( RT_SYSEQ );
    }
    else
    {
        TCModuleImp *actwin = activeMdiChild();
        if( actwin )
        {  dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmDelete();
            defineModuleKeysList( actwin->rtNumRecord() );
        }
    }
}

//TCM_EV_COMMAND(CM_RECPLOT, CmPlot);
void TVisorImp::CmPlot()
{
    TCModuleImp *actwin = activeMdiChild();
    if( actwin )
    {
        dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmPlot();
    }
    else
    {   NewSystemDialog *wn = activeNewSystem();
        if( wn )
            dynamic_cast<TCModule*>(aMod[RT_SYSEQ].get())->CmPlot();
    }
}

//TCM_EV_COMMAND(CM_PRINT, CmPrint);
void TVisorImp::CmPrint()
{
    TCModuleImp *actwin = activeMdiChild();
    if( actwin )
    {
        dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->CmPrint();
    }
    else
    {   NewSystemDialog *wn = activeNewSystem();
        if( wn )
            //  dynamic_cast<TCModule *>(aMod[RT_SYSEQ].get())->CmPrint();
            wn->CmPrintEqstat();
    }
}


void TVisorImp::CmFilter()
{
    TCModuleImp *actwin = activeMdiChild();
    if( actwin )
    {
        dynamic_cast<TCModule*>(aMod[actwin->rtNum()].get())->CmFilter();
        pFilterKey->setText(dynamic_cast<TCModule*>(aMod[actwin->rtNumRecord()].get())->getFilter());
        defineModuleKeysList( actwin->rtNumRecord() );
    }
    else
    {   NewSystemDialog *wn = activeNewSystem();
        if( wn )
        {   dynamic_cast<TCModule*>(aMod[RT_SYSEQ].get())->CmFilter();
            pFilterKey->setText(dynamic_cast<TCModule*>(aMod[RT_SYSEQ].get())->getFilter());
            defineModuleKeysList( RT_SYSEQ );
        }
    }
}


void TVisorImp::CmNext()
{
    /*TCModuleImp *actwin = activeMdiChild();
  if( actwin )
    dynamic_cast<TCModule *>(aMod[actwin->rtNum()].get())->CmNext();
  else
  {   NewSystemDialog *wn = activeNewSystem();
      if( wn )
         wn->CmNext();
  }*/
    if( currentNrt >=0)
    {
        int row = tbKeys->currentRow();
        if( row < tbKeys->rowCount()-1 )
        {  row++;
            QTableWidgetItem *curItem = tbKeys->item(row,0);
            tbKeys->setCurrentItem( curItem );
            tbKeys->scrollToItem( curItem );
            openRecordKey( row, 0  );
        }
    }
}

void TVisorImp::CmPrevious()
{
    /*TCModuleImp *actwin = activeMdiChild();
  if( actwin )
    dynamic_cast<TCModule *>(aMod[actwin->rtNum()].get())->CmPrevious();
  else
  {   NewSystemDialog *wn = activeNewSystem();
      if( wn )
         wn->CmPrevious();
  }*/
    if( currentNrt >=0)
    {
        int row = tbKeys->currentRow();
        if( row > 0  )
        {  row--;
            QTableWidgetItem *curItem = tbKeys->item(row,0);
            tbKeys->setCurrentItem( curItem );
            tbKeys->scrollToItem( curItem );
            openRecordKey( row, 0  );
        }
    }
}

// for Qt first argument of macro is ignored
#define TCM_EV_COMMAND(mm,ff)	void TVisorImp::ff() \
{ TCModuleImp *actwin = activeMdiChild();\
    if( actwin )\
{  dynamic_cast<TCModule*>(aMod[actwin->rtNum()].get())->ff(); \
    int nRT_ = static_cast<int>(actwin->rtNum()); \
    defineModuleKeysList( nRT_ );  }    \
    }

TCM_EV_COMMAND( CM_REBILDFILE, CmRebildFile)
TCM_EV_COMMAND(CM_ADDFILETOLIST, CmAddFileToList)
TCM_EV_COMMAND(CM_ADDOPENFILE, CmAddOpenFile)
TCM_EV_COMMAND(CM_REOPENFILELIST, CmReOpenFileList)

TCM_EV_COMMAND(CM_CHOISELIST, CmCopyList)
TCM_EV_COMMAND(CM_RENAMELIST, CmRenameList)
TCM_EV_COMMAND(CM_TRANSFERLIST, CmTransferList)
TCM_EV_COMMAND(CM_DELETELIST, CmDeleteList)
TCM_EV_COMMAND(CM_KEYSTOTXT, CmKeysToTXT)
TCM_EV_COMMAND(CM_IMPORT, CmImport)
TCM_EV_COMMAND(CM_EXPORT, CmExport)
TCM_EV_COMMAND(CM_IMPORT2, CmBackup)
TCM_EV_COMMAND(CM_EXPORT2, CmRestore)
TCM_EV_COMMAND(CM_IMPORT2, CmBackuptoJson)
TCM_EV_COMMAND(CM_EXPORT2, CmRestorefromJson)


//----------------------------------------------------------------------------------------
// NewSystemDialog commands

//Data insert actions
void                                  // Thermodynamic data
TVisorImp::CmOpen_MTPARAM()
{
    OpenModule(this, MD_MTPARM, 0, true);
}

void                                 // Bulk composition (b)
TVisorImp::CmOpen_SYSTEM_ICOMP()
{
    OpenModule(this, MD_SYSTEM, 0, true ); // ICOMP page
}

void                                 // Bulk composition (b)
TVisorImp::CmInsert_SYSTEM()
{
    string key_s = rt[RT_SYSEQ]->PackKey();
    TProfil::pm->systbcInput( this, key_s.c_str() );
    Update( true );
}

void TVisorImp::CmOutMulti()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmOutMulti();
}

void TVisorImp::CmReadMulti()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmReadMulti();
}

void TVisorImp::CmProjectExportJson()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn ) {
        wn->CmProjectExportJson();
    }
}

void TVisorImp::CmProjectImportJson()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn ) {
        wn->CmProjectImportJson();
        CmDataBaseMode();
        //Update( true );
    }
}

//----------------------------------------------------------------------------------------
//Calculate actions

void TVisorImp::CmRunBCC()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmRunBCC();
}


void TVisorImp::setActionPrecise()
{
    ui->sactionPrecise->setChecked( TProfil::pm->pa.p.PRD );
}

// Show resalts (view) actions
// Access to MULTI data
void                                  // Dual Solution (u), pH, pe, I
TVisorImp::CmOpen_EQSTAT_ICOMP()
{
    OpenModule(this, MD_EQDEMO, 0, true);
}


void                                  // Details for Experts
TVisorImp::CmOpen_MULTI()
{
    OpenModule(this, MD_MULTI, 0, true ); //... page
}

//----------------------------------------------------------------------------------------
// Print actions
void TVisorImp::CmPrintMtparam()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmPrintMtparam();
}

void TVisorImp::CmPrintSystem()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmPrintSystem();
}

void TVisorImp::CmPrintEqstat()
{                                // Only for non-zero DC, PH
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmPrintEqstat();
}


void TVisorImp::CmPrResults()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmPrResults();
}


void TVisorImp::CmPrInput()
{
    NewSystemDialog *wn = activeNewSystemCommand();
    if( wn )
        wn->CmPrInput();
}

//=============================================
// Open files list dialog
//=============================================
class TFileList:   public QDialog
{
    QListWidget* pFileList;
public:
    TFileList( QWidget* win, size_t iRt, const char* caption );
};

TFileList::TFileList(QWidget* win, size_t iRt, const char* caption ):
    QDialog( win )
{
    setWindowTitle(caption);

    pFileList = new QListWidget(this);
    const TCStringArray& s = rt[iRt]->GetOpenFiles();
    for( size_t ii=0; ii<s.size(); ii++ )
        pFileList->addItem(s[ii].c_str());

    pFileList->item(rt[iRt]->GetWorkFileName())->setSelected(true);

    QPushButton* btn;
    btn = new QPushButton(this);
    btn->setText("&Close");
    btn->setDefault(true);
    connect( btn, SIGNAL(clicked()), this, SLOT(close()) );

    QVBoxLayout* editBox = new QVBoxLayout( this );
    editBox->addWidget( pFileList);
    editBox->addWidget( btn );

}

void TVisorImp::EvFileDown()
{
    int nrt = nRTofActiveSubWindow();
    if( nrt<0 )
        return;
    TFileList dlg( this, static_cast<size_t>(nrt), "Info page" );
    dlg.exec();
}

// -------------- End of file module_w.cpp ----------------------


