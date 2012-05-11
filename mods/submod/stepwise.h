// $Id$

#ifndef _stepwise_h_
#define _stepwise_h_

#ifdef Use_mt_mode

class ThreadControl {

 public:
    static void wakeOne();
    static void wakeAll();
    static bool wait ();
    static bool wait ( unsigned long time );
    static char* GetPoint();
    static void SetPoint( const char* str );
};


#define STEP_POINT( sp ) { \
	if( TProfil::pm->stepWise ) { \
            ThreadControl::SetPoint( sp ); \
    	    ThreadControl::wakeOne(); \
    	    ThreadControl::wait(); \
	} \
        if( TProfil::pm->userCancel ) \
            throw UserCancelException(); \
	}

#define STEP_POINT2() { \
	if( stepWise ) { \
           ThreadControl::wait(); \
	} \
        if( userCancel ) \
           throw UserCancelException(); \
	}


#else

#define STEP_POINT( ap )

#endif

#endif
