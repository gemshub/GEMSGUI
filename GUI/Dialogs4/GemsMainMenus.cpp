//-------------------------------------------------------------------
// $Id: module_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of GemsMainWindow class
//
// Copyright (C) 2011  S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Selektor GUI library which uses the
// Qt v.4 cross-platform App & UI framework (http://qt.nokia.com)
// under LGPL v.2.1 (http://www.gnu.org/licenses/lgpl-2.1.html)
//
// This file may be distributed under the terms of GEMS3 Development
// Quality Assurance Licence (GEMS3.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#include <QtGui>

#include "visor.h"
#include "m_param.h"
//#include "v_mod.h"
#include "GemsMainWindow.h"
#include "SettingsDialog.h"
#include "NewSystemDialog.h"
#include "ProgressDialog.h"
#include "GraphDialog.h"
//#include "LoadMessage.h"

void TVisorImp::updateMenus()
{
   // addition work with MdiChild
    bool hasMdiChild = (mdiArea->activeSubWindow() != 0);

    menubar->clear();
    toolBar_2->hide();
    toolBar_3->hide();
    toolBar_4->hide();
    toolBar_5->hide();

    menubar->addAction(menu_Modules->menuAction());

    if( hasMdiChild )
    {
        TCModuleImp *mdwin = activeMdiChild();
        if( mdwin && mdwin->getViewMode()   )
        { // if we open graph, submodules and ... hide menu_record !!!
          //pModuleName->setText(mdwin->moduleName().c_str());
          if( mdwin->rtNum() >= MD_RMULTS
              && pVisor->ProfileMode == MDD_SYSTEM )
          {
            changeModulesKeys( RT_SYSEQ  );
            menubar->addAction(menu_Record->menuAction());
            action_Calculate->setEnabled(false);
            toolBar_2->show();
            toolBar_3->show();
            toolBar_5->show();
            menubar->addAction(smenu_Data->menuAction());
            menubar->addAction(smenu_Calc->menuAction());
            menubar->addAction(smenu_View->menuAction());
            menubar->addAction(smenu_Print->menuAction());
           }
          else
          {
             changeModulesKeys( mdwin->rtNum()  );
             menubar->addAction(menu_Record->menuAction());
             action_Calculate->setEnabled(true);
             toolBar_2->show();
             toolBar_4->show();
             toolBar_5->show();
             if( pVisor->ProfileMode == MDD_DATABASE)
             {
               menubar->addAction(menuRecord_List->menuAction());
               menubar->addAction(menu_Database_Files->menuAction());
              }
         }
        }
        else
        {   // for Chemical Systems must be other menu in future !!!
         NewSystemDialog *syswin = activeNewSystem();
         if( syswin )
         { // if we open graph, submodules and ... hide menu_record !!!
            //pModuleName->setText("SingleSystem");
            changeModulesKeys( RT_SYSEQ );
            menubar->addAction(menu_Record->menuAction());
            action_Calculate->setEnabled(false);
            toolBar_2->show();
            toolBar_3->show();
            toolBar_5->show();
            menubar->addAction(smenu_Data->menuAction());
            menubar->addAction(smenu_Calc->menuAction());
            menubar->addAction(smenu_View->menuAction());
            menubar->addAction(smenu_Print->menuAction());
           }
          else
            changeModulesKeys( -1 );
        }
      }
    else
      changeModulesKeys( -1 );

    if(hasMdiChild)
    {  int ndx = indexMdiChild( mdiArea->activeSubWindow()->widget() );
        if( ndx >= 0 )
          pModuleName->setCurrentIndex(ndx);
    }
    //else
    //    pModuleName->clear();

     menubar->addAction(menuWindow->menuAction());
     menubar->addAction(smenu_Help->menuAction());

     actClose->setEnabled(hasMdiChild);
     actCloseAll->setEnabled(hasMdiChild);
     actTile->setEnabled(hasMdiChild);
     actCascade->setEnabled(hasMdiChild);
     separatorAct->setVisible(hasMdiChild);
 }


//enabled = !(pVisor->ProfileMode == MDD_SYSTEM && LoadMessage::pDia );
void TVisorImp::setMenuEnabled( bool menuEnabled )
{
    menu_Record->setEnabled(menuEnabled);
    smenu_Data->setEnabled(menuEnabled);
    smenu_Calc->setEnabled(menuEnabled);
    smenu_View->setEnabled(menuEnabled);
    smenu_Print->setEnabled(menuEnabled);
    toolBar->setEnabled(menuEnabled);
    toolBar_2->setEnabled(menuEnabled);
    toolBar_4->setEnabled(menuEnabled);
    toolBar_3->setEnabled(menuEnabled);
    toolBar_5->setEnabled(menuEnabled);
    itemWidget1->setEnabled(menuEnabled);
}

gstring TVisorImp::nameMdiChild( QWidget *p )
{
    gstring mdName;

    TCModuleImp *child = qobject_cast<TCModuleImp *>(p);
    if(child)
      mdName = child->moduleName();
    else
     {
        NewSystemDialog *wn = qobject_cast<NewSystemDialog *>(p);
        if( wn )
         mdName = "SingleSystem";
        else
        {   GraphDialog *dlg = qobject_cast<GraphDialog *>(p);
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
    gstring iconName;

    TCModuleImp *child = qobject_cast<TCModuleImp *>(p);
    if(child)
      iconName = child->iconFile();
    else
     {
        NewSystemDialog *wn = qobject_cast<NewSystemDialog *>(p);
        if( wn )
         iconName = ":/modules/Icons/SysEqModuleIcon.png";
        else
        {   GraphDialog *dlg = qobject_cast<GraphDialog *>(p);
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
    menuWindow->clear();
    menuWindow->addAction(actClose);
    menuWindow->addAction(actCloseAll);
    menuWindow->addSeparator();
    menuWindow->addAction(actTile);
    menuWindow->addAction(actCascade);
    menuWindow->addAction(separatorAct);

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

        QAction *action  = menuWindow->addAction(text);
        action->setCheckable(true);
        action ->setChecked(windows.at(i) == mdiArea->activeSubWindow());
        connect(action, SIGNAL(triggered()), windowMapper, SLOT(map()));
        windowMapper->setMapping(action, windows.at(i));
    }
}


void  TVisorImp::moveToolBar( int , int )
{
   /* if(pVisor->ProfileMode == MDD_SYSTEM )
        toolBar->setFixedWidth(toolProject->size().width());
    else
        toolBar->setFixedWidth(toolDataBase->size().width());
   */
      if(pVisor->ProfileMode == MDD_SYSTEM )
        toolProject->setFixedWidth(toolBar->size().width());
    else
        toolDataBase->setFixedWidth(toolBar->size().width());

    toolModule->setFixedWidth(splH->widget(0)->width());
}

// -----------------------------------------------------------
// Actions and commands

//  Connect all actions
void TVisorImp::setActions()
{

 // Modules
    connect( actionIComp, SIGNAL( triggered()), this, SLOT(CmIComp()));
    connect( actionDComp, SIGNAL( triggered()), this, SLOT(CmDComp()));
    connect( actionReacDC, SIGNAL( triggered()), this, SLOT(CmReacDC()));
    connect( actionRTparm, SIGNAL( triggered()), this, SLOT(CmRTparm()));
    connect( actionPhase, SIGNAL( triggered()), this, SLOT(CmPhase()));
    connect( actionCompos, SIGNAL( triggered()), this, SLOT(CmCompos()));

    connect( actionSysEq, SIGNAL( triggered()), this, SLOT(CmSysEq()));
    connect( actionProcess, SIGNAL( triggered()), this, SLOT(CmProcess()));
    connect( actionGEM2MT, SIGNAL( triggered()), this, SLOT(CmGEM2MT()));
    connect( actionGtDemo, SIGNAL( triggered()), this, SLOT(CmGtDemo()));
    connect( actionDualTh, SIGNAL( triggered()), this, SLOT(CmDualTh()));
    connect( actionUnSpace, SIGNAL( triggered()), this, SLOT(CmUnSpace()));
    connect( actionProject, SIGNAL( triggered()), this, SLOT(CmProject()));
    connect( actionSDref, SIGNAL( triggered()), this, SLOT(CmSDref()));
    connect( actionConst, SIGNAL( triggered()), this, SLOT(CmConst()));
    connect( actionExit, SIGNAL( triggered()), this, SLOT(close()));

 // Help
    connect( sactionHelp, SIGNAL( triggered()), this, SLOT(CmHelp()));
    connect( sactionHowto, SIGNAL( triggered()), this, SLOT(CmHowto()));
    connect( sactionScript, SIGNAL( triggered()), this, SLOT(CmScript()));
    connect( sactionMoreBCC, SIGNAL( triggered()), this, SLOT(CmMoreBCC()));
    connect( sactionMoreIPM, SIGNAL( triggered()), this, SLOT(CmMoreIPM()));
    connect( sactionAbout, SIGNAL( triggered()), this, SLOT(CmHelpAbout()));
    connect( actionAuthors, SIGNAL( triggered()), this, SLOT(CmHelpAuthors()));
    connect( actionThanks, SIGNAL( triggered()), this, SLOT(CmHelpThanks()));
    connect( actionLicense, SIGNAL( triggered()), this, SLOT(CmHelpLicense()));

    connect( action_Help_2, SIGNAL( triggered()), this, SLOT(CmHelp2()));
    connect( action_Preferences, SIGNAL( triggered()), this, SLOT(CmSettingth()));
    connect( actionTutorial, SIGNAL( triggered()), this, SLOT(CmTutorial()));

// Record
    connect( action_Create, SIGNAL( triggered()), this, SLOT(CmCreate()));
    connect( action_New, SIGNAL( triggered()), this, SLOT(CmNew()));
    connect( action_Display, SIGNAL( triggered()), this, SLOT(CmShow()));
    connect( action_Remake, SIGNAL( triggered()), this, SLOT(CmDerive()));
    connect( action_Calculate, SIGNAL( triggered()), this, SLOT(CmCalc()));
    connect( action_Save, SIGNAL( triggered()), this, SLOT(CmSave()));
    connect( actionSave_As, SIGNAL( triggered()), this, SLOT(CmSaveAs()));
    connect( action_Delete, SIGNAL( triggered()), this, SLOT(CmDelete()));
    connect( action_Plot, SIGNAL( triggered()), this, SLOT(CmPlot()));
    connect( action_Print, SIGNAL( triggered()), this, SLOT(CmPrint()));
    //connect( action_Close, SIGNAL( triggered()), this, SLOT(CloseWin()));
    connect( actionNext, SIGNAL( triggered()), this, SLOT(CmNext()));
    connect( actionPrevious, SIGNAL( triggered()), this, SLOT(CmPrevious()));
    connect( actionFilter, SIGNAL( triggered()), this, SLOT(CmFilter()));
    connect( actionFile, SIGNAL( triggered()), this, SLOT(EvFileDown()));

    // Record list
       connect( action_Copy, SIGNAL( triggered()), this, SLOT(CmCopyList()));
       connect( action_Rename, SIGNAL( triggered()), this, SLOT(CmRenameList()));
       connect( action_Move, SIGNAL( triggered()), this, SLOT(CmTransferList()));
       connect( action_Delete_list, SIGNAL( triggered()), this, SLOT(CmDeleteList()));
       connect( action_List, SIGNAL( triggered()), this, SLOT(CmKeysToTXT()));
       connect( action_Backup, SIGNAL( triggered()), this, SLOT(CmBackup()));
       connect( action_Restore, SIGNAL( triggered()), this, SLOT(CmRestore()));
       connect( action_Export, SIGNAL( triggered()), this, SLOT(CmExport()));
       connect( action_Import, SIGNAL( triggered()), this, SLOT(CmImport()));

    // Database files
       connect( action_Selection, SIGNAL( triggered()), this, SLOT(CmReOpenFileList()));
       connect( action_Add_Link, SIGNAL( triggered()), this, SLOT(CmAddOpenFile()));
       connect( action_New_2, SIGNAL( triggered()), this, SLOT(CmAddFileToList()));
       connect( action_Compress, SIGNAL( triggered()), this, SLOT(CmRebildFile()));

    // Window
    connect( actClose, SIGNAL(triggered()), mdiArea, SLOT(closeActiveSubWindow()));
    connect(actCloseAll, SIGNAL(triggered()), mdiArea, SLOT(closeAllSubWindows()));
    connect(actTile, SIGNAL(triggered()), mdiArea, SLOT(tileSubWindows()));
    connect(actCascade, SIGNAL(triggered()), mdiArea, SLOT(cascadeSubWindows()));
    separatorAct = new QAction(this);
    separatorAct->setSeparator(true);
    menuWindow->addAction(separatorAct);
    updateWindowMenu();
    connect(menuWindow, SIGNAL(aboutToShow()), this, SLOT(updateWindowMenu()));

    //NewSystemDialog
    connect( sactionOpenMTPARAM, SIGNAL( triggered()), this, SLOT(CmOpen_MTPARAM()));
    connect( sactionOpen_SYSTEM_ICOMP, SIGNAL( triggered()), this, SLOT(CmOpen_SYSTEM_ICOMP()));
    connect( sactionINSERT_SYSTEM, SIGNAL( triggered()), this, SLOT(CmInsert_SYSTEM()));
    connect( sactionOutMulti, SIGNAL( triggered()), this, SLOT(CmOutMulti()));
    connect( sactionReadMulti, SIGNAL( triggered()), this, SLOT(CmReadMulti()));

    connect( saction_BCC, SIGNAL( triggered()), this, SLOT(CmRunBCC()));
    connect( saction_IPM, SIGNAL( triggered()), this, SLOT(CmRunIPM()));
    //connect( actionSimplex, SIGNAL( triggered()), this, SLOT(CmSimplex()));
    //connect( actionPrecise, SIGNAL( triggered()), this, SLOT(CmPrecise()));
    //connect( actionStepwise, SIGNAL( triggered()), this, SLOT(CmStepwise()));

    connect( sactionEQUSTAT_ICOMP, SIGNAL( triggered()), this, SLOT(CmOpen_EQSTAT_ICOMP()));
    connect( sactionOpenMULTI, SIGNAL( triggered()), this, SLOT(CmOpen_MULTI()));

    connect( sactionPrintMtparm, SIGNAL( triggered()), this, SLOT(CmPrintMtparam()));
    connect( sactionPrintSystem, SIGNAL( triggered()), this, SLOT(CmPrintSystem()));
    connect( sactionPrintEqstat, SIGNAL( triggered()), this, SLOT(CmPrintEqstat()));
    connect( sactionPrInput, SIGNAL( triggered()), this, SLOT(CmPrInput()));
    connect( sactionPrResults, SIGNAL( triggered()), this, SLOT(CmPrResults()));


    //pModuleName = new QLabel( toolModule );
    pModuleName = new QComboBox( toolModule );
    pModuleName->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
    connect( pModuleName, SIGNAL( currentIndexChanged(int)), this, SLOT(setActiveSubWindowIdex(int)));
    toolModule->addWidget( pModuleName );

    //pModeName = new QLabel( toolBar );
    //QFont fnt = pModeName->font();
    //fnt.setBold(true);
    //fnt.setPointSize(12);
    //fnt.setStyleHint(QFont::Decorative);
    //pModeName->setFont(fnt);
    //toolBar->addWidget( pModeName );

    //
   pLine = new QLineEdit( toolBar_6 );
   pLine->setEnabled( TRUE );
   pLine->setFocusPolicy( Qt::ClickFocus );
   pLine->setReadOnly( TRUE );
   pLine->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   toolBar_6->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );
   toolBar_6->addWidget( pLine ); // setStretchableWidget( pLine );

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
    aMod[actwin->rtNum()].CmHelp();
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
    //AboutDialog dlg(this);
    //dlg.exec();
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

     ((TCModule*)&aMod[actwin->rtNumRecord()])->CmScript();
  }
  else
  {   NewSystemDialog *wn = activeNewSystem();
      if( wn )
         ((TCModule*)&aMod[RT_SYSEQ])->CmScript();
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
      ((TCModule*)&aMod[actwin->rtNumRecord()])->CmCreate();
     else
      ((TCModule*)&aMod[actwin->rtNumRecord()])->CmCreateinProfile();
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
          ((TCModule*)&aMod[actwin->rtNumRecord()])->CmNew();
        else
          ((TCModule*)&aMod[actwin->rtNumRecord()])->CmNewinProfile();
     }
  }
}

//TCM_EV_COMMAND(CM_11, CmLoadinProfile);
void TVisorImp::CmShow( const char * key )
{
  NewSystemDialog *wn = activeNewSystemCommand();
  if( wn )
         wn->CmSelect( key );
  else
  {
    TCModuleImp *actwin = activeMdiChild();
    if( actwin )
    {
      if(pVisor->ProfileMode == MDD_DATABASE)
       ((TCModule*)&aMod[actwin->rtNumRecord()])->CmShow( key );
      else
       ((TCModule*)&aMod[actwin->rtNumRecord()])->CmLoadinProfile( key );
      QMdiSubWindow * grDlg = findMdiGraph(actwin->moduleName().c_str());
      if( grDlg )
      {
        QMdiSubWindow *wn = findMdiChild(actwin->moduleName().c_str());
        //mdiArea->setActiveSubWindow( wn );
        mdiArea->setActiveSubWindow( grDlg );
        mdiArea->closeActiveSubWindow();
        mdiArea->setActiveSubWindow( wn );
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
       ((TCModule*)&aMod[actwin->rtNumRecord()])->CmDerive();
  }
 }

//TCM_EV_COMMAND(CM_RECCALC, CmCalc);
void TVisorImp::CmCalc()
{
  TCModuleImp *actwin = activeMdiChild();
  if( actwin )
    ((TCModule*)&aMod[actwin->rtNumRecord()])->CmCalc();
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
      ((TCModule*)&aMod[actwin->rtNumRecord()])->CmSaveM();
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
       ((TCModule*)&aMod[actwin->rtNumRecord()])->CmSaveAs();
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
    {  ((TCModule*)&aMod[actwin->rtNumRecord()])->CmDelete();
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
     ((TCModule*)&aMod[actwin->rtNumRecord()])->CmPlot();
  }
  else
  {   NewSystemDialog *wn = activeNewSystem();
      if( wn )
         ((TCModule*)&aMod[RT_SYSEQ])->CmPlot();
  }
}

//TCM_EV_COMMAND(CM_PRINT, CmPrint);
void TVisorImp::CmPrint()
{
  TCModuleImp *actwin = activeMdiChild();
  if( actwin )
  {
     ((TCModule*)&aMod[actwin->rtNumRecord()])->CmPrint();
  }
  else
  {   NewSystemDialog *wn = activeNewSystem();
      if( wn )
       //  ((TCModule*)&aMod[RT_SYSEQ])->CmPrint();
        wn->CmPrintEqstat();
  }
}


void TVisorImp::CmFilter()
{
  TCModuleImp *actwin = activeMdiChild();
  if( actwin )
  {
     ((TCModule*)&aMod[actwin->rtNum()])->CmFilter();
     pFilterKey->setText(((TCModule*)&aMod[actwin->rtNumRecord()])->getFilter());
      defineModuleKeysList( actwin->rtNumRecord() );
  }
  else
  {   NewSystemDialog *wn = activeNewSystem();
      if( wn )
      {   ((TCModule*)&aMod[RT_SYSEQ])->CmFilter();
          pFilterKey->setText(((TCModule*)&aMod[RT_SYSEQ])->getFilter());
          defineModuleKeysList( RT_SYSEQ );
      }
  }
}


void TVisorImp::CmNext()
{
  /*TCModuleImp *actwin = activeMdiChild();
  if( actwin )
    ((TCModule*)&aMod[actwin->rtNum()])->CmNext();
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
    ((TCModule*)&aMod[actwin->rtNum()])->CmPrevious();
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
                                 {  ((TCModule*)&aMod[actwin->rtNum()])->ff(); \
                                    defineModuleKeysList( actwin->rtNum() );  }    \
                                }

TCM_EV_COMMAND( CM_REBILDFILE, CmRebildFile);
TCM_EV_COMMAND(CM_ADDFILETOLIST, CmAddFileToList);
TCM_EV_COMMAND(CM_ADDOPENFILE, CmAddOpenFile);
TCM_EV_COMMAND(CM_REOPENFILELIST, CmReOpenFileList);

TCM_EV_COMMAND(CM_CHOISELIST, CmCopyList);
TCM_EV_COMMAND(CM_RENAMELIST, CmRenameList);
TCM_EV_COMMAND(CM_TRANSFERLIST, CmTransferList);
TCM_EV_COMMAND(CM_DELETELIST, CmDeleteList);
TCM_EV_COMMAND(CM_KEYSTOTXT, CmKeysToTXT);
TCM_EV_COMMAND(CM_IMPORT, CmImport);
TCM_EV_COMMAND(CM_EXPORT, CmExport);
TCM_EV_COMMAND(CM_IMPORT2, CmBackup);
TCM_EV_COMMAND(CM_EXPORT2, CmRestore);

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
    gstring key_s = rt[RT_SYSEQ].PackKey();
    TProfil::pm->systbcInput( this, key_s.c_str() );
    Update( true );
}

void TVisorImp::CmOutMulti()
{
    NewSystemDialog *wn = activeNewSystemCommand();;
    if( wn )
        wn->CmOutMulti();
}

void TVisorImp::CmReadMulti()
{
     NewSystemDialog *wn = activeNewSystemCommand();
     if( wn )
       wn->CmReadMulti();
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
   sactionPrecise->setChecked( TProfil::pm->pa.p.PRD );
}

void TVisorImp::CmRunIPM()
{
    TMulti::sm->GetPM()->pNP =
        ( sactionSimplex->isChecked())? 0: 1;
    if( sactionPrecise->isChecked() && !TProfil::pm->pa.p.PRD )
          TProfil::pm->pa.p.PRD = -5; // Changed
    TProfil::pm->pa.p.PRD =
        ( !sactionPrecise->isChecked())? 0: TProfil::pm->pa.p.PRD;

    try
    {

#ifdef Use_mt_mode
       TProfil::pm->userCancel = false;

        ProgressDialog* dlg = new ProgressDialog(this,sactionStepwise->isChecked()  );

        dlg->show();
#else

        if( !menu->isItemChecked( pStepwiseCheck ) )
        {
           CalcMulti();
        }
#endif
        Update(true);
    }
    catch( TError& err)
    {
        Update(true);
        vfMessage(this, err.title, err.mess);
    }
    NewSystemDialog::pDia->setCurrentTab(1);
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
        TFileList( QWidget* win, int iRt, const char* caption );
};

TFileList::TFileList(QWidget* win, int iRt, const char* caption ):
        QDialog( win )
{
   setWindowTitle(caption);

   pFileList = new QListWidget(this);
   const TCStringArray& s = rt[iRt].GetOpenFiles();
   for( uint ii=0; ii<s.GetCount(); ii++ )
              pFileList->addItem(s[ii].c_str());

        pFileList->item(rt[iRt].GetWorkFileName())->setSelected(true);

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
    TFileList dlg( this, nrt, "Info page" );
    dlg.exec();
}

// -------------- End of file module_w.cpp ----------------------


