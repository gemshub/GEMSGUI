//-------------------------------------------------------------------
// $Id: visor_w.h 968 2007-12-13 13:23:32Z gems $
//
// Declaration of TVisorImp class
//
// Copyright (C) 1996-2008  A.Rysin, S.Dmytriyeva
// Uses  gstring class (C) A.Rysin 1999
//
// This file is part of the GEM-Vizor library which uses the
// Qt v.2.x GUI Toolkit (Troll Tech AS, http://www.trolltech.com)
// according to the Qt Duo Commercial license
//
// This file may be distributed under the terms of the GEMS-PSI
// QA Licence (GEMSPSI.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail gems2.support@psi.ch
//-------------------------------------------------------------------

#ifndef _visor_w_h
#define _visor_w_h

#include <QTimer>
#include <QPainter>
#include <QMainWindow>
#include <QFont>
#include <QWaitCondition>
#include <QCloseEvent>
#include <QPixmap>
#include <QProcess>

#include "v_vals.h"
using namespace std;

class TCModule;
// class Helper;

#ifdef QT_NO_DEBUG

#define GEMS_RELEASE

#endif

enum myThreadEvents { thMessage = 0, thQuestion, thQuestion3,
                      thChoice, thChoice2, thExcludeFillEdit  };
// data for thread
struct DThread
{
  int res;
  bool wait;
  //vfMessage & vfQuestion
  gstring title;     //prompt(vfChoice), caption
  gstring mess;
  //vfQuestion3
  gstring s1;
  gstring s2;
  gstring s3;
  //vfChoice (error, open only one SysEq) 
  TCStringArray list;
  int seli;   
  //vfChoise3
  bool all; // only return
  //vfExcludeFillEdit
  TOArray<bool> sel;
  double fill_data;

  //Message&Quastion&Question3
  void setDThread( const gstring& title_, const gstring& mess_, 
		  const gstring& s1_="", const gstring& s2_="", const gstring& s3_="")
  {  res =0;
     wait = true;
     title = title_;
     mess =mess_;
     s1 =s1_;
     s2 =s2_;
     s3 =s3_;
  }
  //vfChoice&vfChoice2
  void setDThread( TCStringArray& arr, const char* prompt, int sel_, bool all_=false )
  {  res =0;
     wait = true;
     title = prompt;
     mess ="";
     all = all_;
     seli = sel_;
     list.Clear();
     for( uint ii=0; ii<arr.GetCount(); ii++ )
       list.Add( arr[ii]);
  }
  //vfExcludeFillEdit
  void setDThread( const char* caption, TCStringArray& aICkeys,
             double fill_data_ )
  {  res = 0;
     wait = true;
     title = caption;
     fill_data = fill_data_;
     list.Clear();
     for( uint ii=0; ii<aICkeys.GetCount(); ii++ )
       list.Add( aICkeys[ii]);
  }
};


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

    size_t updateTime;

    // Appearance settings
    QPixmap*    pixLogo;
    QPixmap*    pixSys;
    QFont	CellFont;
    QFont	axisLabelFont;
    QFont	defaultFont;
    //  int	DoubleDigits;
    // other settings
    bool	configAutosave;

    int charWidth;
    int charHeight;

    char TCpoint[32];  // Step point ID for stepwise mode

// old help data
//    Helper *assistantClient;

protected slots:
    void closeEvent( QCloseEvent* );
//    void evHelpClosed();

public slots:
    bool SetProfileMode();
    bool SetGeneralMode();
    void CmExit();
    void CmHelp();
    void CmHelpAbout();
    void CmHelpMenuItem(); //  Added 05.01.01
    void CalcMulti();
    void SaveSystem();
    void Update(bool force);
    void theadService( int nFunction, QWidget* par );
    
public:
    DThread thdata;
    QProcess *proc;

    TVisorImp(int c, char** v);
    ~TVisorImp();

//nmt    void CalcFinished();
    //  void SetStatus(const char* txt);

    bool MakeQuery(TCModule& m);

    void SetDialog(QWidget* centralDialog);

    void OpenModule(QWidget* parent, int i, int page=0, bool viewmode=false, bool select=false);
    void GetHelp();
    void OpenHelp(const char* file, const char* item=0);
    void OpenProgress(bool step=false);
    void CloseProgress();
    bool Message( QWidget* parent, const char* name,
             const char* msg, int prog, int total, bool move);
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
    const QFont& getAxisLabelFont() const
    {
        return axisLabelFont;
    }
    const QFont& getDefaultFont() const
    {
        return defaultFont;
    }    

    void setCellFont(const QFont& newCellFont);
    void setAxisLabelFont(const QFont& newAxisLabelFont) 
    {
	axisLabelFont = newAxisLabelFont;
    }
    int getCharWidth() const;
    int getCharHeight() const;
    int getLabelWidth() const;
    int getCellWidth() const;

    int getDoubleDigits() const
    {
        return TValBase::doublePrecision;
    }
    void setDoubleDigits(int newDoubleDigits)
    {
        TValBase::doublePrecision = newDoubleDigits;
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
    QMutex& getMutexCalc();

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
extern Qt::HANDLE pThread;

#endif   // _visor_w_h
