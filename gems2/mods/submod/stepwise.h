// $Id$

#ifdef Use_mt_mode

#define STEP_POINT() { \
	if( stepWise ) { \
    	    pVisorImp->getWaitProgress().wakeOne(); \
    	    pVisorImp->getWaitCalc().wait(); \
	} \
        if( userCancel ) \
            throw UserCancelException(); \
	}

#else

#define STEP_POINT()

#endif
