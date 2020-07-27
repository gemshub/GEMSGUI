//-------------------------------------------------------------------
// $Id: module_w.cpp 968 2007-12-13 13:23:32Z gems $
//
// Implementation of TCModuleImp class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
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

#include <QCloseEvent>
#include <QVBoxLayout>
#include <QHBoxLayout>


#include "GemsMainWindow.h"
#include "visor.h"
#include "module_w.h"
#include "page_w.h"
#include "v_mod.h"

/*!
    The constructor
*/

TCModuleImp::TCModuleImp(size_t irt, int page, int aviewmode):
        QDialog(0),
        iMod(irt),
        rMod( aMod[irt] ),
        last_update( 0 ),
        viewmode(aviewmode)
{
   //setupUi(this);
   //(void)statusBar();
    setAttribute( Qt::WA_DeleteOnClose );

    std::string s = rMod.GetName();
    s += " :: ";
    rMod.SetTitle();
    s += rMod.GetString();
    SetCaption( s.c_str() );

    // size is set up from TCWindow
    //  resize(rInfo.init_width, rInfo.init_height);
    //setMinimumSize( 300, 200 );

    rMod.pImp = this;
    pWin = new TCWindow(this, aWinInfo[iMod], page);
    //setCentralWidget(pWin);
    QVBoxLayout* mainBox = new QVBoxLayout(this);
    mainBox->addWidget( pWin );
    rMod.Setup();
}

/*!
   The desctructor
*/
TCModuleImp::~TCModuleImp()
{}

size_t TCModuleImp::rtNum() const
{
    return   rMod.rtNum();
}

size_t TCModuleImp::rtNumRecord() const
{
    if( rMod.IsSubModule() )
        return  RT_SYSEQ;
      else
        return   rMod.rtNum();
}

std::string TCModuleImp::iconFile() const
{
    if( rMod.IsSubModule() )
        return  aMod[RT_SYSEQ].GetIcon();
      else
        return  rMod.GetIcon();
}

int TCModuleImp::curPage()
 {
     return pWin->curPage();
 }

void TCModuleImp::closeEvent(QCloseEvent* e)
{
    // focus out for last focused widget
    //if( qApp->focusWidget() )  // ???? not need
    //    qApp->focusWidget()->clearFocus();
    // save state for user window

    if( !(windowState() & Qt::WindowMaximized) )
    {
        aWinInfo[iMod].init_width = parentWidget()->width();
        aWinInfo[iMod].init_height = parentWidget()->height();
    }

    // close module
    if( rMod.EvClose() ) 
    {
	  pWin->close();
	  rMod.pImp = NULL;
          e->accept();
          pVisorImp->closeMdiChild( this );
    }
    else
        e->ignore();
}


QSize TCModuleImp::sizeHint() const
{
    return QSize(aWinInfo[iMod].init_width, aWinInfo[iMod].init_height);
}

/*!
    Updates screen contents of the module
    if update interval elapsed since the last update 
*/
void TCModuleImp::Update(bool force)
{
    // Sveta no update windows if Systat calc
    if( !force )
        return;

    pWin->Update();
    std::string s = rMod.GetName();
    s += " :: ";
    s += rMod.GetString();
    SetCaption( s.c_str() );

    last_update = time(0);
}

/*void TCModuleImp::Raise(int page)
{
    show();
    raise();
    pWin->EmitEvTabSelChange(page);
}*/

void TCModuleImp::SelectStart()
{
   //if( qApp->focusWidget() )
   //   qApp->focusWidget()->clearFocus();

   if( iMod >= (int)rt.GetCount() || rt[iMod].RecCount() <= 0)
       return;   // Added to avoid a selection

   if( pVisor->ProfileMode )
   {
       if( iMod <= RT_SYSEQ && iMod != RT_SDATA)
        return;
       ((TCModule*)&rMod)->CmLoadinProfile();
    }
     else
      ((TCModule*)&rMod)->CmShow();
}

/*!
    Starts dialog for remake parameters
*/
void TCModuleImp::MakeQuery()
{
    TQueryWindow qd(aWinInfo[iMod]);
    qd.exec();

    Update(true);
}

void TCModuleImp::saveGraphData(jsonui::ChartData *data)
{
    rMod.SaveChartData(data);
    cout<< "saveGraphData" << endl;
}

// -------------- End of file module_w.cpp ----------------------


