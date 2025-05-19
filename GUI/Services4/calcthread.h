// $Id: calcthread.h 710 2006-06-07 14:58:15Z gems $

/*

    Class CalcThread gives CalcEqstat functions different thread
	of execution, this easies TProject <==> ProgressDialog
	interaction in stepwise mode
	You hould have -DQT_THREAD_SUPPORT=1 in your compiler options
	and also you should link agaisnt threaded QT (e.g. libqt-mt.so in linux)
	
	Andy Rysin (c) 2001
	GEMS Project
*/

#ifndef calcthread_included
#define calcthread_included

#include <QMutex>
#include <QThread>
#include <QWaitCondition>

#include "m_param.h"
#include "m_proces.h"
#include "m_gem2mt.h"
#include "stepwise.h"
#include "GemsMainWindow.h"

class NewThread: public QThread 
{
    Q_OBJECT

    //  mutable QMutex mutex;
    //  mutable QWaitCondition cond;
    bool quit;

signals:
    void sgUpdate(bool);

public slots:
    void emitUpdate(bool force)
    {  emit sgUpdate(force); }
    void emitWakeOne()
    {  ThreadControl::wakeOne(); }

public:

    mutable int result;

    NewThread( QObject *parent ): QThread(parent)
    {
        // pVisorImp->getMutexCalc().lock();
        quit = false;
    }
    
    ~NewThread() 
    {
        quit = true;
        ThreadControl::wakeOne();	// let's calc
        //    	cond.wakeOne();
        wait();
    }

    TError error;	// error message and title
    

    // put the code here to clean up after
    // calculations have been cancelled (or finished)
    void clean_up() { }
    
};


class CalcThread: public NewThread 
{
    Q_OBJECT

public:
    CalcThread(QObject *parent ): NewThread(parent) {}
    ~CalcThread() {}
    

    void run()
    {
        error.title = "";
        error.mess = "";
        try
        {
            QMutexLocker  loker(&pVisorImp->getMutexCalc());
            showMss = 1L;
            double dummy = -1.;
            TProfil::pm->CalcEqstat( dummy, -1, 0. );
        }
        catch( TError& err )
        {  error = err; }
        catch( TMulti::UserCancelException& /*ex*/ )
        {}
    }

};

class ProcessThread: public NewThread 
{
		Q_OBJECT

public:
    ProcessThread(QObject *parent):NewThread(parent) 
    {}
    ~ProcessThread() {}

    TError msg;	        // message&questin message and title

    void run()
    {
	   error.title = "";
	   error.mess = "";
	  try {
             QMutexLocker  loker(&pVisorImp->getMutexCalc());
             TProcess::pm->internalCalc();
	      }
	  catch( TError& err ) 
	  {  error = err; 	}
      catch( TProcess::UserCancelException& /*ex*/ )
	  { 	}
   }
    
};

class GEM2MTThread: public NewThread
{
                Q_OBJECT

public:
    GEM2MTThread(QObject *parent):NewThread(parent)
    {}
    ~GEM2MTThread() {}

    TError msg;	        // message&questin message and title

    void run()
    {
           error.title = "";
           error.mess = "";
          try {
             QMutexLocker  loker(&pVisorImp->getMutexCalc());
             TGEM2MT::pm->internalCalc();
              }
          catch( TError& err )
          {  error = err; 	}
          catch( TGEM2MT::UserCancelException& /*ex*/ )
          { 	}
   }

};

#endif // calcthread_included
