//-------------------------------------------------------------------
// Id: gems/vizor/visor_w.h  version 2.0.0       2001
//
// Declaration of TVisorImp class
//
// Created : 970207    Modified: 010908
//
// Copyright (C) 1996-2001  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license #1435515
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

protected slots:
    void closeEvent( QCloseEvent* );
    void evHelpClosed();

public slots:
    bool SetProfileMode();
    bool SetGeneralMode();
    void ShowPhaseWindow();
    void ShowEqPhaseWindow();
    void CmExit();
    void CmHelp();
    void CmHelpAbout();
    void CmHelpMenuItem(); //  Added 05.01.01

    void NewSystem();
    void RemakeSystem();
    void LoadSystem();
    void SaveSystem();
    void CalcMulti();

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
    void OpenHelp(const char* file, const char* item=0, QWidget* parent=0);
    void OpenProgress(bool step=false);
    void CloseProgress();
    void Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total);
    void CloseMessage();

    void PrintText( const char* title, char* text);

/*    GraphDialog* MakePlot(TCModule *pmod, TIArray<TPlot>& plts, const char* title,
                      float* sizeReg, float* sizePrt,
                      TPlotLine* aLines, int aAxisType=0,
                      const char* aXName=0, const char* aYName=0);
    GraphDialog* MakePlot(TCModule *pmod, TIArray<TPlot>& plts, const char * title,
                      int aAxisType=0, const char* aXName=0, const char* aYName=0);

*/
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
    void setCellFont(const QFont& newCellFont)
    {
        CellFont = newCellFont;
    }

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
};

extern TVisorImp* pVisorImp;

#endif   // _visor_w_h
