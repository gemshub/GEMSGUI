//-------------------------------------------------------------------
// $Id$
//
// Declaration of TVisorImp class
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://les.web.psi.ch/Software/GEMS-PSI/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _visor_w_h
#define _visor_w_h

#include <qthread.h>
#include <qmainwindow.h>
#include <qfont.h>

#include "v_vals.h"
#include "graph.h"


class TCModule;
class HelpWindow;

/*!
 \class TVisorImp
 \brief This class handles work of TVisor with real GUI classes
*/

class TVisorImp:
            public QMainWindow
{
    Q_OBJECT

    int argc;
    char** argv;

    HelpWindow* pHelpWidget;
    size_t updateTime;

    // Appearance settings
    QPixmap*    pixLogo;
    QPixmap*    pixSys;
    QFont	CellFont;
    //  int	DoubleDigits;
    // other settings
    bool	configAutosave;

    int charWidth;
    int charHeight;

    char TCpoint[32];  // Step point ID for stepwise mode

protected slots:
    void closeEvent( QCloseEvent* );
    void evHelpClosed();

public slots:
    bool SetProfileMode();
    bool SetGeneralMode();
//    void ShowPhaseWindow();
//    void ShowEqPhaseWindow();
    void CmExit();
    void CmHelp();
    void CmHelpAbout();
    void CmHelpMenuItem(); //  Added 05.01.01

    void CalcMulti();
    void SaveSystem();
    
public:
    TVisorImp(int c, char** v);
    ~TVisorImp();

    void Update(bool force);
    void CalcFinished();
    //  void SetStatus(const char* txt);

    //  TCModuleImp* OpenModule(int im, int page=0, bool viewmode=false);
    bool MakeQuery(TCModule& m);

    void SetDialog(QWidget* centralDialog);

    void OpenModule(QWidget* parent, int i, int page=0, bool viewmode=false);
    void OpenHelp(const char* file, const char* item=0, QWidget* parent=0, bool modal=false);
    void OpenProgress(bool step=false);
    void CloseProgress();
    void Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total);
    void CloseMessage();
    void ProcessProgress( QWidget* parent );

    void PrintText( const char* title, char* text);

    unsigned short updateInterval() const
    {
        return updateTime;
    }

    void setUpdateInterval(unsigned short updInterval)
    {
	if( updInterval > 0 && updInterval < 60 )
    	    updateTime = updInterval;
    }

    const char* version();
    const char* getGEMTitle();
    const QPixmap& getLogo() const
    {
        return *pixLogo;
    }
    const QPixmap& getSysIc() const
    {
        return *pixSys;
    }

    const QFont& getCellFont() const
    {
        return CellFont;
    }
    
    void setCellFont(const QFont& newCellFont);
    int getCharWidth() const;
    int getCharHeight() const;
    int getLabelWidth() const;
    int getCellWidth() const;

    int getDoubleDigits() const
    {
        return TValBase::doublePrecision - 1;
    }
    void setDoubleDigits(int newDoubleDigits)
    {
        TValBase::doublePrecision = newDoubleDigits + 1;
    }

    void setConfigAutosave(bool autosave)
    {
        configAutosave = autosave;
    }

    bool getConfigAutosave() const
    {
        return configAutosave;
    }


// functions for threaded calculation
    QWaitCondition& getWaitProgress();
    QWaitCondition& getWaitCalc();

    char* getTCpoint()
    {
      return TCpoint;
    }

    void setTCpoint( const char* str )
    {
      strncpy( TCpoint, str, 15 );
    }
    
};

extern TVisorImp* pVisorImp;

#endif   // _visor_w_h
