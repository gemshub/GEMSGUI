/*

    Class CalcThread gives CalcEqstat functions different thread
	of execution, this easies TProfile <==> ProgressDialog
	interaction in stepwise mode
	You hould have -DQT_THREAD_SUPPORT=1 in your compiler options
	and also you should link agaisnt threaded QT (e.g. libqt-mt.so in linux)

	Andy Rysin (c) 2001
	GEMS Project
*/

#ifdef __unix

//#define Use_mt_mode


#endif // Use_mt_mode
