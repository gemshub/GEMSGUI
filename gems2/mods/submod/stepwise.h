
#define STEP_POINT() { \
	if( stepWise ) { \
    	    pVisorImp->getWaitProgress().wakeOne(); \
    	    pVisorImp->getWaitCalc().wait(); \
	} \
        if( userCancel ) \
            throw UserCancelException(); \
	}

