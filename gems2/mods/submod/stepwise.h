// $Id$

#ifdef Use_mt_mode

class ThreadControl {
    
 public:
    static void wakeOne();
    static bool wait ();
    static bool wait ( unsigned long time );
};


#define STEP_POINT() { \
	if( stepWise ) { \
    	    ThreadControl::wakeOne(); \
    	    ThreadControl::wait(); \
	} \
        if( userCancel ) \
            throw UserCancelException(); \
	}

#else

#define STEP_POINT()

#endif
