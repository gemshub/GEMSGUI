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

#include <qthread.h>
#include "m_param.h"

class CalcThread:
    public QThread {
    
public:
    CalcThread() {}
    ~CalcThread() {}
    
    TError error;	// error message and title
    
    void run() {
	error.title = "";
	error.mess = "";
	try {
    	    TProfil::pm->CalcEqstat(false);
	}
	catch( TError& err ) {
	    error = err;
	}
	catch( TProfil::UserCancelException& ex ) {
	}
    }
    
    // put the code here to clean up after
    // calculations have been cancelled (or finished)
    void clean_up() {
    }
};

#endif // calcthread_included
